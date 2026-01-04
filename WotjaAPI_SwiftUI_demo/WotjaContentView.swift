//
// WotjaContentView.swift
// WotjaAPI_SwiftUI_demo
//
// Copyright © 1990-2026 Intermorphic Limited. All Rights Reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

import SwiftUI

import CoreAudioKit

import AudioUnit
import CoreAudioKit
import CoreAudioTypes

struct WotjaContentView: View {

  @State var wotjaURI:String = sWotjaURITestAlbum1

  static var checkFoundTimer = Timer()
  static var monitorPlayTimer = Timer()

  @State var wotjaPluginStatus:String = WotjaAPI.getWotjaStatusString()
  @State var playStopButtonText: String = "Play"

  init() {
    searchForWotja()
  }

  let cRequiredMinimumWotjaVersion = WotjaAPI.getFirstSupportedWotjaVersion()

  func searchForWotja() {
    WotjaAPI.search(forWotja: cRequiredMinimumWotjaVersion) {  result in
      wotjaPluginStatus = WotjaAPI.getWotjaStatusString()
    }
  }

  var isPreview: Bool {
    return ProcessInfo.processInfo.environment["XCODE_RUNNING_FOR_PREVIEWS"] == "1"
  }

#if os(OSX)
  private let pasteboard = NSPasteboard.general

  private func getPasteboardString() -> String? {
    let text = pasteboard.string(forType: NSPasteboard.PasteboardType.string)
    return text
  }
#else
  private let pasteboard = UIPasteboard.general

  private func getPasteboardString() -> String? {
    return pasteboard.string
  }
#endif

  @State private var showAlert = false
  @State private var alertMessage:String = ""
  @State private var alertTitle:String = ""

  // MARK: Track# (trackIndex) picker
  let trackIndexArray = [0, 1, 2, 3, 4, 5]
  @State private var trackIndex: Int = 0

  // MARK: Tempo picker
  enum Tempo: String, CaseIterable, Identifiable {
    case none = "-"
      case tempo60 = "60", tempo70 = "70", tempo80 = "80", tempo90 = "90", tempo100 = "100"
      var id: Self { self }
  }
  @State private var selectedTempo: Tempo = .none

  // MARK: Root picker
  enum Root: String, CaseIterable, Identifiable {
    case none = "-",
         aflat = "Ab",
         a = "A",
         asharp = "As",
         bflat = "Bb",
         b = "B",
         c = "C",
         csharp = "Cs",
         dflat = "Db",
         d = "D",
         dsharp = "Ds",
         eflat = "Eb",
         e = "E",
         f = "F",
         fsharp = "Fs",
         gflat = "Gb",
         g = "G",
         gsharp = "Gs"
    var id: Self { self }
  }
  @State private var selectedRoot: Root = .none

#if os(macOS)
  let pickerWidth = 180.0
  let pickerHeight = 40.0
#else
  let pickerWidth = 180.0
  let pickerHeight = 100.0
#endif

  func doLoadAndPlay() {
    WotjaAPI.loadAndPlay(self.wotjaURI, trackIndex:self.trackIndex, tempo:self.selectedTempo.rawValue, root: self.selectedRoot.rawValue)
  }

  var body: some View {
    VStack {
      Text(self.wotjaPluginStatus)
        .frame(width:350, height:40)
        .foregroundStyle(Color.gray).padding(EdgeInsets(top:0, leading:0, bottom:0, trailing:0))

      if (WotjaAPI.getWotjaStatus() == .found) {
        VStack {
          HStack {

            Text("Wotja URI:")
              .foregroundStyle(Color.gray)

            TextField(
              "",
              text: self.$wotjaURI
            )
            .textFieldStyle(.roundedBorder)
            .disabled(true)

            Button("Import") {
              // Import from clipboard
              if let text = getPasteboardString() {
                if text.starts(with:"wotja://") {
                  self.alertTitle = "Imported data"
                  self.alertMessage = "wotja:// URL found in clipboard - please wait for it to load and play"
                  self.showAlert = true

                  self.wotjaURI = text
                  doLoadAndPlay()
                } else {
                  self.alertTitle = "Import failed"
                  self.alertMessage = "wotja:// URL not found in clipboard"
                  self.showAlert = true
                }
              }
            }
            .buttonStyle(WotjaAPIButtonStyle())

          }.frame(width:350, height:40)

          HStack(alignment:.center) {
            Text("Toggle play/stop:")
              .foregroundStyle(Color.gray)

            Text("Track#:")
              .foregroundStyle(Color.gray)
              Picker(selection: $trackIndex, label: Text(""))
              {
                  ForEach(trackIndexArray, id: \.self) { value in
                      Text("\(value)").tag(value)
                  }
              }
            .frame(width:60, height:pickerHeight)

            Button(self.playStopButtonText) {
              if (WotjaAPI.getIsPlaying()) {
                WotjaAPI.stop()
              } else {
                WotjaAPI.play(withTrackIndex:self.trackIndex)
              }
            }
            .buttonStyle(WotjaAPIButtonStyle())
          }.frame(width:350, height:40)

          HStack {

            Text("Tempo:")
              .foregroundStyle(Color.gray)
            Picker("Tempo", selection: self.$selectedTempo) {
                ForEach(Tempo.allCases) { tempo in
                    Text(tempo.rawValue.capitalized)
                }
            }

            Text("Root:")
              .foregroundStyle(Color.gray)
            Picker("Root", selection: self.$selectedRoot) {
                ForEach(Root.allCases) { root in
                    Text(root.rawValue.capitalized)
                }
            }

            Button("Set") {
              self.wotjaURI = sWotjaURITestAlbum1
              WotjaAPI.setRoot(self.selectedRoot.rawValue)
              WotjaAPI.setTempo(self.selectedTempo.rawValue)
            }
            .buttonStyle(WotjaAPIButtonStyle())
          }.frame(width:380, height:40)


          HStack {
            Text("Load & play:")
              .foregroundStyle(Color.gray)
            
            Button("Album 1") {
              self.wotjaURI = sWotjaURITestAlbum1
              doLoadAndPlay()
            }
            .buttonStyle(WotjaAPIButtonStyle())

            Button("Album 2") {
              self.wotjaURI = sWotjaURITestAlbum2
              doLoadAndPlay()
            }
            .buttonStyle(WotjaAPIButtonStyle())

            Button("Album 3") {
              self.wotjaURI = sWotjaURITestAlbum3
              doLoadAndPlay()
            }
            .buttonStyle(WotjaAPIButtonStyle())

          }.frame(width:350, height:40)

          Text("Playing Live Generative Music by Wotja")
            .minimumScaleFactor(1.0)
            .lineLimit(1)
            .frame(height:30)

          Text("Tip: To test a built-in Wotja URI: use of the three Load & play buttons, which use wotja://api_load\n\nTip:to test your own content: in the Wotja app, open an Album; export to clipboard; then press the Import button; this will play your data using wotja://api_load\n\nTip: wotja://api_load may take a few seconds to respond.")
            .minimumScaleFactor(0.5)
            .lineLimit(8)
            .frame(height:120)
            .padding(EdgeInsets(top:0, leading:0, bottom:20, trailing:0))
        }.frame(width:350, height:350)
          .padding(EdgeInsets(top:0, leading:0, bottom:0, trailing:0))
      }

      let wotjaStatus = WotjaAPI.getWotjaStatus();
      if (wotjaStatus == .looking_for_wotja) {
      } else if (wotjaStatus == .not_found) {
        VStack {
          Button("Install Wotja") {
            WotjaAPI.installWotja()
          }
          .buttonStyle(WotjaAPIButtonStyle())

          Button("Try again") {
            searchForWotja()
          }
          .buttonStyle(WotjaAPIButtonStyle())
        }
      } else if (wotjaStatus == .needs_update) {

        VStack {
          Button("Update Wotja") {
            WotjaAPI.updateWotja()
          }
          .buttonStyle(WotjaAPIButtonStyle())

          Button("Try again") {
            searchForWotja()
          }
          .buttonStyle(WotjaAPIButtonStyle())
        }
      }

      if (wotjaStatus == .needs_update || wotjaStatus == .found) {
        HStack {
          Button("About Wotja") {
            WotjaAPI.showAboutWotja()
          }
          .buttonStyle(WotjaAPIButtonStyle())

          Button("Wotja EULA") {
            WotjaAPI.showWotjaEULA()
          }
          .buttonStyle(WotjaAPIButtonStyle())

          Button("Show Wotja") {
            WotjaAPI.showWotja()
          }
          .buttonStyle(WotjaAPIButtonStyle())

          Button("Developer FAQ") {
            WotjaAPI.showDeveloperFAQ()
          }
          .buttonStyle(WotjaAPIButtonStyle())

        }.scaledToFit()
      }
    }
    .padding(EdgeInsets(top:0, leading:10, bottom:0, trailing:10))
    .onAppear {
      WotjaContentView.checkFoundTimer = Timer.scheduledTimer(withTimeInterval: 1.0, repeats: true) { _ in
        wotjaPluginStatus = WotjaAPI.getWotjaStatusString()

        let wotjaStatus = WotjaAPI.getWotjaStatus();
        switch (wotjaStatus) {
        case .looking_for_wotja:
          break
        case .not_found,
            .needs_update,
            .found:
          WotjaContentView.checkFoundTimer.invalidate()
          break
        @unknown default:
          break
        }

        if (wotjaStatus == .found) {
          if (isPreview == false) {
            doLoadAndPlay()
          }
        }
      }

      WotjaContentView.monitorPlayTimer = Timer.scheduledTimer(withTimeInterval: 1.0, repeats: true) { _ in

        let wotjaStatus = WotjaAPI.getWotjaStatus();
        switch (wotjaStatus) {
        case .found:
          if WotjaAPI.getIsPlaying() {
            self.playStopButtonText = "Stop"

          } else {
            self.playStopButtonText = "Play"
          }
          break
        case .looking_for_wotja,
            .not_found,
            .needs_update:
          break
        @unknown default:
          break
        }
      }
    }
  }
}

#Preview {
    WotjaContentView()
}
