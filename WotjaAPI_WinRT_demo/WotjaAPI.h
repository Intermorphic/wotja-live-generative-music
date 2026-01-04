//
// WotjaAPI.h
// WotjaAPI_WinRT_demo
//
// Copyright © 1990-2026 Intermorphic Limited. All Rights Reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

#pragma once

#include <string>
#include <functional>

class WotjaAPI {
public:
  // According to Microsoft, the maximum size of wotjaURIString must not exceed 100KB; this is because
  // according to https://learn.microsoft.com/en-us/uwp/api/windows.system.launcher.launchuriasync?view=winrt-22621
  // "Important ... The amount of data that can be transferred must not exceed 100 KB."
  // However, from investigation, the actual limit seems to be only 32768 bytes.
  // This is mentioned here:
  // https://github.com/MicrosoftDocs/winrt-api/issues/990
  static void setFilePathForURLSend(const std::wstring& temporaryFilePath);
  static void loadAndPlay(const std::wstring& wotjaURIString);

  static void stop();
  static void playWithTrackIndex(int trackIndex);
  static bool getIsPlaying();

  // MARK: Wotja Availability checking

  enum WotjaStatus {
    WotjaStatus_looking_for_wotja = 1,
    WotjaStatus_not_found         = 2,
    WotjaStatus_needs_update      = 3,
    WotjaStatus_found             = 4,
  };

  // Returns e.g. "Looking for Wotja..."
  static std::wstring convertWotjaStatusToString(WotjaAPI::WotjaStatus wotjaStatus);

  // Returns e.g. "Looking for Wotja..."
  static std::wstring getWotjaStatusString();

  // Returns e.g. "24.1.0"
  static std::wstring getFirstSupportedWotjaVersion();

  static WotjaAPI::WotjaStatus getWotjaStatus();

  static void searchForWotja(const std::wstring& withMinimumVersion, std::function<void(WotjaStatus)> completion);

  // MARK: Wotja installation, update and miscellaneous
  
  static void installWotja();
  static void updateWotja();
  static void showWotjaEULA();
  static void showWotja();
  static void showDeveloperFAQ();
  static void showAboutWotja();
};
