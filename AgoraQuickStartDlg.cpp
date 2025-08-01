// AgoraQuickStartDlg.cpp : implementation file
//

// include pch.h first to ensure precompiled headers are used
#include "pch.h"

//
#include "AgoraEventHandler.h"  // For message ID definitions
#include "AgoraQuickStart.h"
#include "AgoraQuickStartDlg.h"
#include "afxdialogex.h"
#include "framework.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TOKEN                                                                  \
  "007eJxTYKjh0H/"                                                             \
  "yoPzxOlbFrxwHfLkWc5xobeqMaZpuJP39zdHSLhYFBnPDxEQTczPjNGNTQxNLYzNLS1PLFAvLx" \
  "LRUo1RTA5Pkj1I9GQ2BjAwpn1WZGBkgEMRnYShILM1hYAAACpEejA=="

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
  // AgoraManager handles cleanup automatically
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
ON_MESSAGE(WM_MSGID(EID_NETWORK_QUALITY),
           &CAgoraQuickStartDlg::OnEIDNetworkQuality)
ON_MESSAGE(WM_MSGID(EID_RTC_STATS), &CAgoraQuickStartDlg::OnEIDRtcStats)
ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STATS),
           &CAgoraQuickStartDlg::OnEIDLocalVideoStats)
END_MESSAGE_MAP()

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
  // Initialize Agora engine through AgoraManager
  if (!m_agoraManager.initialize(m_hWnd)) {
    AfxMessageBox(_T("Failed to initialize Agora Manager"));
  }

  // Initialize CSV file for statistics logging
  InitializeCSVFile();

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
  // The system calls this function to obtain the cursor to display while the
  // user drags
  // the minimized window.
  return static_cast<HCURSOR>(m_hIcon);
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
  if (m_agoraManager.isRemoteRenderActive()) {
    return 0;
  }
  m_agoraManager.setupRemoteVideo(remoteUid, m_staRemote.GetSafeHwnd());
  return 0;
}

LRESULT CAgoraQuickStartDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam) {
  // Remote user left callback
  uid_t remoteUid = wParam;
  if (!m_agoraManager.isRemoteRenderActive()) {
    return 0;
  }
  m_agoraManager.clearRemoteVideo(remoteUid);
  return 0;
}

LRESULT CAgoraQuickStartDlg::OnEIDNetworkQuality(WPARAM wParam, LPARAM lParam) {
  // Network quality callback
  uid_t uid = wParam;
  int rxQuality = LOWORD(lParam);  // Receive quality
  int txQuality = HIWORD(lParam);  // Transmit quality

  // Only adjust based on local user's quality (uid == 0) or use worst quality
  if (uid == 0) {
    // Use the worse of rx and tx quality for adjustment
    int worstQuality = std::max(rxQuality, txQuality);
    m_agoraManager.adjustVideoQualityBasedOnNetwork(worstQuality);

    // Optional: Display quality info in debug output
    CString debugMsg;
    debugMsg.Format(
        _T("Network Quality - UID: %u, TX: %d, RX: %d, Adjusted to: %d\n"), uid,
        txQuality, rxQuality, worstQuality);
    OutputDebugString(debugMsg);
  }

  return 0;
}

LRESULT CAgoraQuickStartDlg::OnEIDRtcStats(WPARAM wParam, LPARAM lParam) {
  // Extract RTC statistics from message parameters
  int txBitrate = LOWORD(wParam);     // TX bitrate in kbps
  int rxBitrate = HIWORD(wParam);     // RX bitrate in kbps
  int rtt = LOWORD(lParam);           // Round trip time in ms
  int txPacketLoss = HIWORD(lParam);  // TX packet loss rate (scaled by 100)

  // Update UI or store statistics as needed
  CString statsText;
  statsText.Format(_T("Stats: TX %d kbps, RX %d kbps, RTT %dms, Loss %.1f%%"),
                   txBitrate, rxBitrate, rtt, txPacketLoss / 100.0f);

  // You can update a status bar or label here
  // For now, just output to debug console
  OutputDebugString(statsText + _T("\n"));

  return 0;
}

LRESULT CAgoraQuickStartDlg::OnEIDLocalVideoStats(WPARAM wParam,
                                                  LPARAM lParam) {
  // Extract local video statistics from message parameters
  int sentBitrate = LOWORD(wParam);    // Sent bitrate in kbps
  int sentFrameRate = HIWORD(wParam);  // Sent frame rate
  int encodedWidth = LOWORD(lParam);   // Encoded frame width
  int encodedHeight = HIWORD(lParam);  // Encoded frame height

  // Update UI or store video statistics as needed
  CString videoStatsText;
  videoStatsText.Format(_T("Video: %dx%d@%dfps, %d kbps"), encodedWidth,
                        encodedHeight, sentFrameRate, sentBitrate);

  // You can update a video stats display here
  // For now, just output to debug console
  OutputDebugString(videoStatsText + _T("\n"));

  // Write statistics to CSV file
  WriteStatsToCSV(sentBitrate, sentFrameRate, encodedWidth, encodedHeight);

  return 0;
}

void CAgoraQuickStartDlg::OnBnClickedBtnJoin() {
  CString strChannelName;
  m_edtChannelName.GetWindowText(strChannelName);
  if (strChannelName.IsEmpty()) {
    AfxMessageBox(_T("Fill channel name first"));
    return;
  }

  if (m_agoraManager.joinChannel(TOKEN, CW2A(strChannelName))) {
    m_agoraManager.setupLocalVideo(m_staLocal.GetSafeHwnd());
  }
}

void CAgoraQuickStartDlg::OnBnClickedBtnLeave() {
  m_agoraManager.leaveChannel();
}

void CAgoraQuickStartDlg::InitializeCSVFile() {
  // Create CSV file with timestamp in filename
  CTime currentTime = CTime::GetCurrentTime();
  CString timestamp = currentTime.Format(_T("%Y%m%d_%H%M%S"));
  m_csvFilePath.Format(_T("video_stats_%s.csv"), timestamp);

  // Write CSV header
  CStdioFile csvFile;
  if (csvFile.Open(m_csvFilePath,
                   CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
    csvFile.WriteString(
        _T("Timestamp,SentBitrate_kbps,SentFrameRate_fps,EncodedWidth,")
        _T("EncodedHeight\n"));
    csvFile.Close();

    CString logMsg;
    logMsg.Format(_T("CSV statistics file created: %s\n"), m_csvFilePath);
    OutputDebugString(logMsg);
  } else {
    OutputDebugString(_T("Failed to create CSV statistics file\n"));
  }
}

void CAgoraQuickStartDlg::WriteStatsToCSV(int sentBitrate, int sentFrameRate,
                                          int encodedWidth, int encodedHeight) {
  // Get current timestamp
  CTime currentTime = CTime::GetCurrentTime();
  CString timestamp = currentTime.Format(_T("%Y-%m-%d %H:%M:%S"));

  // Format CSV line
  CString csvLine;
  csvLine.Format(_T("%s,%d,%d,%d,%d\n"), timestamp, sentBitrate, sentFrameRate,
                 encodedWidth, encodedHeight);

  // Append to CSV file
  CStdioFile csvFile;
  if (csvFile.Open(m_csvFilePath, CFile::modeWrite | CFile::typeText)) {
    csvFile.SeekToEnd();
    csvFile.WriteString(csvLine);
    csvFile.Close();
  } else {
    // If file opening fails, try to recreate it
    OutputDebugString(
        _T("Failed to open CSV file for writing, attempting to recreate\n"));
    InitializeCSVFile();

    // Try again
    if (csvFile.Open(m_csvFilePath, CFile::modeWrite | CFile::typeText)) {
      csvFile.SeekToEnd();
      csvFile.WriteString(csvLine);
      csvFile.Close();
    }
  }
}
