//
// Wotja_API.cpp
// WotjaAPI_WinRT_demo
//
// Copyright (C) 2007-2024 Intermorphic Limited. All rights reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

#include "pch.h"

#include "WotjaAPI.h"

#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <iostream>

#include <appmodel.h>

using namespace winrt;
using namespace Windows::Foundation;

static bool mbPlaying = false;

static void sOpenURL(const std::wstring& urlString) {
  winrt::Windows::Foundation::Uri uri(urlString);
  winrt::Windows::System::Launcher::LaunchUriAsync(uri);
}

static void sOpenURLInBackground(const std::wstring& urlString) {
  // There is no specific way to do this on Windows.
  // What happens, depends on the target URL handler.
  // In general, Wotja launches in the background.
  sOpenURL(urlString);
}

static void sOpenURLInForeground(const std::wstring& urlString) {
  // There is no specific way to do this on Windows.
  // What happens, depends on the target URL handler.
  // In general, Edge etc. launch in the background.
  sOpenURL(urlString);
}

static std::wstring sTemporaryFilePath = L"";
void WotjaAPI::setFilePathForURLSend(const std::wstring& temporaryFilePath) {
  sTemporaryFilePath = temporaryFilePath;
}

#include <algorithm>

void replaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

std::wstring convertNarrowToWide(const std::string& sString) {
  int sLength = (int)sString.length() + 1;
  int wLength = MultiByteToWideChar(CP_ACP, 0, sString.c_str(), sLength, 0, 0);
  std::wstring wString(wLength, L'\0');
  MultiByteToWideChar(CP_ACP, 0, sString.c_str(), sLength, &wString[0], wLength);
  return wString;
}

std::string convertWideToNarrow(const std::wstring& wString) {
  auto numBytes = WideCharToMultiByte(CP_UTF8, 0, wString.c_str(), -1, nullptr, 0, nullptr, nullptr);

  std::string nString(numBytes + 1, '\0');
  WideCharToMultiByte(CP_UTF8, 0, wString.c_str(), -1, &nString[0], numBytes, nullptr, nullptr);
  return nString;
}

std::string encodeStringForWotjaURI(const std::string& sString) {
  // The only valid characters for a Wotja URI are those in the following character set:
  auto cWotjaURI_ValidCharsSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
  std::string validCharSet(cWotjaURI_ValidCharsSet);

  std::stringstream encodedPathStream;
  for (auto theChar : sString) {
    if (validCharSet.find(theChar) != std::string::npos) {
      encodedPathStream << theChar;
    } else if (theChar == '\0') {
    } else {
      encodedPathStream << "%";
      encodedPathStream << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)theChar;
    }
  }
  auto result = encodedPathStream.str();
  return result;
}

static void loadAndPlayFromFile(const std::wstring& wotjaURIString) {
  std::string pathStr  = convertWideToNarrow(sTemporaryFilePath);
  std::string wotjaStr = convertWideToNarrow(wotjaURIString);

  // Replace invalid path characters, with encoded characters.
  std::string sPathToWrite = encodeStringForWotjaURI(pathStr);
  auto wPathToWrite = convertNarrowToWide(sPathToWrite);

  std::wstringstream stringstream;
  stringstream << L"wotja://api_load/?path=" << wPathToWrite;
  std::wstring urlToSend = stringstream.str();

  std::ofstream outStream(sTemporaryFilePath, std::ios::binary | std::ios::trunc);
  outStream << wotjaStr;
  outStream.close();

  mbPlaying = true;

  sOpenURLInBackground(urlToSend);
}

void WotjaAPI::loadAndPlay(const std::wstring& wotjaURIString) {

  // Load Wotja content, and start it playing.
  if (sTemporaryFilePath != L"") {
    // Use the path we've been provided
    loadAndPlayFromFile(wotjaURIString);
    return;
  }

  std::wstring useString = wotjaURIString;
  if (useString.starts_with(L"wotja://api_load/?data=") == false) {
    // Change wotja://blahblah to wotja://api_load/?data=blahblah
    useString.replace(0, 8, L"wotja://api_load/?data=");
  }

  mbPlaying = true;

  sOpenURLInBackground(useString);
}

void WotjaAPI::stop() {
  mbPlaying = false;
  sOpenURLInBackground(L"wotja://api_stop/");
}

void WotjaAPI::playWithTrackIndex(int trackIndex) {
  mbPlaying = true;

  std::wstringstream stringstream;
  stringstream << L"wotja://api_play/?ti=" << trackIndex;
  sOpenURLInBackground(stringstream.str());
}

bool WotjaAPI::getIsPlaying() {
  return mbPlaying;
}

// MARK: Wotja availability checking

// Wotja API support added in the following Wotja version
static std::wstring cMinimumWotjaVersion = L"24.0.90";

static WotjaAPI::WotjaStatus sWotjaStatus        = WotjaAPI::WotjaStatus_looking_for_wotja;
static std::wstring sWotjaVersion                = L""; // Set internally to e.g. "24.1.0"
static std::wstring sWotjaMinimumVersionRequired = L""; // Set by app to e.g. "24.1.0"

// Returns e.g. "Looking for Wotja..."
std::wstring WotjaAPI::convertWotjaStatusToString(WotjaAPI::WotjaStatus wotjaStatus) {
  switch (wotjaStatus) {
  case WotjaAPI::WotjaStatus_not_found:
    return L"Wotja not found";
  case WotjaAPI::WotjaStatus_needs_update:
    return std::wstring(L"Wotja update required (found ") + sWotjaVersion + L"), needs " + sWotjaMinimumVersionRequired;
  case WotjaAPI::WotjaStatus_found:
    return std::wstring(L"Wotja found (version ") + sWotjaVersion + L")";
  case WotjaAPI::WotjaStatus_looking_for_wotja:
  default:
    return L"Looking for Wotja...";
  }
}

// Returns e.g. @"Looking for Wotja..."
std::wstring WotjaAPI::getWotjaStatusString() {
  return convertWotjaStatusToString(getWotjaStatus());
}

// Returns e.g. @"24.1.0"
std::wstring WotjaAPI::getFirstSupportedWotjaVersion() {
  return cMinimumWotjaVersion;
}

WotjaAPI::WotjaStatus WotjaAPI::getWotjaStatus() {
  return sWotjaStatus;
}

uint32_t versionNumberFromString(const std::wstring& value) {
  std::vector<std::wstring> output;

  std::wstring::size_type prev_pos = 0, pos = 0;

  while ((pos = value.find(L".", pos)) != std::string::npos) {
    std::wstring substring(value.substr(prev_pos, pos - prev_pos));
    output.push_back(substring);
    prev_pos = ++pos;
  }

  output.push_back(value.substr(prev_pos, pos - prev_pos)); // Last word

  uint32_t theNumber = 0;

  for (auto& component : output) {
    uint32_t testChar = (uint32_t)std::stoi(component);
    theNumber         = (theNumber << 8) + testChar;
  }

  return theNumber;
}

void WotjaAPI::searchForWotja(const std::wstring& withMinimumVersion, std::function<void(WotjaStatus)> completion) {
  // This works, from the Power Shell!
  // Get-AppxPackage -Name "Intermorphic.Wotja22Lite"

  sWotjaMinimumVersionRequired = withMinimumVersion;

  std::thread  t1([completion] {
    auto   packageFamilyName = L"Intermorphic.Wotja22Lite_2dswjjh8nep0p";
    UINT32 count = 0, bufferLength = 0;

    GetPackagesByPackageFamily(packageFamilyName, &count, 0, &bufferLength, 0);
    if (count > 0) {
      PWSTR* packageFullNames = (PWSTR*)alloca(count * sizeof(PWSTR) + bufferLength * sizeof(WCHAR));
      PWSTR  buffer           = (PWSTR)(packageFullNames + count);

      if (NOERROR == GetPackagesByPackageFamily(packageFamilyName, &count, packageFullNames, &bufferLength, buffer)) {
        if (count > 0) {
          PCWSTR packageFullName = *packageFullNames++;
          OutputDebugStringW(L"packageFullName=");
          OutputDebugStringW(packageFullName);
          OutputDebugStringW(L"\n");

          std::wstring stripPrefix(L"Intermorphic.Wotja22Lite_");
          std::wstring versionString = packageFullName;
          if (versionString.starts_with(stripPrefix)) {
            versionString.replace(0, stripPrefix.length(), L"");
          }

          std::wstring stripSuffix(L"_x64__2dswjjh8nep0p");
          if (versionString.ends_with(stripSuffix)) {
            versionString.erase(versionString.length() - stripSuffix.length());
          }

          std::wstring stripSuffix2(L".0");
          if (versionString.ends_with(stripSuffix2)) {
            versionString.erase(versionString.length() - stripSuffix2.length());
          }

          sWotjaVersion = versionString;

          auto versionNumber = versionNumberFromString(sWotjaVersion);

          uint32_t minimumRequiredVersionNumber = versionNumberFromString(sWotjaMinimumVersionRequired);
          if (versionNumber < minimumRequiredVersionNumber) {
            sWotjaStatus = WotjaStatus_needs_update;
            completion(WotjaStatus_needs_update);
          } else {
            sWotjaStatus = WotjaStatus_found;
            completion(WotjaStatus_found);
          }

          return;
        }
      }

      sWotjaStatus = WotjaStatus_not_found;
      completion(sWotjaStatus);
    }
  });
  t1.join();
}

// MARK: Wotja installation, update and miscellaneous

void WotjaAPI::installWotja() {
  sOpenURLInForeground(L"https://apps.microsoft.com/store/detail/9NH2TJ3JGNVP");
}

void WotjaAPI::updateWotja() {
  sOpenURLInForeground(L"https://apps.microsoft.com/store/detail/9NH2TJ3JGNVP");
}

void WotjaAPI::showWotjaEULA() {
  sOpenURLInForeground(L"https://wotja.com/legal/eula");
}

void WotjaAPI::showDeveloperFAQ() {
  sOpenURLInForeground(L"https://wotja.com/help/#developer");
}

void WotjaAPI::showAboutWotja() {
  sOpenURLInForeground(L"https://wotja.com/music/");
}

void WotjaAPI::showWotja() {
  sOpenURLInForeground(L"wotja://api_show/");
}



