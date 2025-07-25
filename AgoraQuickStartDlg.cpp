
// AgoraQuickStartDlg.cpp : implementation file
//

// include pch.h first to ensure precompiled headers are used
#include "pch.h"

//

#include "AgoraQuickStart.h"
#include "AgoraQuickStartDlg.h"
#include "afxdialogex.h"
#include "framework.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx {
 public:
  CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

  // Implementation
 protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CAgoraQuickStartDlg dialog

CAgoraQuickStartDlg::CAgoraQuickStartDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_AGORAQUICKSTART_DIALOG, pParent) {
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CAgoraQuickStartDlg::~CAgoraQuickStartDlg() {
  // Ensure video capture is stopped
  stopVideoCapture();

  if (m_rtcEngine) {
    // https://docs.agora.io/en/broadcast-streaming/get-started/get-started-sdk?platform=windows#release-resources
    m_rtcEngine->release(true);
    m_rtcEngine = NULL;
  }
}

void CAgoraQuickStartDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_CHANNEL, m_edtChannelName);
  DDX_Control(pDX, IDC_STATIC_REMOTE, m_staRemote);
  DDX_Control(pDX, IDC_STATIC_LOCAL, m_staLocal);
}

BEGIN_MESSAGE_MAP(CAgoraQuickStartDlg, CDialog)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()

ON_BN_CLICKED(ID_BTN_JOIN, &CAgoraQuickStartDlg::OnBnClickedBtnJoin)
ON_BN_CLICKED(ID_BTN_LEAVE, &CAgoraQuickStartDlg::OnBnClickedBtnLeave)
ON_MESSAGE(WM_MSGID(EID_JOIN_CHANNEL_SUCCESS),
           CAgoraQuickStartDlg::OnEIDJoinChannelSuccess)
ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraQuickStartDlg::OnEIDUserJoined)
ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraQuickStartDlg::OnEIDUserOffline)
END_MESSAGE_MAP()

// Insert your project's App ID obtained from the Agora Console
#define APP_ID "71aa4763f35149369959d89afe2e504c"

// Insert the temporary token obtained from the Agora Console
#define TOKEN                                                         \
  "007eJxTYAgvqHitPCMgpfRu5XWevS72n1I6j+"                             \
  "bvXXPYqpCrpnVVwEoFBnPDxEQTczPjNGNTQxNLYzNLS1PLFAvLxLRUo1RTA5Nkj6+" \
  "NGQ2BjAzKWw6xMjJAIIjPwlCQWJrDwAAAYKUfyQ=="

// CAgoraQuickStartDlg message handlers

BOOL CAgoraQuickStartDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != nullptr) {
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);   // Set big icon
  SetIcon(m_hIcon, FALSE);  // Set small icon

  // TODO: Add extra initialization here
  // Set the message receiver
  m_eventHandler.SetMsgReceiver(m_hWnd);

  // Initialize Agora engine
  initializeAgoraEngine();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAgoraQuickStartDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAgoraQuickStartDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);  // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()),
                0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user
// drags
//  the minimized window.
HCURSOR CAgoraQuickStartDlg::OnQueryDragIcon() {
  return static_cast<HCURSOR>(m_hIcon);
}

void CAgoraQuickStartDlg::initializeAgoraEngine() {
  // Create IRtcEngine object
  m_rtcEngine = createAgoraRtcEngine();
  // Create IRtcEngine context object
  RtcEngineContext context;
  // Input your App ID. You can obtain your project's App ID from the Agora
  // Console
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
    }
  } else {
    AfxMessageBox(_T("Failed to initialize Agora RTC engine"));
  }
}

void CAgoraQuickStartDlg::joinChannel(const char* token,
                                      const char* channelName) {
  ChannelMediaOptions options;
  // Set channel profile to live broadcasting
  options.channelProfile =
      agora::CHANNEL_PROFILE_TYPE::CHANNEL_PROFILE_LIVE_BROADCASTING;
  // Set user role to broadcaster; keep default value if setting user role to
  // audience
  options.clientRoleType = CLIENT_ROLE_BROADCASTER;

  // Publish the audio stream captured by the microphone
  // options.publishMicrophoneTrack = true;
  // Publish the camera track
  // options.publishCameraTrack = true;

  // Publish the self-captured video stream
  options.publishCustomVideoTrack = true;
  // Disable built-in camera
  options.publishCameraTrack = false;
  // Set the custom video track ID
  options.customVideoTrackId = m_videoTrackId;

  // Automatically subscribe to all audio streams
  options.autoSubscribeAudio = true;
  // Automatically subscribe to all video streams
  options.autoSubscribeVideo = true;
  // Specify the audio latency level
  options.audienceLatencyLevel = AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;
  // Join the channel using the token and channel name (both are const char*)
  int ret = m_rtcEngine->joinChannel(token, channelName, 0, options);

  if (ret == 0) {
    // Start custom video capture
    startVideoCapture();
  } else {
    CString error;
    error.Format(_T("Failed to join channel. Error code: %d"), ret);
    AfxMessageBox(error);
  }
}

void CAgoraQuickStartDlg::startVideoCapture() {
  // Initialize camera capture
  m_videoCap.open(0, cv::CAP_DSHOW);
  if (!m_videoCap.isOpened()) {
    AfxMessageBox(_T("Failed to open camera"));
    return;
  }

  // Set high resolution for capture
  m_videoCap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
  m_videoCap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
  m_videoCap.set(cv::CAP_PROP_FPS, 30);

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
                     30.0, cv::Size(actualWidth, actualHeight));

  if (!m_localWriter.isOpened()) {
    AfxMessageBox(_T("Failed to initialize video writer"));
    return;
  }

  // Start capture thread
  m_stopCapture = false;
  m_videoThread = std::thread(&CAgoraQuickStartDlg::videoCaptureLoop, this);
}

void CAgoraQuickStartDlg::videoCaptureLoop() {
  cv::Mat highResFrame;
  auto lastFrameTime = std::chrono::steady_clock::now();
  const auto frameInterval = std::chrono::milliseconds(33);  // ~30 FPS

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

void CAgoraQuickStartDlg::processFrame(const cv::Mat& highResFrame) {
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
      frame.cropLeft = 0;
      frame.cropTop = 0;
      frame.cropRight = 0;
      frame.cropBottom = 0;
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

void CAgoraQuickStartDlg::stopVideoCapture() {
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

LRESULT CAgoraQuickStartDlg::OnEIDJoinChannelSuccess(WPARAM wParam,
                                                     LPARAM lParam) {
  // Join channel success callback
  uid_t localUid = wParam;
  return 0;
}

LRESULT CAgoraQuickStartDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam) {
  // Remote user joined callback
  uid_t remoteUid = wParam;
  if (m_remoteRender) {
    return 0;
  }
  setupRemoteVideo(remoteUid);
  return 0;
}

LRESULT CAgoraQuickStartDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam) {
  // Remote user left callback
  uid_t remoteUid = wParam;
  if (!m_remoteRender) {
    return 0;
  }
  // Clear remote view
  VideoCanvas canvas;
  canvas.uid = remoteUid;
  m_rtcEngine->setupRemoteVideo(canvas);
  m_remoteRender = false;
  return 0;
}

void CAgoraQuickStartDlg::setupLocalVideo() {
  // Set local video display properties
  VideoCanvas canvas;
  // Set video to be scaled proportionally
  canvas.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
  // User ID
  canvas.uid = 0;
  // Video display window
  canvas.view = m_staLocal.GetSafeHwnd();
  m_rtcEngine->setupLocalVideo(canvas);
  // Preview the local video
  m_rtcEngine->startPreview();
}

void CAgoraQuickStartDlg::setupRemoteVideo(uid_t remoteUid) {
  // Set remote video display properties
  VideoCanvas canvas;
  // Set video size to be proportionally scaled
  canvas.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
  // Remote user ID
  canvas.uid = remoteUid;
  // You can only choose to set either view or surfaceTexture. If both are set,
  // only the settings in view take effect.
  canvas.view = m_staRemote.GetSafeHwnd();
  m_rtcEngine->setupRemoteVideo(canvas);
  m_remoteRender = true;
}

void CAgoraQuickStartDlg::leaveChannel() {
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

void CAgoraQuickStartDlg::OnBnClickedBtnJoin() {
  CString strChannelName;
  m_edtChannelName.GetWindowText(strChannelName);
  if (strChannelName.IsEmpty()) {
    AfxMessageBox(_T("Fill channel name first"));
    return;
  }
  joinChannel(TOKEN, CW2A(strChannelName));
  setupLocalVideo();
}
void CAgoraQuickStartDlg::OnBnClickedBtnLeave() { leaveChannel(); }
