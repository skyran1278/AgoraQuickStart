# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A Windows C++ MFC application demonstrating Agora RTC SDK integration with OpenCV video capture, featuring network-adaptive video quality adjustment.

## Build

This project uses Visual Studio 2022 (v143 toolset). Build via MSBuild or the Visual Studio IDE:

```bash
msbuild AgoraQuickStart.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild AgoraQuickStart.vcxproj /p:Configuration=Debug /p:Platform=x64
```

**External dependencies (must be installed separately):**
- Agora RTC SDK: placed under `sdk/` in the solution root (`sdk/high_level_api/include`, `sdk/x86_64/`)
- OpenCV 4.1.2x: expected at `C:\opencv\build\` (hardcoded paths in `.vcxproj`)

There are no unit tests.

## Architecture

**Manager Pattern + Event-Driven UI**

| Class | File | Purpose |
|-------|------|---------|
| `AgoraManager` | AgoraManager.h/cpp | RTC engine wrapper: init, join/leave, video capture thread, quality adaptation |
| `AgoraEventHandler` | AgoraEventHandler.h/cpp | Agora SDK callbacks → posts Windows messages to the dialog |
| `CAgoraQuickStartDlg` | AgoraQuickStartDlg.h/cpp | MFC dialog: UI controls, message handlers, CSV stats logging |

### Video Pipeline

1. `AgoraManager::startVideoCapture()` opens the webcam via `cv::VideoCapture` at 1920×1080@60fps and spawns `videoCaptureLoop()` in a `std::thread`.
2. Each frame is saved to a local `.avi` file via `cv::VideoWriter`, then resized and converted BGR→YUV I420 before being pushed to Agora via `mediaEngine::pushVideoFrame()`.
3. Resolution and FPS are stored in `m_currentWidth`, `m_currentHeight`, `m_pushFPS` and updated dynamically by `adjustVideoQualityBasedOnNetwork()`.

### Network Quality Adaptation

`adjustVideoQualityBasedOnNetwork()` maps incoming quality tier (1=good, 2=medium, 3=poor) to resolution presets:
- Tier 1: 1280×720 @ 15 fps
- Tier 2: 640×360 @ 7 fps
- Tier 3: 64×64 @ 1 fps

Transitions require 5 consecutive consistent readings (stability counter) before applying. Triggered from `OnEIDNetworkQuality()` or `OnEIDRtcStats()` in the dialog.

### Event Flow

```
Agora SDK callback (background thread)
  → AgoraEventHandler::onXxx()
  → PostMessage(m_hMsgHanlder, WM_MSGID(EID_xxx), ...)
  → CAgoraQuickStartDlg::OnEIDxxx()
```

Message IDs are defined via `WM_MSGID(EID_xxx)` macros in `AgoraEventHandler.h`.

### Statistics Logging

`OnEIDLocalVideoStats()` appends rows to a CSV file (`video_stats_YYYYMMDD_HHMMSS.csv`) with columns: Timestamp, SentBitrate_kbps, SentFrameRate_fps, EncodedWidth, EncodedHeight, NetworkQuality.

## Key Implementation Notes

- Thread synchronization: `std::mutex m_captureMutex` guards frame access between the capture thread and the push logic.
- The dialog stores a `CAgoraQuickStartDlg*` pointer in `AgoraEventHandler` to get the `HWND` message target.
- `m_pushFPS` controls frame rate throttling inside the capture loop via `std::chrono` timing.
- Local video recording filename: `local_record_YYYYMMDD_HHMMSS.avi` in the working directory.
