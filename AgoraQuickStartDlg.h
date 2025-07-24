
// AgoraQuickStartDlg.h : header file
//

#pragma once

#include "CAgoraQuickStartRtcEngineEventHandler.h"
#include "IAgoraRtcEngine.h"

using namespace agora::rtc;
using namespace agora::media::base;

// CAgoraQuickStartDlg dialog
class CAgoraQuickStartDlg : public CDialog {
  // Construction
 public:
  CAgoraQuickStartDlg(CWnd *pParent = nullptr);  // standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_AGORAQUICKSTART_DIALOG };
#endif

 protected:
  virtual void DoDataExchange(CDataExchange *pDX);  // DDX/DDV support

  // Implementation
 protected:
  HICON m_hIcon;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()

 private:
  // Declare the required variables
  bool m_initialize = false;
  IRtcEngine *m_rtcEngine = nullptr;  // RTC engine instance

  bool m_remoteRender = false;

  CEdit m_staRemote;
  CEdit m_staLocal;

  /**
   * @brief
   * https://docs.agora.io/en/broadcast-streaming/get-started/get-started-sdk?platform=windows#initialize-the-engine
   *
   */
  void initializeAgoraEngine();

  /**
   * @brief
   * https://docs.agora.io/en/broadcast-streaming/get-started/get-started-sdk?platform=windows#join-a-channel
   *
   * @param token
   * @param channelName
   */
  void joinChannel(const char *token, const char *channelName);

  // Handle callback events such as user joining/user leaving
  LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
  LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
  LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);

  /**
   * @brief
   * https://docs.agora.io/en/broadcast-streaming/get-started/get-started-sdk?platform=windows#display-the-local-video
   *
   */
  void setupLocalVideo();

  /**
   * @brief
   * https://docs.agora.io/en/broadcast-streaming/get-started/get-started-sdk?platform=windows#display-remote-video
   *
   * @param remoteUid
   */
  void setupRemoteVideo(uid_t remoteUid);
};
