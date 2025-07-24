
// AgoraQuickStartDlg.h : header file
//

#pragma once

#include "IAgoraRtcEngine.h"

using namespace agora::rtc;

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

  void initializeAgoraEngine();
  void joinChannel(const char *token, const char *channelName);
};
