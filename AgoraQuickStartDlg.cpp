
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

void CAgoraQuickStartDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAgoraQuickStartDlg, CDialog)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

// Insert your project's App ID obtained from the Agora Console
#define APP_ID "71aa4763f35149369959d89afe2e504c"

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
  //   context.eventHandler = &m_eventHandler;
  // Initialize
  int ret = m_rtcEngine->initialize(context);
  m_initialize = (ret == 0);

  if (m_initialize) {
    // Enable the video module
    m_rtcEngine->enableVideo();
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
  options.publishMicrophoneTrack = true;
  // Publish the camera track
  options.publishCameraTrack = true;
  // Automatically subscribe to all audio streams
  options.autoSubscribeAudio = true;
  // Automatically subscribe to all video streams
  options.autoSubscribeVideo = true;
  // Specify the audio latency level
  options.audienceLatencyLevel = AUDIENCE_LATENCY_LEVEL_LOW_LATENCY;
  // Join the channel using the token and channel name (both are const char*)
  m_rtcEngine->joinChannel(token, channelName, 0, options);
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
  // Render remote view
  VideoCanvas canvas;
  canvas.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
  canvas.uid = remoteUid;
  canvas.view = m_staRemote.GetSafeHwnd();
  m_rtcEngine->setupRemoteVideo(canvas);
  m_remoteRender = true;
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
