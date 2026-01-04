# WotjaAPI_Android_demo

This project contains a simple Kotlin UI app, targeting Android.

The project shows how you can create a program for Android that passes a Wotja URI (WJURI) to the [Wotja App](https://wotja.com) for it to then generate live Generative Music from the data in that WJURI.

By copying and adapting the code in this project, into your own project, you can easily add-in Wotja background music to any game or visualiser written for Android, whether your game is written in Kotlin, Android, or C++.

# How does it work?

If the Wotja app isn't detected, or has an earlier version than your app requires, then your app shows a Toast message to prompt the user to install the Wotja app from the App Store.

Once the Wotja app is installed, the Wotja app becomes available, and the project can detect it and load it using "Wotja URI" data.

The code provided shows you how to do various things, including:
- detect for the presence of the Wotja app
- show the Wotja entry on the App Store so that the user can install Wotja if the app wasn't detected
- initialize the Wotja app and play your audio content through it (using a standard "Wotja URI")

# Wotja API - methods

Please refer to the [main README.md](../README.md) to find out more information.

# Wotja API - how it works

Please refer to the [main README.md](../README.md) to find out more information.

# Limitations

Please refer to the [main README.md](../README.md) to find out more information on general Limitations of the Wotja API.

On Android, it might take a short while for the Wotja app to respond to load operations.

On Android, every use of a Wotja URI from your Application will give the Wotja app the focus. We hope we might be able to find a workaround for this at some point.

The Wotja music will timeout automatically, unless the user has unlocked the Wotja 'Pro' feature set through the Wotja App in-app store.

Wotja API calls are one-way: your app can send commands to the Wotja app, but there is no way for the Wotja app to send messages or response back to your app.

In general, you will need to test your Android App or Game using a physical Android device.

The demo app could be improved by our adding some UI elements that use the Wotja API commands to set tempo and root (these are all implemented in the Apple and Windows demo apps)

Hopefully, we can figure-out a way to prevent the Wotja app getting the focus on every API call

# License

If not otherwise stated in the header of a file, all code in this project is released under the terms described in [LICENSE.md](./LICENSE.md)

Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula

Copyright © 1990-2026 Intermorphic Limited. All Rights Reserved.

