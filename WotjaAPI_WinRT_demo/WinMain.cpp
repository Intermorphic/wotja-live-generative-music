//
// WinMain.cpp
// WotjaAPI_WinRT_demo
//
// Copyright (C) 2007-2024 Intermorphic Limited. All rights reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

#include "pch.h"

#include "resource.h"

// MARK: the WotjaAPI implementation
#include "WotjaAPI.h"

// MARK: some built-in test data
#include "WotjaURITestData.h"

using namespace winrt;
using namespace Windows::Foundation;


// MARK: Initial Wotja URI that the demo app starts playing.
static std::wstring wotjaURI;

// MARK: Window handles for main app and various controls.
HWND mainHwnd;
HWND hWndWotjaStatusText;
HWND hWndWotjaURILabel;
HWND hWndWotjaURITextEdit;
HWND hwndImportButton;
HWND hwndLoadAndPlayButton;
HWND hWndWotjaTipText1;
HWND hWndWotjaTipText2;
HWND hwndInstallWotjaButton;
HWND hwndUpdateWotjaButton;
HWND hwndTryAgainButton;
HWND hwndPlayButton;
HWND hwndLoadAlbum1Button;
HWND hwndLoadAlbum2Button;
HWND hwndLoadAlbum3Button;
HWND hwndShowAboutWotjaButton;
HWND hwndShowWotjaEULAButton;
HWND hwndShowWotjaButton;
HWND hwndShowDeveloperFAQButton;

HBRUSH BGColorBrush;

// MARK: Alter the layout depending on current Wotja status

#define WM_WOTJA_STATE_CHANGED (WM_USER + 0x0001)

void layoutHorizontalControls(const std::vector<HWND>& controlHwnds, float x, float y, float width, float height, float spacingPerItem) {
  if (controlHwnds.size() == 0) {
    return;
  }

  if (controlHwnds.size() == 1) {
    spacingPerItem = 0.0f;
  }
  float itemWidthWithSpacing = width / (float)controlHwnds.size();

  for (auto hwnd : controlHwnds) {
    SetWindowPos(hwnd, NULL, (int)x, (int)y, (int)(itemWidthWithSpacing - spacingPerItem), (int)height, 0);
    x += itemWidthWithSpacing;
  }
}

void updateLayout() {
  RECT rcClient; // screen size

  GetClientRect(mainHwnd, &rcClient);

  auto width = rcClient.right - rcClient.left;
  // auto height = rcClient.bottom - rcClient.top;

  SetWindowPos(hWndWotjaStatusText, NULL, 10, 10, width - 20, 20, 0);

  switch (WotjaAPI::getWotjaStatus()) {
  case WotjaAPI::WotjaStatus_looking_for_wotja:
    break;
  case WotjaAPI::WotjaStatus_not_found: {
    int buttonWidth = 120;
    int buttonX = (width - buttonWidth)/2;
    SetWindowPos(hwndInstallWotjaButton, NULL, buttonX, 40, buttonWidth, 20, 0);
    SetWindowPos(hwndTryAgainButton, NULL, buttonX, 70, buttonWidth, 20, 0);
  }
    break;
  case WotjaAPI::WotjaStatus_needs_update: {
    int buttonWidth = 120;
    int buttonX = (width - buttonWidth)/2;
    SetWindowPos(hwndUpdateWotjaButton, NULL, buttonX, 40, buttonWidth, 20, 0);
    SetWindowPos(hwndTryAgainButton, NULL, buttonX, 70, buttonWidth, 20, 0);
  }
    break;
  case WotjaAPI::WotjaStatus_found: {
    std::vector<HWND> controlHwnds{hwndImportButton, hWndWotjaURILabel, hWndWotjaURITextEdit, hwndLoadAndPlayButton, hwndPlayButton};
    layoutHorizontalControls(controlHwnds, 10.0f, 40.0f, width - (10.0f * 2.0f), 20.0f, 10.0f);

    std::vector<HWND> controlHwnds2 {hwndLoadAlbum1Button, hwndLoadAlbum2Button, hwndLoadAlbum3Button};
    layoutHorizontalControls(controlHwnds2, 60.0f, 80.0f, width - (60.0f * 2.0f), 20.0f, 10.0f);

    SetWindowPos(hWndWotjaTipText1, NULL, 10, 110, width - 20, 20, 0);
    SetWindowPos(hWndWotjaTipText2, NULL, 10, 140, width - 20, 80, 0);

    std::vector<HWND> controlHwnds3 {hwndShowAboutWotjaButton, hwndShowWotjaEULAButton, hwndShowWotjaButton, hwndShowDeveloperFAQButton};
    layoutHorizontalControls(controlHwnds3, 60.0f, 230.0f, width - (60.0f * 2.0f), 20.0f, 10.0f);

  }
    break;
  }
}

// MARK: Alter the displayed controls (text, visibility) depending on current Wotja status

void updateUIState() {
  std::wstring wotjaStatusString = WotjaAPI::getWotjaStatusString();
  auto         theText           = wotjaStatusString.c_str();
  SetWindowText(hWndWotjaStatusText, theText);
  SetWindowText(hWndWotjaURITextEdit, wotjaURI.c_str());

  if (WotjaAPI::getIsPlaying()) {
    SetWindowText(hwndPlayButton, L"Stop");
  } else {
    SetWindowText(hwndPlayButton, L"Play");
  }

  switch (WotjaAPI::getWotjaStatus()) {
  case WotjaAPI::WotjaStatus_not_found:
    ShowWindow(hWndWotjaURILabel, SW_HIDE);
    ShowWindow(hWndWotjaURITextEdit, SW_HIDE);
    ShowWindow(hWndWotjaTipText1, SW_HIDE);
    ShowWindow(hWndWotjaTipText2, SW_HIDE);
    ShowWindow(hwndPlayButton, SW_HIDE);
    ShowWindow(hwndInstallWotjaButton, SW_SHOWNOACTIVATE);
    ShowWindow(hwndUpdateWotjaButton, SW_HIDE);
    ShowWindow(hwndTryAgainButton, SW_SHOWNOACTIVATE);
    ShowWindow(hwndShowAboutWotjaButton, SW_HIDE);
    ShowWindow(hwndShowWotjaEULAButton, SW_HIDE);
    ShowWindow(hwndShowWotjaButton, SW_HIDE);
    ShowWindow(hwndShowDeveloperFAQButton, SW_HIDE);
    break;
  case WotjaAPI::WotjaStatus_needs_update:
    ShowWindow(hWndWotjaURILabel, SW_HIDE);
    ShowWindow(hWndWotjaURITextEdit, SW_HIDE);
    ShowWindow(hWndWotjaTipText1, SW_HIDE);
    ShowWindow(hWndWotjaTipText2, SW_HIDE);
    ShowWindow(hwndPlayButton, SW_HIDE);
    ShowWindow(hwndInstallWotjaButton, SW_HIDE);
    ShowWindow(hwndUpdateWotjaButton, SW_SHOWNOACTIVATE);
    ShowWindow(hwndTryAgainButton, SW_SHOWNOACTIVATE);
    ShowWindow(hwndShowAboutWotjaButton, SW_HIDE);
    ShowWindow(hwndShowWotjaEULAButton, SW_HIDE);
    ShowWindow(hwndShowWotjaButton, SW_HIDE);
    ShowWindow(hwndShowDeveloperFAQButton, SW_HIDE);
    break;
  case WotjaAPI::WotjaStatus_looking_for_wotja:
    ShowWindow(hWndWotjaURILabel, SW_HIDE);
    ShowWindow(hWndWotjaURITextEdit, SW_HIDE);
    ShowWindow(hWndWotjaTipText1, SW_HIDE);
    ShowWindow(hWndWotjaTipText2, SW_HIDE);
    ShowWindow(hwndPlayButton, SW_HIDE);
    ShowWindow(hwndInstallWotjaButton, SW_HIDE);
    ShowWindow(hwndUpdateWotjaButton, SW_HIDE);
    ShowWindow(hwndTryAgainButton, SW_HIDE);
    ShowWindow(hwndShowAboutWotjaButton, SW_HIDE);
    ShowWindow(hwndShowWotjaEULAButton, SW_HIDE);
    ShowWindow(hwndShowWotjaButton, SW_HIDE);
    ShowWindow(hwndShowDeveloperFAQButton, SW_HIDE);
    break;
  case WotjaAPI::WotjaStatus_found:
    ShowWindow(hWndWotjaURILabel, SW_SHOWNOACTIVATE);
    ShowWindow(hWndWotjaURITextEdit, SW_SHOWNOACTIVATE);
    ShowWindow(hWndWotjaTipText1, SW_SHOWNOACTIVATE);
    ShowWindow(hWndWotjaTipText2, SW_SHOWNOACTIVATE);
    ShowWindow(hwndPlayButton, SW_SHOWNOACTIVATE);
    ShowWindow(hwndInstallWotjaButton, SW_HIDE);
    ShowWindow(hwndUpdateWotjaButton, SW_HIDE);
    ShowWindow(hwndTryAgainButton, SW_HIDE);
    ShowWindow(hwndShowAboutWotjaButton, SW_SHOWNOACTIVATE);
    ShowWindow(hwndShowWotjaEULAButton, SW_SHOWNOACTIVATE);
    ShowWindow(hwndShowWotjaButton, SW_SHOWNOACTIVATE);
    ShowWindow(hwndShowDeveloperFAQButton, SW_SHOWNOACTIVATE);
    break;
  }
}

// MARK: Query clipboard for text

std::wstring getTextFromClipboard() {
  if (OpenClipboard(nullptr) == 0) {
    return std::wstring();
  }

  std::wstring lResult;

  HANDLE lhMem = GetClipboardData(CF_UNICODETEXT);
  if (lhMem != nullptr) {
    wchar_t *wstring = (wchar_t *)GlobalLock(lhMem);

    if (wstring == nullptr) {
    } else {
      lResult = wstring;
    }

    GlobalUnlock(lhMem);
    ::CloseClipboard();

    return lResult;
  }

  ::CloseClipboard();

  return lResult;
}

std::wstring createTemporaryFilePath() {

  TCHAR szTempFileName[MAX_PATH];
  TCHAR lpTempPathBuffer[MAX_PATH];

  //  Gets the temp path env string (no guarantee it's a valid path).
  DWORD dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
  if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
    return L"";
  }

  //  Generates a temporary file name.
  UINT uRetVal = GetTempFileName(lpTempPathBuffer, TEXT("DEMO"), 0, szTempFileName);
  if (uRetVal == 0) {
    return L"";
  }

  return std::wstring(szTempFileName);
}

// MARK: Use Wotja API to see if Wotja is available; and if so, with what version

void searchForWotja() {
  std::wstring cRequiredMinimumWotjaVersion = WotjaAPI::getFirstSupportedWotjaVersion();
  //cRequiredMinimumWotjaVersion              = L"29.9.9";
  WotjaAPI::searchForWotja(cRequiredMinimumWotjaVersion, [](WotjaAPI::WotjaStatus wotjaStatus) {
    // Update the UI to reflect the new status string!
    PostMessage(mainHwnd, WM_WOTJA_STATE_CHANGED, 0, 0);

    // If we've now found Wotja, we can start it playing automatically!
    switch (wotjaStatus) {
      case WotjaAPI::WotjaStatus_found:
        // Start Wotja playing in the background
        WotjaAPI::loadAndPlay(wotjaURI);
        break;
      case WotjaAPI::WotjaStatus_looking_for_wotja:
        break;
      case WotjaAPI::WotjaStatus_not_found:
        // TODO - give user option to install Wotja
        break;
      case WotjaAPI::WotjaStatus_needs_update:
        // TODO - give user option to update Wotja
        break;
      default:
        break;
    }
  });
}

// MARK: Main app message handler

LRESULT CALLBACK myWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
  switch (iMsg) {
    case WM_CLOSE:
      WotjaAPI::stop();
      DestroyWindow(hwnd);
      break;

    case WM_DESTROY:
      WotjaAPI::stop();
      PostQuitMessage(0);
      break;

    case WM_SIZE: {
      updateLayout();
    }
      break;

    case WM_PAINT: {
      // You can change this if you want...
      PAINTSTRUCT ps;
      //HDC hdc = 
      (void)BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
    }
      return 0L;

    case WM_WOTJA_STATE_CHANGED:
      updateUIState();
      updateLayout();
      return 0L;

    case WM_CTLCOLORSTATIC: {
      // Give static controls transparent background colour
      HDC hdcStatic = (HDC)wParam;
      SetBkMode(hdcStatic, TRANSPARENT);
      return (LRESULT)BGColorBrush;
    }

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
          // Menu item handling...
        case IDM_FILE_EXIT:
          PostMessage(hwnd, WM_CLOSE, 0, 0);
          return 0L;

        case IDC_IMPORT: {
          std::wstring text = getTextFromClipboard();
          if (text.starts_with(L"wotja://")) {
            text.erase(std::remove(text.begin(), text.end(), '\n'), text.cend());

            MessageBoxW(hwnd, L"Please wait for the Wotja URI to load and play", L"Loading import", MB_OK);
            wotjaURI = text;
            updateUIState();
            WotjaAPI::loadAndPlay(wotjaURI);
            updateUIState();
          } else {
            MessageBoxW(hwnd, L"wotja:// URL not found in clipboard", L"Import Failed", MB_OK);
          }
        }
          return 0L;

        case IDC_INSTALL_WOTJA:
          WotjaAPI::installWotja();
          return 0L;

        case IDC_UPDATE_WOTJA:
          WotjaAPI::updateWotja();
          return 0L;

        case IDC_TRY_AGAIN:
          searchForWotja();
          return 0L;

        case IDC_WOTJA_LOADANDPLAY:
          MessageBoxW(hwnd, L"Please wait for the Wotja URI to load and play", L"Loading and Play", MB_OK);
          WotjaAPI::loadAndPlay(wotjaURI);
          return 0L;

        case IDC_WOTJA_PLAY:
          if (WotjaAPI::getIsPlaying()) {
            WotjaAPI::stop();
          } else {
            WotjaAPI::playWithTrackIndex(0);
          }
          updateUIState();
          return 0L;

        case IDC_SHOW_ABOUT_WOTJA:
          WotjaAPI::showAboutWotja();
          return 0L;

        case IDC_SHOW_WOTJA_EULA:
          WotjaAPI::showWotjaEULA();
          return 0L;

        case IDC_SHOW_DEVELOPER_FAQ:
          WotjaAPI::showDeveloperFAQ();
          return 0L;

        case IDC_SHOW_WOTJA:
          WotjaAPI::showWotja();
          return 0L;

        case IDC_LOAD_ALBUM1: {
          wotjaURI = sWotjaURITestAlbum1;
          updateUIState();
          WotjaAPI::loadAndPlay(wotjaURI);
          updateUIState();
        }
          return 0L;

        case IDC_LOAD_ALBUM2: {
          wotjaURI = sWotjaURITestAlbum2;
          updateUIState();
          WotjaAPI::loadAndPlay(wotjaURI);
          updateUIState();
        }
          return 0L;

        case IDC_LOAD_ALBUM3: {
          wotjaURI = sWotjaURITestAlbum3;
          updateUIState();
          WotjaAPI::loadAndPlay(wotjaURI);
          updateUIState();
        }
          return 0L;
        }
      break;

    default:
      return DefWindowProc(hwnd, iMsg, wParam, lParam);
  }
  return 0;
}

// MARK: Utility - create static text control

HWND createStaticTextControl(const std::wstring& textString) {
  return CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", textString.c_str(), WS_CHILD | WS_VISIBLE | SS_CENTER | WS_SYSMENU, 0, 0, 0, 0, mainHwnd, (HMENU)NULL, (HINSTANCE)GetWindowLongPtr(mainHwnd, GWLP_HINSTANCE), NULL);
}

// MARK: Utility - create button

HWND createButtonControl(const std::wstring& textString, uint64_t controlId) {
  return CreateWindowW(L"BUTTON", textString.c_str(), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 0, 0, 0, 0, mainHwnd, (HMENU)controlId, (HINSTANCE)GetWindowLongPtr(mainHwnd, GWLP_HINSTANCE), NULL);
}

// MARK: WinMain function - the app entry point

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int iCmdShow) {
  init_apartment(apartment_type::single_threaded);

  MSG      msg;
  WNDCLASS wndclass;

  // Specify a window class
  wndclass.style         = 0;
  wndclass.lpfnWndProc   = myWndProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = hInstance;
  wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName  = MAKEINTRESOURCEW(IDR_MYMENU);
  wndclass.lpszClassName = L"Wotja API WinRT Demo";

  // Register the window class
  RegisterClass(&wndclass);

  // Create a window based on the window class
  mainHwnd = CreateWindowW(L"Wotja API WinRT Demo", L"Wotja API WinRT Demo", WS_OVERLAPPEDWINDOW, 20, 20, 600, 330, NULL, NULL, hInstance, NULL);

  BGColorBrush = CreateSolidBrush(RGB(255, 255, 255));

  hWndWotjaStatusText = createStaticTextControl(L"");
  hWndWotjaURILabel   = createStaticTextControl(L"Wotja URI:");

  // Edit field (disabled/non-editable)
  hWndWotjaURITextEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), L"", WS_CHILD | WS_DISABLED | WS_VISIBLE, 0, 0, 0, 0, mainHwnd, (HMENU)NULL, hInstance, NULL);

  hwndImportButton      = createButtonControl(L"Import", IDC_IMPORT);
  hwndLoadAndPlayButton = createButtonControl(L"Load && Play", IDC_WOTJA_LOADANDPLAY);
  hWndWotjaTipText1     = createStaticTextControl(L"Playing Live Generative Music by Wotja");
  hWndWotjaTipText2     = createStaticTextControl(L"Tip: To test a built-in Wotja URI: use of the three Play buttons, which use wotja://api_load\n\nTip:to test your own content: in the Wotja app, open an Album; export to clipboard; then press the Import button; this will play your data using wotja://api_load\n\nTip: wotja://api_load may take time to respond.");
  hwndPlayButton         = createButtonControl(L"Play", IDC_WOTJA_PLAY);
  hwndInstallWotjaButton = createButtonControl(L"Install Wotja", IDC_INSTALL_WOTJA);
  hwndUpdateWotjaButton  = createButtonControl(L"Update Wotja", IDC_UPDATE_WOTJA);
  hwndTryAgainButton     = createButtonControl(L"Try again", IDC_TRY_AGAIN);
  hwndLoadAlbum1Button   = createButtonControl(L"Album 1", IDC_LOAD_ALBUM1);
  hwndLoadAlbum2Button   = createButtonControl(L"Album 2", IDC_LOAD_ALBUM2);
  hwndLoadAlbum3Button   = createButtonControl(L"Album 3", IDC_LOAD_ALBUM3);

  hwndShowAboutWotjaButton    = createButtonControl(L"About Wotja", IDC_SHOW_ABOUT_WOTJA);
  hwndShowWotjaEULAButton     = createButtonControl(L"Wotja EULA", IDC_SHOW_WOTJA_EULA);
  hwndShowWotjaButton         = createButtonControl(L"Show Wotja", IDC_SHOW_WOTJA);
  hwndShowDeveloperFAQButton  = createButtonControl(L"Developer FAQ", IDC_SHOW_DEVELOPER_FAQ);

  // Display the window on the screen
  ShowWindow(mainHwnd, iCmdShow);
  UpdateWindow(mainHwnd);

  // Prepare our first Wotja content to play and load.
  wotjaURI = sWotjaURITestAlbum1;

  // MARK: On Windows, there is a system limit on URL calls of approximately 32 KB.
  // To work around this limit, any Wotja URI data that is to be loaded in a must be
  // passed via a temporary file rather than directly within the Wotja API URL.
  // Therefore, if you're using Wotja URIs of (say) 30 KB or more, you *must use WotjaAPI::setFilePathForURLSend(...)*
  // before loading any Wotja data,  as shown in the example project.
  // It is your reponsibility to ensure that any such file path is accessible both to your application,
  // and to the Wotja application.
  // It is the responsiblity of your application to remove this file before you close your application.
  std::wstring temporaryFilePath = createTemporaryFilePath();
  WotjaAPI::setFilePathForURLSend(temporaryFilePath);

  // Update the UI to reflect the current Wotja API state.
  updateUIState();
  updateLayout();

  // MARK: search for Wotja.
  // On success, this will load and play the Wotja URI that we have loaded in the wotjaURI variable.
  // Therefore, before this call, a few lines back, we called setFilePathForURLSend
  searchForWotja();

  // Run a message loop to take care of the message queue
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // Close file if it possible
  if (temporaryFilePath != L"") {
    _wunlink(temporaryFilePath.c_str());
  }

  return (int)msg.wParam;
}
