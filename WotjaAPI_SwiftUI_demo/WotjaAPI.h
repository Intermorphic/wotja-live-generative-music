//
// WotjaAPI.h
// WotjaAPI_SwiftUI_demo
//
// Copyright © 1990-2026 Intermorphic Limited. All Rights Reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

#import <Foundation/Foundation.h>

#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
  #define IM_TARGET_IOS 1
#elif TARGET_OS_IPHONE
  #define IM_TARGET_IOS 1
#else
  #define IM_TARGET_MACOS 1
#endif

@interface WotjaAPI : NSObject

- (instancetype _Nonnull)init;

// MARK: Wotja Playback and other API methods

+(void) loadAndPlay:(NSString* _Nonnull)wotjaURIString trackIndex:(NSInteger)trackIndex tempo:(NSString* _Nullable)tempo root:(NSString* _Nullable)root;
+(BOOL) getIsPlaying;
+(void) stop;
+(void) playWithTrackIndex:(NSInteger)trackIndex;
+(void) setTempo:(NSString* _Nonnull)tempo;
+(void) setRoot:(NSString* _Nonnull)root;

// MARK: Wotja Availability checking

typedef NS_ENUM(NSUInteger, WotjaStatus) {
  WotjaStatus_looking_for_wotja = 1,
  WotjaStatus_not_found = 2,
  WotjaStatus_needs_update = 3,
  WotjaStatus_found = 4,
};

// Returns e.g. @"Looking for Wotja..."
+(NSString* _Nonnull) convertWotjaStatusToString:(WotjaStatus)wotjaStatus;

// Returns e.g. @"Looking for Wotja..."
+(NSString* _Nonnull) getWotjaStatusString;

// Returns e.g. @"24.1.0"
+(NSString* _Nonnull) getFirstSupportedWotjaVersion;

+(WotjaStatus) getWotjaStatus;

+(void) searchForWotja:(NSString* _Nonnull)withMinimumVersion completion:(void(^_Nonnull)(WotjaStatus))completion;

// MARK: Wotja installation, update and miscellaneous

+(void) installWotja;
+(void) updateWotja;
+(void) showWotjaEULA;
+(void) showWotja;
+(void) showDeveloperFAQ;
+(void) showAboutWotja;

@end
