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

void AgoraEventHandler::onRtcStats(const agora::rtc::RtcStats &stats) {
  if (m_hMsgHandler) {
    // Log comprehensive RTC statistics
    CString statsMsg;
    statsMsg.Format(
        _T("RTC Stats - Duration: %ds, TX Bytes: %d, RX Bytes: %d, ")
        _T("TX Bitrate: %d kbps, RX Bitrate: %d kbps, ")
        _T("TX Packet Loss: %.2f%%, RX Packet Loss: %.2f%%, ")
        _T("RTT: %dms, Users: %d, CPU App: %.1f%%, CPU Total: %.1f%%\n"),
        stats.duration, stats.txBytes, stats.rxBytes, stats.txKBitRate,
        stats.rxKBitRate, stats.txPacketLossRate, stats.rxPacketLossRate,
        stats.lastmileDelay, stats.userCount, stats.cpuAppUsage,
        stats.cpuTotalUsage);

    OutputDebugString(statsMsg);

    // Send stats via Windows message for UI updates
    // Pack key stats into message parameters
    WPARAM wParam = MAKEWPARAM(stats.txKBitRate, stats.rxKBitRate);
    LPARAM lParam =
        MAKELPARAM(stats.lastmileDelay, (WORD)(stats.txPacketLossRate * 100));
    ::PostMessage(m_hMsgHandler, WM_MSGID(EID_RTC_STATS), wParam, lParam);
  }
}

void AgoraEventHandler::onLocalVideoStats(VIDEO_SOURCE_TYPE source,
                                          const LocalVideoStats &stats) {
  if (m_hMsgHandler) {
    // Log detailed local video statistics
    CString videoStatsMsg;
    videoStatsMsg.Format(
        _T("Local Video Stats - Source: %d, Sent Bitrate: %d kbps, ")
        _T("Sent FPS: %d, Encoder Output FPS: %d, ")
        _T("Rendered FPS: %d, Target Bitrate: %d kbps, ")
        _T("Target FPS: %d, Quality Adapt: %d, ")
        _T("Encoded Width: %d, Encoded Height: %d, ")
        _T("Sent Frames: %d, Codec: %d, ")
        _T("TX Packet Loss: %.2f%%, Capture FPS: %d\n"),
        static_cast<int>(source), stats.sentBitrate, stats.sentFrameRate,
        stats.encoderOutputFrameRate, stats.rendererOutputFrameRate,
        stats.targetBitrate, stats.targetFrameRate,
        static_cast<int>(stats.qualityAdaptIndication), stats.encodedFrameWidth,
        stats.encodedFrameHeight, stats.sentFrameRate,
        static_cast<int>(stats.codecType), stats.txPacketLossRate,
        stats.captureFrameRate);

    OutputDebugString(videoStatsMsg);

    // Send video stats via Windows message for UI updates
    // Pack key video stats into message parameters
    WPARAM wParam = MAKEWPARAM(stats.sentBitrate, stats.sentFrameRate);
    LPARAM lParam =
        MAKELPARAM(stats.encodedFrameWidth, stats.encodedFrameHeight);
    ::PostMessage(m_hMsgHandler, WM_MSGID(EID_LOCAL_VIDEO_STATS), wParam,
                  lParam);
  }
}
