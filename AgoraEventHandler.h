#pragma once

#include "IAgoraRtcEngine.h"

using namespace agora::rtc;

// Define the message ID
#define WM_MSGID(code) (WM_USER + 0x200 + code)
#define EID_JOIN_CHANNEL_SUCCESS 0x00000002
#define EID_USER_JOINED 0x00000004
#define EID_USER_OFFLINE 0x00000004

/**
 * @brief
 * https://docs.agora.io/en/broadcast-streaming/get-started/get-started-sdk?platform=windows#subscribe-to--events
 *
 */
// Define the AgoraEventHandler class to handle callback
// events such as users joining and leaving the channel
class AgoraEventHandler : public IRtcEngineEventHandler {
 public:
  AgoraEventHandler();

  // Set the handle of the message receiving window
  void SetMsgReceiver(HWND hWnd);

  // Register onJoinChannelSuccess callback
  // This callback is triggered when a local user successfully joins a channel
  virtual void onJoinChannelSuccess(const char *channel, uid_t uid,
                                    int elapsed) override;

  // Register onUserJoined callback
  // This callback is triggered when the remote host successfully joins the
  // channel
  virtual void onUserJoined(uid_t uid, int elapsed) override;

  // Register onUserOffline callback
  // This callback is triggered when the remote host leaves the channel or is
  // offline
  virtual void onUserOffline(uid_t uid,
                             USER_OFFLINE_REASON_TYPE reason) override;

 private:
  HWND m_hMsgHandler;
};
