
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

  virtual ~CAgoraQuickStartDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_AGORAQUICKSTART_DIALOG };
#endif

  // Handle the join/leave button click event
  afx_msg void OnBnClickedBtnJoin();
  afx_msg void OnBnClickedBtnLeave();

  // Handle callback events such as user joining/user leaving
  afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);

 protected:
  virtual void DoDataExchange(CDataExchange *pDX);  // DDX/DDV support

  // Implementation
 protected:
  HICON m_hIcon;

  CEdit m_edtChannelName;
  CEdit m_staRemote;
  CEdit m_staLocal;

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

  /**
   * @brief
   * https://docs.agora.io/en/broadcast-streaming/get-started/get-started-sdk?platform=windows#leave-the-channel
   *
   */
  void leaveChannel();
};
