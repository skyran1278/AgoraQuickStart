#pragma once

// #include <atomic>
// #include <chrono>
// #include <mutex>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
// #include <thread>

#include "AgoraEventHandler.h"
#include "IAgoraRtcEngine.h"

using namespace agora::rtc;
using namespace agora::media::base;

class AgoraManager {
 public:
  AgoraManager();
  ~AgoraManager();

  // Core initialization and cleanup
  bool initialize(HWND hWnd);
  void release();

  // Channel operations
  bool joinChannel(const char* token, const char* channelName);
  void leaveChannel();

  // Video setup
  void setupLocalVideo(HWND localViewHwnd);
  void setupRemoteVideo(uid_t remoteUid, HWND remoteViewHwnd);
  void clearRemoteVideo(uid_t remoteUid);

  // Video capture control
  void startVideoCapture();
  void stopVideoCapture();

  // FPS configuration
  void setPushFPS(int fps);
  int getPushFPS() const { return m_pushFPS; }

  // Set push mode: true for time-based, false for frame-skipping
  void setTimeBasedPush(bool timeBased) { m_timeBasedPush = timeBased; }

  // Video quality management based on network conditions
  void adjustVideoQualityBasedOnNetwork(int networkQuality);
  void setVideoResolution(int width, int height);
  void updateVideoEncoderConfiguration();
  void getCurrentVideoSettings(int& width, int& height, int& fps) const;

  // Advanced bitrate management
  enum BitrateStrategy {
    STRATEGY_FIXED_PRESETS,  // Use fixed bitrates per quality level
    STRATEGY_DYNAMIC_CALC,   // Calculate based on resolution + fps
    STRATEGY_CONSERVATIVE    // More conservative approach for poor networks
  };
  void setBitrateStrategy(BitrateStrategy strategy) {
    m_bitrateStrategy = strategy;
  }

  // Getters for state
  bool isInitialized() const { return m_initialize; }
  bool isRemoteRenderActive() const { return m_remoteRender; }

 private:
  // Agora engine components
  IRtcEngine* m_rtcEngine;
  agora::util::AutoPtr<agora::media::IMediaEngine> m_mediaEngine;
  AgoraEventHandler m_eventHandler;

  // State variables
  bool m_initialize;
  bool m_remoteRender;
  int m_videoTrackId;
  int m_pushFPS;         // Configurable push FPS
  bool m_timeBasedPush;  // true for time-based, false for frame-skipping

  // Video quality settings
  int m_videoWidth;
  int m_videoHeight;
  int m_currentNetworkQuality;
  BitrateStrategy m_bitrateStrategy;  //
                                      // Video capture components
  cv::VideoCapture m_videoCap;
  cv::VideoWriter m_localWriter;
  std::thread m_videoThread;
  std::atomic<bool> m_stopCapture;
  std::mutex m_frameMutex;
  int m_frameCounter;  // For frame skipping  // Private methods
  void videoCaptureLoop();
  void processFrame(const cv::Mat& highResFrame);

  // Bitrate calculation strategies
  int calculateFixedBitrate();
  int calculateDynamicBitrate();
  int calculateConservativeBitrate();
};
