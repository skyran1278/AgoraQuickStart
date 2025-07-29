#include "pch.h"

//

#include "AgoraManager.h"

// Insert your project's App ID obtained from the Agora Console
#define APP_ID "71aa4763f35149369959d89afe2e504c"

AgoraManager::AgoraManager()
    : m_rtcEngine(nullptr),
      m_initialize(false),
      m_remoteRender(false),
      m_videoTrackId(-1),
      m_stopCapture(false) {}

AgoraManager::~AgoraManager() { release(); }

bool AgoraManager::initialize(HWND hWnd) {
  // Set the message receiver for event handler
  m_eventHandler.SetMsgReceiver(hWnd);

  // Create IRtcEngine object
  m_rtcEngine = createAgoraRtcEngine();
  if (!m_rtcEngine) {
    return false;
  }

  // Create IRtcEngine context object
  RtcEngineContext context;
  // Input your App ID
  context.appId = APP_ID;
  // Add event handler for callbacks and events
  context.eventHandler = &m_eventHandler;

  // Initialize
  int ret = m_rtcEngine->initialize(context);
  m_initialize = (ret == 0);

  if (m_initialize) {
    // Enable the video module
    m_rtcEngine->enableVideo();

    // For creating multiple custom video tracks, call createCustomVideoTrack
    // multiple times
    m_videoTrackId = m_rtcEngine->createCustomVideoTrack();
    m_mediaEngine.queryInterface(m_rtcEngine,
                                 agora::rtc::AGORA_IID_MEDIA_ENGINE);

    if (m_videoTrackId < 0 || !m_mediaEngine) {
      AfxMessageBox(_T("Failed to create custom video track"));
      return false;
    }
  } else {
    AfxMessageBox(_T("Failed to initialize Agora RTC engine"));
    return false;
  }

  return true;
}

void AgoraManager::release() {
  // Ensure video capture is stopped
  stopVideoCapture();

  if (m_rtcEngine) {
    // Release resources
    m_rtcEngine->release(true);
    m_rtcEngine = nullptr;
  }

  m_initialize = false;
  m_remoteRender = false;
  m_videoTrackId = -1;
}

bool AgoraManager::joinChannel(const char* token, const char* channelName) {
  if (!m_rtcEngine || !m_initialize) {
    return false;
  }

  ChannelMediaOptions options;
  // Set channel profile to live broadcasting
  options.channelProfile =
      agora::CHANNEL_PROFILE_TYPE::CHANNEL_PROFILE_LIVE_BROADCASTING;
  // Set user role to broadcaster
  options.clientRoleType = CLIENT_ROLE_BROADCASTER;

  // Publish the audio stream captured by the microphone
  options.publishMicrophoneTrack = false;
  // Publish the camera track
  options.publishCameraTrack = false;

  // Publish the self-captured video stream
  options.publishCustomVideoTrack = true;
  // Set the custom video track ID
  options.customVideoTrackId = m_videoTrackId;

  // Automatically subscribe to all audio streams
  options.autoSubscribeAudio = true;
  // Automatically subscribe to all video streams
  options.autoSubscribeVideo = true;
  // Specify the audio latency level
  options.audienceLatencyLevel = AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;

  // Join the channel using the token and channel name
  int ret = m_rtcEngine->joinChannel(token, channelName, 0, options);

  if (ret == 0) {
    // Start custom video capture
    startVideoCapture();
    return true;
  } else {
    CString error;
    error.Format(_T("Failed to join channel. Error code: %d"), ret);
    AfxMessageBox(error);
    return false;
  }
}

void AgoraManager::leaveChannel() {
  // Stop video capture first
  stopVideoCapture();

  if (m_rtcEngine) {
    // Stop local video preview
    m_rtcEngine->stopPreview();
    // Leave the channel
    m_rtcEngine->leaveChannel();
    // Clear local view
    VideoCanvas canvas;
    canvas.uid = 0;
    m_rtcEngine->setupLocalVideo(canvas);
    m_remoteRender = false;
  }
}

void AgoraManager::setupLocalVideo(HWND localViewHwnd) {
  if (!m_rtcEngine) {
    return;
  }

  // Set local video display properties
  VideoCanvas canvas;
  // Set video to be scaled proportionally
  canvas.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
  // User ID
  canvas.uid = 0;
  // Video display window
  canvas.view = localViewHwnd;
  m_rtcEngine->setupLocalVideo(canvas);
  // Preview the local video
  m_rtcEngine->startPreview();
}

void AgoraManager::setupRemoteVideo(uid_t remoteUid, HWND remoteViewHwnd) {
  if (!m_rtcEngine) {
    return;
  }

  // Set remote video display properties
  VideoCanvas canvas;
  // Set video size to be proportionally scaled
  canvas.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
  // Remote user ID
  canvas.uid = remoteUid;
  // Video display window
  canvas.view = remoteViewHwnd;
  m_rtcEngine->setupRemoteVideo(canvas);
  m_remoteRender = true;
}

void AgoraManager::clearRemoteVideo(uid_t remoteUid) {
  if (!m_rtcEngine) {
    return;
  }

  // Clear remote view
  VideoCanvas canvas;
  canvas.uid = remoteUid;
  m_rtcEngine->setupRemoteVideo(canvas);
  m_remoteRender = false;
}

void AgoraManager::startVideoCapture() {
  // Initialize camera capture
  m_videoCap.open(0, cv::CAP_DSHOW);
  if (!m_videoCap.isOpened()) {
    AfxMessageBox(_T("Failed to open camera"));
    return;
  }

  // Set high resolution for capture
  m_videoCap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
  m_videoCap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
  m_videoCap.set(cv::CAP_PROP_FPS, 60);

  // Get actual resolution (camera might not support requested resolution)
  int actualWidth = (int)m_videoCap.get(cv::CAP_PROP_FRAME_WIDTH);
  int actualHeight = (int)m_videoCap.get(cv::CAP_PROP_FRAME_HEIGHT);

  // Initialize local video writer with timestamp
  CTime currentTime = CTime::GetCurrentTime();
  CString timestamp = currentTime.Format(_T("%Y%m%d_%H%M%S"));
  CString filename;
  filename.Format(_T("local_record_%s.avi"), timestamp);
  std::string filenameStr = CT2A(filename.GetString());

  m_localWriter.open(filenameStr, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                     60.0, cv::Size(actualWidth, actualHeight));

  if (!m_localWriter.isOpened()) {
    AfxMessageBox(_T("Failed to initialize video writer"));
    return;
  }

  // Start capture thread
  m_stopCapture = false;
  m_videoThread = std::thread(&AgoraManager::videoCaptureLoop, this);
}

void AgoraManager::stopVideoCapture() {
  // Signal thread to stop
  m_stopCapture = true;

  // Wait for thread to finish
  if (m_videoThread.joinable()) {
    m_videoThread.join();
  }

  // Clean up resources
  {
    std::lock_guard<std::mutex> lock(m_frameMutex);
    if (m_localWriter.isOpened()) {
      m_localWriter.release();
    }
  }

  if (m_videoCap.isOpened()) {
    m_videoCap.release();
  }
}

void AgoraManager::videoCaptureLoop() {
  cv::Mat highResFrame;
  auto lastFrameTime = std::chrono::steady_clock::now();
  const auto frameInterval = std::chrono::milliseconds(17);  // ~60 FPS

  while (!m_stopCapture && m_videoCap.isOpened()) {
    // Capture frame
    if (!m_videoCap.read(highResFrame) || highResFrame.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      continue;
    }

    // Process frame (save locally + send to Agora)
    processFrame(highResFrame);

    // Frame rate control
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = currentTime - lastFrameTime;
    if (elapsed < frameInterval) {
      std::this_thread::sleep_for(frameInterval - elapsed);
    }
    lastFrameTime = std::chrono::steady_clock::now();
  }
}

void AgoraManager::processFrame(const cv::Mat& highResFrame) {
  try {
    // 1. Save high-resolution frame locally
    {
      std::lock_guard<std::mutex> lock(m_frameMutex);
      if (m_localWriter.isOpened()) {
        m_localWriter.write(highResFrame);
      }
    }

    // 2. Prepare low-resolution frame for Agora
    cv::Mat lowResFrame;

    // Resize to lower resolution for network transmission
    cv::resize(highResFrame, lowResFrame, cv::Size(640, 360), 0, 0,
               cv::INTER_LINEAR);

    // Convert BGR to YUV I420 format required by Agora
    cv::Mat yuvI420;
    cv::cvtColor(lowResFrame, yuvI420, cv::COLOR_BGR2YUV_I420);

    // 3. Create ExternalVideoFrame and push to Agora
    if (m_mediaEngine && m_videoTrackId >= 0) {
      ExternalVideoFrame frame;
      frame.type = ExternalVideoFrame::VIDEO_BUFFER_TYPE::VIDEO_BUFFER_RAW_DATA;
      frame.format = VIDEO_PIXEL_I420;
      frame.buffer = yuvI420.data;
      frame.stride = 640;
      frame.height = 360;
      frame.rotation = 0;
      frame.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now().time_since_epoch())
                            .count();

      int result = m_mediaEngine->pushVideoFrame(&frame, m_videoTrackId);
      if (result != 0) {
        OutputDebugStringA("Failed to push video frame to Agora\n");
      }
    }

  } catch (const cv::Exception& e) {
    CString error;
    error.Format(_T("OpenCV Error: %s"), CString(e.what()));
    OutputDebugString(error);
  }
}
