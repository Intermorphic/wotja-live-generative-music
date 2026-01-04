<img src="https://wotja.com/ws/images/svg/logo/wotja-website-img.svg" alt="Wotja" width="200"/>

# Demo Applications for Developers that show how to send a Wotja URI to a Wotja App for it to generate Live Generative Music

[Intermorphic](https://intermorphic.com), developers of [Wotja](https://wotja.com), have created a Generative Music API based around the use of cross-platform Wotja API URI strings. The Wotja API is supported by all Wotja app variants; for all of iOS, macOS, Windows and Android.

[Intermorphic](https://intermorphic.com), have also made available these iOS/macOS, Windows and Android demo projects to provide a starting point for small developers to explore the use by their app, game, visualiser etc. ('App/Game') of Live Generative Music. 

The Wotja API methods rely on passing a Wotja Album, Schema or Mix in the form of a Wotja URI (WJURI) to Wotja Software (specifically the Wotja App, see below) downloaded and installed by the App/Game End User on their device. Live Generative Music from the data in that WJURI can then be generated in the Wotja URI Player included in that Wotja App. 

# Where to download Wotja Software?

End Users can download the free [Wotja](https://wotja.com) App Bundle (which includes both a Wotja App and Wotja Plug-in) via the relevant platform-specific App Store. See: [Wotja downloads](https://wotja.com/downloads/). 

# What is a Wotja URI (WJURI)?

The filetypes used/saved by Wotja are Album (.wotja_al), and Schema (.wotja_sc), Mix (.wotja) and Template (.wotja_tp) files. 

The data in them is encoded in the form of URI link (wotja://*thewotjadata*), meaning they are text string data. We refer to this data as a Wotja URI ('WJURI'). A WJURI can be can be as little as 3Kb and can also now include the Wotja API methods below.

Note: *thewotjadata* itself will look something like e.g. "album/textstring...", "schema/textstring...", "mix/textstring..." or "template/textstring...".

## WotjaAPI_SwiftUI_demo

The project shows how you can create a program for iOS and/or macOS that passes a Wotja URI to the Wotja App for it to then generate Live Generative Music from the data in that WJURI.

- [Apple macOS/iOS WotjaAPI_SwiftUI_demo README.md](./WotjaAPI_SwiftUI_demo/README.md)

## WotjaAPI_WinRT_demo

The project shows how you can create a program for Windows that passes a Wotja URI to the Wotja App for it to then generate Live Generative Music from the data in that WJURI.

- [Windows WotjaAPI_WinRT_demo README.md](./WotjaAPI_WinRT_demo/README.md)

## WotjaAPI_Android_demo

The project shows how you can create a program for Android that passes a Wotja URI to the Wotja App for it to then generate Live Generative Music from the data in that WJURI.

- [Android WotjaAPI_Android_demo README.md](./WotjaAPI_Android_demo/README.md)

# How do they work?

The projects show how you can create a program for macOS, iOS, Windows or Android; that passes a Wotja URI to the Wotja App for it to then generate Live Generative Music from the data in that WJURI.

If the Wotja App isn't detected, or has an earlier version than required by your App/Game, then your App/Game should tell your End User to install or get the latest version of Wotja from the relevant platform-specific App Store.

Once the Wotja Bundle is installed, the project can detect the Wotja App and load it the Wotja API.

The project code provided shows you how to do various things, including:
- detect for the presence of the Wotja App
- discover the version of the Wotja App (if it has been detected)
- show the Wotja entry on the platform-specific App Store so that the user can install the Wotja Bundle if the Wotja App wasn't detected
- initialize the Wotja App and send to it a Wotja URI
- stop and re-start the Wotja App playing
- launch the Wotja App directly from your app
- manipulate some selected Wotja properties while playing (including Tempo and Root)

# Wotja API - how it works

The Wotja API methods rely on constructing a Wotja API URI in order to communicate with the Wotja App.

The communication is one-way: your app can send commands to the Wotja App, but there is no way for the Wotja App to send messages or response back to your app.

If you pass invalid properties or use invalid URI data, the API calls will generally fail silently.

The Wotja API URI is constructed as follows:

Simple case (no properties):
wotja://api_*method*/

When supplying one property:
wotja://api_*method*/?*property1*=*value1*

When supplying two properties:
wotja://api_*method*/?*property1*=*value1*&*property2*=*value*

etc.

# The following methods and properties are available in Wotja 24.1.0

## Load and play a Wotja URI [Mix, Album or Schema] (wotja://*thewotjadata*)

You *must* provide either the *data* or *path* property for any call to wotja://api_load. Which version you should use, is platform dependent, and is described below.

In the examples below, your Wotja URI is assumed to be in the format wotja://*thewotjadata*

### Load and play from data in the Wotja API URI

```
wotja://api_load/?data=thewotjadata
```

Note: this approach is recommended only for Apple (macOS/iOS) platforms.

The approach will also work on Windows, but *only* if your Wotja URI data is less than 32 KB. In general, for Windows we advise you always to use the path property (see below) rather than the data property.

### Windows only: load and play from file path, where that file contains the Wotja API URI data in UTF-8 format

wotja://api_load/?path=*pathtofilecontainingWotjaURI*

On Windows, you *must* use this approach for Wotja URIs that are > 32 KB; it is up to your app to ensure that the Wotja App can read documents from the document path your app specifies. The data in the file path you specify *must* contain the original Wotja URI data to play (which looks like "wotja://*thewotjadata*"), and *must* be in UTF-8 format.

The value for pathtofilecontainingWotjaURI must have escaped certain characters that are not valid in URIs, including:
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
wotja://api_load/?path=c:\Documents\myfile.txt

Must be encoded as:
wotja://api_load/?path=c%3A%5CDocuments%5Cmyfile.txt

And other expected variants, for example this string:
wotja://api_load/?path=c:\Documents\myfile.txt&tempo=100

Must be encoded as:
wotja://api_load/?path=c%3A%5CDocuments%5Cmyfile.txt&tempo=100
```

### Load wotja://*thewotjadata* but don't play immediately (requires subsequent call to wotja://api_play)

```
wotja://api_load/?play=false&data=thewotjadata
```

### Load and play album track index (from 0; default is 0):
```
wotja://api_load/?ti=0&data=thewotjadata
```

### Load and play with tempo and root:
```
wotja://api_load/?tempo=50&root=Cs&data=thewotjadata
```

## Play (already loaded Wotja URI [Mix, Album or Schema])

### Play (simple):
```
wotja://api_play/
```

### Play with tempo:
```
wotja://api_play/?tempo=50
```

### Play with root (Cs means C sharp, Cb means C flat)
```
wotja://api_play/?root=Cs
```

### Play with tempo and root:
```
wotja://api_play/?tempo=50&root=Cs
```

### Play album track index (from 0):
```
wotja://api_play/?ti=0
```

### Play album track index (from 0) with root (Cs means C sharp, Cb means C flat)
```
wotja://api_play/?ti=0&root=Cs
```

### Play album track index (from 0) with tempo in BPM (beats per minute, an integer value)
```
wotja://api_play/?ti=0&tempo=100
```

### Play album track index (from 0) with  root (Cs means C sharp, Cb means C flat) and tempo in BPM (beats per minute, an integer value)
```
wotja://api_play/?ti=0&root=Cs&tempo=100
```
    
## Stop (already loaded Wotja URI [Mix, Album or Schema])

### Stop:
```
wotja://api_stop/
```
    
## Set arbitrary property/properties dynamically

Currently, root and tempo are available to set while a WJURI is playing.

The values are set in the currently playing Mix, or track (Mix or auto-generated Mix from a Schema) in Album, or auto-generated Mix from the Schema.

Tempo is an integer value in Beats per Minute (BPM).

Root may be a value from A to G, or As to Gs (where the s includes "sharp"), or Ab to Gb (where the b includes "flat").

### Set root
```
wotja://api_set/?root=Cs
```

### Set tempo
```
wotja://api_set/?tempo=100
```

### Set root and tempo
```
wotja://api_set/?root=Cs&tempo=100
```

## Show/launch the Wotja App

### Show/launch Wotja App (and un-minimize if it is currently minimized):
```
wotja://api_show/
```

## Wotja Album examples

When using a Wotja Album WJURI, you can specify a track index to use (from 0 up).
Note: If you're using a Mix or a Schema WJURI, the track index parameter (if any) is ignored.

### Load and play track at index 1 in Wotja Album WJURI (the second track)
```
wotja://api_load/?ti=1&data=thewotjadata
```

### Load and play track at index 1 in Wotja Album WJURI (the second track), and set tempo and root at the same time
```
wotja://api_load/?ti=1&tempo=100&root=Cs&data=thewotjadata
```

### Play track at index 1 in Wotja Album WJURI (the second track)
```
wotja://api_play/?ti=1
```

### Play track at index 1 in Wotja Album WJURI (the second track), and set tempo and root at the same time
```
wotja://api_play?ti=1&tempo=100&root=Cs
```

# Limitations

The Wotja API is subject to change at any point, for any reason. We give no warranties as to its behaviour.

The Wotja App will be foregrounded when the first WJURI is passed to it.

It might take a short while for the Wotja App to respond to WJURI load operations.

The Wotja App operates by default in Lite mode. Lite mode has a number of limitations which include auto-timeouts, these being longer for Albums, shorter for Schemas and even shorter for Mixes. The Lite mode limitations for any version of Wotja Software are detailed in its related [Feature Set](https://wotja.com/app/#fst). To remove those limitations a Wotja App User can at any time Go Pro in the [Wotja App In-App Store](https://wotja.com/guide/#store).

Platform-specific limitations are dicussed in the project-specific README.md files:

- [Apple macOS/iOS WotjaAPI_SwiftUI_demo README.md](./WotjaAPI_SwiftUI_demo/README.md)
- [Windows WotjaAPI_WinRT_demo README.md](./WotjaAPI_WinRT_demo/README.md)
- [Android WotjaAPI_Android_demo README.md](./WotjaAPI_Android_demo/README.md)

# Other Platforms, Other SDKs

Those who are interested, might consider creating wrappers / plug-ins for 3rd party game SDKs such https://godotengine.org or https://unity.com or https://partner.steamgames.com/doc/sdk, or for 3rd party app SDKs such as https://www.qt.io

# License

If not otherwise stated in the header of a file, all code in this project is released under the terms described in [LICENSE.md](./LICENSE.md).

Note: For the avoidance of doubt this project and the Demo Applications in it are provided as is and are subject to change. They do not include Wotja Software and confer no rights whatsoever in Wotja Software which if required by the End User of the App/Game can be be downloaded by that End User via the relevant platform-specific App Store. The use of Wotja Software is governed solely by the Wotja EULA to be found at https://wotja.com/legal/eula.

Copyright (C) 1990-2026 [Intermorphic Limited](https://intermorphic.com). All rights reserved.

