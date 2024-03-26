# Wotja API WinRT App Demo

This project contains a simple Win32 UI app, targeting Windows, using WinRT/C++.

The project shows how you can create a program for iOS and/or macOS that passes a Wotja URI (WJURI) to the [Wotja App](https://wotja.com) for it to then generate live Generative Music from the data in that WJURI.

By copying and adapting the code in this project, into your own project, you can easily add-in Wotja background music to any game or visualiser written for iOS or macOS, whether your game is written in C++, C, C# or some other language.

# How does it work?

If the Wotja App isn't detected, or has an earlier version than your app requires, then your app should the user to install the Wotja app from the Microsoft Store.

Once the Wotja app is installed, the project can detect it and load it the Wotja API.

The code provided shows you how to do various things, including:
- detect for the presence of the Wotja App
- discover the version of the Wotja App (if the App has been detected)
- show the Wotja entry on the Microsoft Store so that the user can install Wotja if the app wasn't detected
- initialize the Wotja App and play your audio content through it (using a standard "Wotja URI")
- stop and re-start the Wotja app playing
- launch the Wotja application directly from your app

# Wotja API - methods

Please refer to [WotjaAPI.h](./WotjaAPI.h).

The WotjaAPI for Windows is supplied as C++ source code, which we hope will be easy to use from common Windows development languages.

# Wotja API - how it works

Please refer to the [main README.md](../README.md) to find out more information.

# Limitations

Please refer to the [main README.md](../README.md) to find out more information on general Limitations of the Wotja API.

On Windows, there is a system limit on URI calls of approximately 32 KB.
To work around this limit, any Wotja URI data that is to be loaded in a *must* be passed via a temporary file rather than directly within the Wotja API URI.
Therefore, if you're using Wotja URIs of (say) 30 KB or more, you *must* use *WotjaAPI::setFilePathForURISend(...)*,
*before* you attempt to load any Wotja data, and you must use the *path* property of wotja:api_load/?...);
this approach is used in the example project.
It is your reponsibility to ensure that any such file is accessible both by your application, and by the Wotja application.
It is the responsiblity of your application to remove this file before you close your application.

The only valid characters for a Wotja URI are those in the following character set:
```
static constexpr const char* *cWotjaURI_ValidCharsSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
```

Any character intended for the *path* property of *wotja://api_load/?path=* that is *not* in the above character set, must be escaped; for example:
```
  " "  -> "%20"
  "-"  -> "%2D"
  ":"  -> "%3A"
  ";"  -> "%3B"
  "["  -> "%5B"
  "]"  -> "%5D"
  "\\" -> "%5C"
```

```
So this string:
wotja://api_load/?path=c:\My Documents\myfile.txt

Must be encoded as:
wotja://api_load/?path=c%3A%5CMy%20Documents%5Cmyfile.txt

And other expected variants, for example this string:
wotja://api_load/?path=c:\My Documents\myfile.txt&tempo=100

Must be encoded as:
wotja://api_load/?path=c%3A%5CMy%20Documents%5Cmyfile.txt&tempo=100
```

# License

If not otherwise stated in the header of a file, all code in this project is released under the terms described in [LICENSE.md](./LICENSE.md)

Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula

Copyright (C) 2007-2024 Intermorphic Limited. All Rights Reserved.

