#include "pch.h"

//
#include "AgoraEventHandler.h"

AgoraEventHandler::AgoraEventHandler() : m_hMsgHandler(nullptr) {}

void AgoraEventHandler::SetMsgReceiver(HWND hWnd) { m_hMsgHandler = hWnd; }

void AgoraEventHandler::onJoinChannelSuccess(const char *channel, uid_t uid,
                                             int elapsed) {
  if (m_hMsgHandler) {
    ::PostMessage(m_hMsgHandler, WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), uid, 0);
  }
}

void AgoraEventHandler::onUserJoined(uid_t uid, int elapsed) {
  if (m_hMsgHandler) {
    ::PostMessage(m_hMsgHandler, WM_MSGID(EID_USER_JOINED), uid, 0);
  }
}

void AgoraEventHandler::onUserOffline(uid_t uid,
                                      USER_OFFLINE_REASON_TYPE reason) {
  if (m_hMsgHandler) {
    ::PostMessage(m_hMsgHandler, WM_MSGID(EID_USER_OFFLINE), uid, 0);
  }
}

void AgoraEventHandler::onNetworkQuality(uid_t uid, int txQuality,
                                         int rxQuality) {
  if (m_hMsgHandler) {
    // Pack both tx and rx quality into lParam (tx in high word, rx in low word)
    LPARAM qualityInfo = MAKELPARAM(rxQuality, txQuality);
    ::PostMessage(m_hMsgHandler, WM_MSGID(EID_NETWORK_QUALITY), uid,
                  qualityInfo);
  }
}
