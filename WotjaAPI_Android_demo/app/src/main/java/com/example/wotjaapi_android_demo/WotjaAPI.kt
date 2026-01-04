//
// WotjaAPI.kt
// WotjaAPI_Android_demo
//
// Copyright © 1990-2026 Intermorphic Limited. All Rights Reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

package com.example.wotjaapi_android_demo

import android.content.ActivityNotFoundException
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.pm.PackageInfo
import android.content.pm.PackageManager
import android.net.Uri

class WotjaAPI(inContext: Context) {
    val context = inContext

    // On first use, launch Wotja app via a Wotja API URI
    var didLaunchWotjaWithURI = false
    var wotjaComponentName: ComponentName? = null
    var wotjaVersion = ""

    // Note that for this to work, we need a corresponding entry for "<queries>"
    // in this project's AndroidManifest.xml. Take a look at AndroidManifest.xml!
    fun getIsWotjaInstalled(): Boolean {
        if (wotjaComponentName != null) {
            return true
        }

        // The URI used here isn't loaded; it is just for testing.
        val myIntent = Intent(
            Intent.ACTION_VIEW,
            Uri.parse("wotja://api_load/?path=im/23_Mixed_1.wotja_sc")
        )

        // Verify the original intent will resolve to at least one activity
        wotjaComponentName = myIntent.resolveActivity(context.packageManager)

        if (wotjaComponentName != null) {
            //val packagesInstalledList: List<PackageInfo> = packageManager.getInstalledPackages(PackageManager.GET_META_DATA)
            var packageInfo: PackageInfo? = null
            try {
                packageInfo = context.packageManager?.getPackageInfo("com.intermorphic.WotjaGP", 0)
            } catch (e: PackageManager.NameNotFoundException) {
            }
            if (packageInfo == null) {
                try {
                    packageInfo =
                        context.packageManager?.getPackageInfo("com.intermorphic.WotjaAZ", 0)
                } catch (e: PackageManager.NameNotFoundException) {
                }
            }

            if (packageInfo != null) {
                wotjaVersion = packageInfo.versionName
            }

            return true
        }
        return false
    }

    fun getInstalledWotjaVersion(): String {
        return wotjaVersion
    }

    fun openURI(uriString: String) : Boolean {

        try {
            val myIntent = Intent(
                Intent.ACTION_VIEW,
                Uri.parse(uriString)
            )
            context.startActivity(myIntent)
        } catch (e: ActivityNotFoundException) {
            return false
        }
        return true
    }

    fun sendWotjaURIByActivity(wotjaURI: String): Boolean {
        // Assume that Wotja app not yet launched;
        // send the (first) Wotja API URI via an Intent that will cause
        // Wotja to both start, and process the Wotja URI
        if (openURI(wotjaURI)) {
            didLaunchWotjaWithURI = true
            return true
        }
        return false
    }

    // If the Wotja app has been launched via a previous Wotja API URI,
    // we should be able to send Wotja API URIs to Wotja via Broadcast;
    // this would have the effect that subsequent commands won't
    // bring Wotja back to the foreground (which can be annoying for 3rd party apps!)
    fun sendWotjaURIByBroadcast(
        wotjaURI: String,
        wotjaComponentName: ComponentName?
    ) {
        val intent = Intent()
        intent.setAction("com.intermorphic.WOTJA_URI_RECEIVER")
        intent.putExtra("data", wotjaURI)
        intent.setComponent(wotjaComponentName);
        context.sendBroadcast(intent)
    }

    fun callWotjaAPI(wotjaURI: String) {

        var useUriString = wotjaURI
        if (wotjaURI.startsWith("wotja://api_") == false) {
            // A plain data URI such as wotja://album/... (not a Wotja API URI) convert into a direct API call
            useUriString = "wotja://api_load/?data=" + wotjaURI.removePrefix("wotja://")
        }
        sendWotjaURIByActivity(useUriString)

        // We currently can't get the broadcast variant to work - if anybody can figure out why, please let us know!
//        if (didLaunchWotjaWithURI == false) {
//            sendWotjaURIByActivity(useUriString)
//        } else {
//            sendWotjaURIByBroadcast(useUriString, wotjaComponentName)
//        }
    }

    fun play() {
        callWotjaAPI("wotja://api_play/")
    }

    fun stop() {
        callWotjaAPI("wotja://api_stop/")
    }

    fun showWotjaInstallerPage() {
        openURI("https://wotja.com/downloads/#android")
    }

    fun showWotjaEULA() {
        openURI("https://wotja.com/legal/eula")
    }

    fun showDeveloperFAQ() {
        openURI("https://wotja.com/help/#developer")
    }

    fun showAboutWotja() {
        openURI("https://wotja.com/music/")
    }

    fun showWotja() {
        callWotjaAPI("wotja://api_show/")
    }
}
