
// AgoraQuickStartDlg.h : header file
//

#pragma once

#include "AgoraManager.h"

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
  afx_msg LRESULT OnEIDNetworkQuality(WPARAM wParam, LPARAM lParam);

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
  // Agora manager to handle all RTC operations
  AgoraManager m_agoraManager;
};
