//
// WotjaAPI.m
// WotjaAPI_SwiftUI_demo
//
// Copyright © 1990-2026 Intermorphic Limited. All Rights Reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

#include "WotjaAPI.h"

#import <CoreAudioKit/CoreAudioKit.h>
#include <AudioUnit/AudioUnit.h>

@interface WotjaAPI()
@end

#if defined(IM_TARGET_IOS)
#define USE_MIDI_PORT 1
#endif // defined(IM_TARGET_IOS)

#if defined(IM_TARGET_MACOS)
// No need to use MIDI port on macOS, as NSWorkspace openURL works fine.
//#define USE_MIDI_PORT 1
#endif // defined(IM_TARGET_MACOS)

@implementation WotjaAPI

- (instancetype)init {
  self = [super init];
  if (self) {
  }
  return self;
}

// Private static method.
// Make an openURL call for iOS or macOS, in a platform specific way.
+(void) sOpenURL:(NSString*)urlString activates:(BOOL)activates {
  NSURL* url = [NSURL URLWithString:urlString];

#if defined(IM_TARGET_IOS)
  // On iOS, there is currently no way to prevent giving Wotja the UI focus - activates is ignored.
  // The user must manually "go back" to the calling application after any call to openURL.
  [[UIApplication sharedApplication] openURL:url options:@{} completionHandler:^(BOOL success) {
  }];
#elif defined(IM_TARGET_MACOS)
  NSWorkspaceOpenConfiguration* configuration = [NSWorkspaceOpenConfiguration configuration];
  configuration.activates = activates;

  [[NSWorkspace sharedWorkspace] openURL:url configuration:configuration completionHandler:^(NSRunningApplication * _Nullable app, NSError * _Nullable error) {
  }];
#endif // defined(TARGET_OS_MAC)
}

+(void) sOpenURLInForeground:(NSString*)urlString {
  [self sOpenURL:urlString activates:YES];
}

+(void) sOpenURLInBackground:(NSString*)urlString {
  // On iOS, there is currently no way to open Wotja in the background.
  // The user must manually "go back" to the calling application after any call to openURL.
  // On macOS, this works as expected.
  [self sOpenURL:urlString activates:NO];
}

// MARK: MIDI port handling (internal code)

#if defined(USE_MIDI_PORT)
// MIDI client, port and endpoint
static MIDIClientRef midiClient = 0;
static MIDIPortRef midiOutputPort = 0;
static MIDIEndpointRef wotjaEndpointRef = 0;
// State: record if we've sent the first command via a URL, or not.
static BOOL bSentFirstURL = false;

// Structure and callback; used to tidy-up the MIDI message data after it has been sent

typedef struct MySysexRequest_t {
  void* mData;
} MySysexRequest;

static void completionProc(MIDISysexSendRequest *request)
{
  if(request->complete) {
    MySysexRequest* mySysexRequest = (MySysexRequest*)request->completionRefCon;

    free(mySysexRequest->mData);
    free((void*)request);
  }
}

+(NSString*) sGetMidiObjectDisplayName:(MIDIObjectRef)endpoint {
  CFStringRef result = CFSTR("");

  MIDIObjectGetStringProperty(endpoint, kMIDIPropertyDisplayName, &result);

  if (result == nil) {
#if DEBUG
    NSLog(@"DEBUG: warning - kMIDIPropertyDisplayName returned nil");
#endif // DEBUG
    return @"Unknown";
  }

  NSString *resultNSString = (__bridge NSString *)result;

  CFRelease(result);

#if DEBUG
  //NSLog(@"DEBUG: kMIDIPropertyDisplayName is (%@)", resultNSString);
#endif // DEBUG

  return resultNSString;
}

void clientCompletionProc(const struct MIDINotification * notif , void * other ) {
  //NSLog(@"clientCompletionProc");
}

+(MIDIEndpointRef) sGetWotjaInputMIDIEndpointRef {
  unsigned long count = MIDIGetNumberOfDestinations();
  for (unsigned long i = 0; i < count; i++) {
    MIDIEndpointRef endpoint = MIDIGetDestination(i);
    if (endpoint != 0) {
      NSString*  name = [WotjaAPI sGetMidiObjectDisplayName:endpoint];
      if ([name isEqualToString:@"Wotja Input"]) {
        return endpoint;
      }
    }
  }

  return 0;
}

/// A helper that NSLogs an error message if "c" is an error code
void NSLogError(OSStatus c, NSString *str)
{
  if (c) {
    NSLog(@"Error (%@): %ld:%@", str, (long)c,[NSError errorWithDomain:NSMachErrorDomain code:c userInfo:nil]);
  }
}
#endif // defined(USE_MIDI_PORT)

// MARK: Public API method - make a Wotja API call.

// Note: the first call is always made via openURL, which on iOS will always give the Wotja App the UI focus; the user will have to "go back" the the calling app.
// Subsequent calls are made (where possible) via Wotja's "Wotja Input" virtual MIDI port, which won't give Wotja the UI focus (even on iOS).

+(BOOL) sMakeWotjaAPICall:(NSString*)wotjaURIString {

  if ([wotjaURIString hasPrefix:@"wotja://api_"] == false) {
    return NO;
  }

#ifdef DEBUG
  // NSLog(@"DEBUG: sMakeWotjaAPICall: processing Wotja URI of size %d characters, UTF8 string length=%d", (int)wotjaURIString.length, (int)[wotjaURIString lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
#endif // DEBUG

#if !defined(USE_MIDI_PORT)
  [WotjaAPI sOpenURLInBackground:wotjaURIString];
#elif defined(USE_MIDI_PORT)
  if (wotjaEndpointRef == 0) {
    wotjaEndpointRef = [WotjaAPI sGetWotjaInputMIDIEndpointRef];
  }

  if (wotjaEndpointRef != 0 && midiClient == 0) {
    // clientCompletionProc is called only if there is a NSRunLoop for this thread...
    OSStatus status = MIDIClientCreate(CFSTR("My Client"), clientCompletionProc, NULL, &midiClient);
    if (status != noErr) {
#if DEBUG
      NSLog(@"DEBUG: MIDIClientCreate failed!");
#endif // DEBUG
    } else {
      //status = MIDIOutputPortCreate(midiClient, CFSTR("My Port"), &midiOutputPort);
      status = MIDIInputPortCreateWithProtocol(midiClient, CFSTR("My Port"), kMIDIProtocol_2_0, &midiOutputPort, ^(const MIDIEventList * _Nonnull evtlist, void * _Nullable srcConnRefCon) {
#if DEBUG
          NSLog(@"DEBUG: MIDIInputPortCreateWithProtocol completed");
#endif // DEBUG
      });
      if (status != noErr) {
#if DEBUG
        NSLog(@"DEBUG; MIDIInputPortCreateWithProtocol failed!");
#endif // DEBUG
      }
    }
  }

  //
  // The Intial Wotja API call is made via the system openURL mechanism.
  // The call to openURL is also used if we were unable to establish a MIDI connection.
  //
  if (bSentFirstURL == false || midiClient == 0 || midiOutputPort == 0 || wotjaEndpointRef == 0) {
    // No MIDI; or very first URL; communicate using openURL.
    if (bSentFirstURL == false) {
#if DEBUG
      NSLog(@"DEBUG: Sending first wotja://api... as URL");
#endif // DEBUG
      bSentFirstURL = true;
    } else {
#if DEBUG
      NSLog(@"DEBUG: Warning: could not find Wotja Input MIDI port - sending second or later wotja://api... as URL");
#endif // DEBUG
    }
    [WotjaAPI sOpenURLInBackground:wotjaURIString];
    return NO;
  }

  //
  // Subsequent Wotja API calls are made via the Wotja Input MIDI port
  // (assuming we could find it!)
  //

  NSMutableData *sysexData = [NSMutableData new];
  char bytesToAppend[] = {
    0xf0, // Sysex start
    0x7D  // Manufacturer byte 1 (only)
  };
  [sysexData appendBytes:bytesToAppend length:sizeof(bytesToAppend)];

  NSData* apiData = [wotjaURIString dataUsingEncoding:NSUTF8StringEncoding];
  [sysexData appendData:apiData];

  char bytesToTerminate[] = {
    0xf7 // Sysex end
  };
  [sysexData appendBytes:bytesToTerminate length:sizeof(bytesToTerminate)];

  int bytesToSend = (int)[sysexData length];
  Byte *dataToSend = (Byte *)[sysexData bytes];

  // Use MIDISendSysex
  {
    Byte* theData = malloc([sysexData length]);
    memcpy(theData, dataToSend, bytesToSend);

    MySysexRequest* mySysexRequest = (MySysexRequest*)calloc(sizeof(MySysexRequest), 1);
    mySysexRequest->mData = theData;

    MIDISysexSendRequest* sendRequest = (MIDISysexSendRequest*)calloc(sizeof(MIDISysexSendRequest), 1);
    sendRequest->destination = wotjaEndpointRef;
    sendRequest->data = theData;
    sendRequest->bytesToSend = bytesToSend;
    sendRequest->complete = 0;
    sendRequest->completionProc = completionProc;
    sendRequest->completionRefCon = mySysexRequest;

    MIDISendSysex(sendRequest);
  }

  // Use MIDISend - messages sent like are never received by Wotja
  /*
  {
    int maxChunkSize = 512; // 65536;
    int packetSize = bytesToSend;
    int chunkSize = bytesToSend;
    if (packetSize <= maxChunkSize) {
      chunkSize = packetSize;
    } else {
      chunkSize = maxChunkSize;
    }

    int byteOffset = 0;
    int bytesRemaining = packetSize;
    while (bytesRemaining > 0) {
      // The maximum size is 65536 ... larger messages, must be split into separate packets!

      int bytesThisTime = bytesRemaining;
      if (bytesThisTime > chunkSize) {
        bytesThisTime = chunkSize;
      }

      {
        int PACKETLIST_SIZE = 1024 + sizeof(MIDIPacketList);
        MIDIPacketList* midiPacketList = (MIDIPacketList*)malloc(PACKETLIST_SIZE);
        //MIDIPacketList midiPacketList = {};
        MIDIPacket *currPacket = MIDIPacketListInit(midiPacketList);

        // If you want to send the MIDI data immediately, just provide a timestamp of 0.
        // If you call MIDIPacketListAdd with the same timestamp as the previous call, then it just concatenates your data to the previous packet. You will see packetList->numPackets and currentPacket stay the same, but currentPacket->length will increase.

        uint64_t timestamp = mach_absolute_time();

        currPacket = MIDIPacketListAdd(midiPacketList, PACKETLIST_SIZE, currPacket, timestamp, bytesThisTime, dataToSend + byteOffset);
        OSStatus status = MIDISend(midiOutputPort, wotjaEndpointRef, midiPacketList);
        if (status != noErr) {
          NSLogError(status, @"MIDISend");
        }

        // TODO - does this "free" prevent the MIDI being sent?
        //free(midiPacketList);

        bytesRemaining -= bytesThisTime;
        byteOffset += bytesThisTime;
      }
    }
  }
  */

  /*
  {
    MIDIEventList midiEventList = {};
    MIDIEventPacket* endPacket = MIDIEventListInit(&midiEventList, kMIDIProtocol_1_0);
    // The maximum size of a event list is 65536 bytes. Large sysex messages must be sent in smaller event lists.
    UInt32 *newMsg = [self convertMidi1ToMidi2WithData:pMsg size:size]; // convert byte array to UInt32 array
      event = MIDIEventListAdd(eventList, bufSize, event, 0, size, newMsg);
    endPacket = MIDIEventListAdd(&midiEventList,  65536, endPacket, 0, ump.count, ump)
    MIDISendEventList(outputPort, destination, &eventList)
  }
   */

  sysexData = nil;
#endif // !defined(USE_MIDI_PORT)

  return YES;
}

static bool mbPlaying = NO;

+(void) loadAndPlay:(NSString*)wotjaURIString trackIndex:(NSInteger)trackIndex tempo:(NSString*)tempo root:(NSString*)root {
  mbPlaying = YES;

  // Load and play using the provided Wotja URI
  if ([wotjaURIString hasPrefix:@"wotja://api_"] == YES) {
#ifdef DEBUG
    NSLog(@"Invalid string format!");
#endif // DEBUG
    return;
  }

  NSString* wotjaData = [wotjaURIString stringByReplacingOccurrencesOfString:@"wotja://" withString:@""];
  NSMutableString* apiURIString = [NSMutableString stringWithFormat:@"wotja://api_load/?data=%@", wotjaData];

  if (trackIndex < 0) {
    // Ignore it!
#ifdef DEBUG
    NSLog(@"Invalid track index!");
#endif // DEBUG
  } else {
    [apiURIString appendString: [NSString stringWithFormat:@"&ti=%d", (int)trackIndex]];
  }

  if (tempo == nil || [tempo isEqualToString:@"-"] || tempo.length == 0 || tempo.intValue < 0) {
    // Ignore it!
  } else {
    [apiURIString appendString: [NSString stringWithFormat:@"&tempo=%@", tempo]];
  }

  if (root == nil || [root isEqualToString:@"-"] || root.length == 0 ) {
    // Ignore it!
  } else {
    [apiURIString appendString: [NSString stringWithFormat:@"&root=%@", root]];
  }

#ifdef DEBUG
  //NSLog(@"loadAndPlay: apiURIString=%@", apiURIString);
#endif // DEBUG

  [self sMakeWotjaAPICall:apiURIString];
}

+(BOOL) getIsPlaying {
  return mbPlaying;
}

+(void) stop {
  mbPlaying = NO;

  [self sMakeWotjaAPICall:@"wotja://api_stop/"];
}

+(void) playWithTrackIndex:(NSInteger)trackIndex {
  mbPlaying = YES;

  [self sMakeWotjaAPICall:[NSString stringWithFormat:@"wotja://api_play/?ti=%d", (int)trackIndex]];
}

+(void) setTempo:(NSString*)tempo {
  if ([tempo isEqualToString:@"-"] || tempo.length == 0 || tempo.intValue < 0) {
    // Ignore it!
    return;
  }

  NSMutableString* apiURIString = [NSMutableString stringWithFormat:@"wotja://api_set/?tempo=%d", tempo.intValue];

  [self sMakeWotjaAPICall:apiURIString];
}

+(void) setRoot:(NSString*)root {
  if ([root isEqualToString:@"-"] || root.length == 0) {
    // Ignore it!
    return;
  }

  NSMutableString* apiURIString = [NSMutableString stringWithFormat:@"wotja://api_set/?root=%@", root];

  [self sMakeWotjaAPICall:apiURIString];
}


// MARK: Wotja availability checking

// Wotja API support added in the following Wotja version
static NSString* cMinimumWotjaVersion = @"26.0.0";

static WotjaStatus sWotjaStatus = WotjaStatus_looking_for_wotja;
static NSString* sWotjaVersion = @""; // Set internally to e.g. "26.0.0"
static NSString* sWotjaMinimumVersionRequired = @""; // Set by app to e.g. "26.0.0"

+(NSString*) getFirstSupportedWotjaVersion {
  return cMinimumWotjaVersion;
}

+(NSString*) getWotjaStatusString {
  return [self convertWotjaStatusToString:sWotjaStatus];
}

+(NSString*) convertWotjaStatusToString:(WotjaStatus)wotjaStatus {
  switch (wotjaStatus) {
  case WotjaStatus_not_found:
    return @"Wotja not found";
  case WotjaStatus_needs_update:
    return [NSString stringWithFormat:@"Wotja update required (found %@), needs %@", sWotjaVersion, sWotjaMinimumVersionRequired];
  case WotjaStatus_found:
    return [NSString stringWithFormat:@"Wotja found (version %@)", sWotjaVersion];
  case WotjaStatus_looking_for_wotja:
  default:
    return @"Looking for Wotja...";
  }
}

+(WotjaStatus) getWotjaStatus {
  return WotjaStatus_found; // sWotjaStatus;
}

+(void) searchForWotja:(NSString*)withMinimumVersion completion:(void(^_Nonnull)(WotjaStatus))completion {
  sWotjaMinimumVersionRequired = withMinimumVersion;

  dispatch_async(dispatch_get_main_queue(), ^{
    [self sPrivateSearchForWotja:withMinimumVersion completion:^(WotjaStatus result) {
      sWotjaStatus = result;
      completion(result);
    }];
  });
}

// MARK: Private methods

NSString* versionNumberToString(UInt32 versionNumber) {
  return [NSString stringWithFormat:@"%d.%d.%d", (int)(versionNumber>>16), (int)((versionNumber>>8) & 0xff), (int)(versionNumber & 0xff)];
}

UInt32 versionNumberFromString(NSString* value) {
  NSArray<NSString*>* components = [value componentsSeparatedByString:@"."];
  UInt32 theNumber = 0;

  for (NSString* component in components) {
    UInt32 testChar = (UInt32)[component integerValue];
    theNumber = (theNumber << 8) + testChar;
  }

  return theNumber;
}

+(void) sPrivateSearchForWotja:(NSString*)withMinimumVersion completion:(void(^_Nonnull)(WotjaStatus))completion {

  // On iOS, we found that canOpenURL doesn't work reliably for detecting the Wotja App.
  // However, scanning for the Wotja AUv3 works reliably (on both iOS and macOS).
  // The only way we found on iOS to check for Wotja version,
  // is to check for the AUv3 version; this also works well on macOS.
  // Therefore, we detect Wotja, and report the Wotja installed version,
  // by checking for the Wotja AUv3 and (if found) querying the Wotja AUv3 version.

  // Describe the Wotja AUv3 unit
  AudioComponentDescription audioComponentDescription;
  audioComponentDescription.componentType = 0; // Wildcard
  audioComponentDescription.componentSubType = 'Wotj';
  audioComponentDescription.componentManufacturer = 'InMo';
  audioComponentDescription.componentFlags = 0;
  audioComponentDescription.componentFlagsMask = 0;

  // Get the Wotja AUv3 unit
  AudioComponent audioComponent = AudioComponentFindNext(nil, &audioComponentDescription);
  if (audioComponent == nil) {
    sWotjaStatus = WotjaStatus_not_found;

    completion(WotjaStatus_not_found);

    return;
  }

  // Extract version string
  UInt32 versionNumber = 0;
  AudioComponentGetVersion(audioComponent, &versionNumber);
  sWotjaVersion = versionNumberToString(versionNumber);

  UInt32 minimumRequiredVersionNumber = versionNumberFromString(sWotjaMinimumVersionRequired);
  if (versionNumber < minimumRequiredVersionNumber) {
    completion(WotjaStatus_needs_update);
  } else {
    completion(WotjaStatus_found);
  }
}

// MARK: Wotja installation, update and miscellaneous

+(void) installWotja {
  [self sOpenURLInForeground:@"https://itunes.apple.com/app/id1344821101"];
}

+(void) updateWotja {
  [self sOpenURLInForeground:@"https://itunes.apple.com/app/id1344821101"];
}

+(void) showWotjaEULA {
  [self sOpenURLInForeground:@"https://wotja.com/legal/eula"];
}

+(void) showDeveloperFAQ {
  [self sOpenURLInForeground:@"https://wotja.com/help/#developer"];
}

+(void) showAboutWotja {
  [self sOpenURLInForeground:@"https://wotja.com/music/"];
}

+(void) showWotja {
  [self sOpenURLInForeground:@"wotja://api_show/"];
}

@end
