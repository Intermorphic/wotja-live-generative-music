# WotjaAPI_SwiftUI_demo

This project contains a simple Swift UI app, targeting both iOS and macOS.

The project shows how you can create a program for iOS and/or macOS that passes a Wotja URI (WJURI) to the [Wotja App](https://wotja.com) for it to then generate live Generative Music from the data in that WJURI.

By copying and adapting the code in this project, into your own project, you can easily add-in Wotja background music to any game or visualiser written for iOS or macOS, whether your game is written in Swift, Objective-C, or C++.

# How does it work?

If the Wotja app isn't detected, or has an earlier version than your app requires, then your app should prompt the user to install the Wotja app from the App Store.

Once the Wotja app is installed, the Wotja app becomes available, and the project can detect it and load it using "Wotja URI" data.

The code provided shows you how to do various things, including:
- detect for the presence of the Wotja app
- discover the version of the Wotja app (if the app has been detected)
- show the Wotja entry on the App Store so that the user can install Wotja if the app wasn't detected
- initialize the Wotja app and play your audio content through it (using a standard "Wotja URI")
- stop and re-start the Wotja app playing
- launch the Wotja application directly from your app

# Wotja API - methods

Please refer to [WotjaAPI.h](./WotjaAPI.h).

The WotjaAPI for macOS and iOS is supplied as Objective-C source code, which is easy to use from common Apple development languages: Swift, Objective-C, Objective-C++ and C++.

# Wotja API - how it works

Please refer to the [main README.md](../README.md) to find out more information.

# Limitations

Please refer to the [main README.md](../README.md) to find out more information on general Limitations of the Wotja API.

On iOS, it might take a short while for the Wotja app to respond to load operations.

On iOS, The initial launch of Wotja from your Application will always give Wotja the focus.

The Wotja music will timeout automatically, unless the user has unlocked the Wotja 'Pro' feature set through the Wotja App in-app store.

Wotja API calls are one-way: your app can send commands to the Wotja app, but there is no way for the Wotja app to send messages or response back to your app.

The iOS Simulator doesn't allow use of the App Store. Therefore, it isn't possible to use the Wotja API if you're testing your iOS App or Game on an iOS Simulator (unless you're a developer at Intermorphic!). As a result, this means that if you want to use Wotja music in your iOS app when testing, you'll need to do your testing on a physical iOS device.

# Essential entitlements.plist setting

It is *essential* that you set the following in your app's entitlements.plist file, otherwise the Wotja AUv3 cannot be found (or loaded!) by your app when running on iOS or macOS; and that prevents the Wotja API from detecting if the Wotja app is installed. Note that the Wotja AUv3 is otherwise not used by the Wotja API; it is used only for ease of detection of the installed Wotja app (and for determining the version of the installed Wotja app).

```
  <key>inter-app-audio</key>
  <true/>
```

# License

If not otherwise stated in the header of a file, all code in this project is released under the terms described in [LICENSE.md](./LICENSE.md)

Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula

Copyright © 1990-2026 Intermorphic Limited. All Rights Reserved.

