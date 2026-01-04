//
// MainActivity.kt
// WotjaAPI_Android_demo
//
// Copyright © 1990-2026 Intermorphic Limited. All Rights Reserved.
// Licensed under the MIT License.
// Note: For the avoidance of doubt use of Wotja is governed by the Wotja EULA to be found at https://wotja.com/legal/eula
//

package com.example.wotjaapi_android_demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.example.wotjaapi_android_demo.ui.theme.WotjaAPI_Android_demoTheme
import java.util.Timer
import java.util.TimerTask


class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContent {
            WotjaAPI_Android_demoTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    App()
                }
            }
        }
    }

}

var sWotjaAPI:WotjaAPI? = null

@Composable
fun App() {
    val context = LocalContext.current

    if (sWotjaAPI == null) {
        sWotjaAPI = WotjaAPI(context)
    }

    val wotjaAPI = sWotjaAPI!!

    var isWotjaInstalled: Boolean by remember { mutableStateOf(wotjaAPI.getIsWotjaInstalled()) }

    var isPlaying: Boolean by remember { mutableStateOf(false) }
    var hasFirstLoadBeenCalled: Boolean by remember { mutableStateOf(false) }

    // Use the three Wotja URI strings that are built-in to this demo application
    val wotjaAPILoadURIs:Array<String> = arrayOf(
        context.getString(com.example.wotjaapi_android_demo.R.string.album1URI),
        context.getString(com.example.wotjaapi_android_demo.R.string.album2URI),
        context.getString(com.example.wotjaapi_android_demo.R.string.album3URI))


    val timer = Timer()

    val task: TimerTask = object : TimerTask() {
        override fun run() {
            if (isWotjaInstalled) {
                timer.cancel()
                timer.purge()
                return
            }

            isWotjaInstalled = wotjaAPI.getIsWotjaInstalled()

            if (isWotjaInstalled) {
                timer.cancel()
                timer.purge()
            }
        }
    }

    timer.schedule(task, 1000L, 1000L)

    Column (
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {

        if (isWotjaInstalled == false) {
            TextButton(onClick = {
                wotjaAPI.showWotjaInstallerPage()
            }) {
                Text("Wotja not found!\n\nPlease install Wotja from\n\n https://wotja.com/downloads/#android")
            }

        } else {
            // Wotja found!
            Text(text = "Wotja " + wotjaAPI.getInstalledWotjaVersion(),
                color = Color.Gray
            )

            var i = 1
            for (wotjaURI in wotjaAPILoadURIs) {
                TextButton(onClick = {
                    wotjaAPI.callWotjaAPI(wotjaURI)
                    hasFirstLoadBeenCalled = true
                    isPlaying = true
                }) {
                    Text("Load & Play Wotja URI #" + i)
                }
                i = i + 1
            }
            if (hasFirstLoadBeenCalled && isPlaying) {
                TextButton(onClick = {
                    wotjaAPI.stop()
                    isPlaying = false
                }) {
                    Text("Stop")
                }
            }


            if (hasFirstLoadBeenCalled && isPlaying == false) {
                TextButton(onClick = {
                    wotjaAPI.play()
                    isPlaying = true
                }) {
                    Text("Play")
                }
            }

            Text(text = "Tip: To test a built-in Wotja URI: use of the three Load & Play buttons, which use wotja://api_load\n\nTip:to test your own content: in the Wotja app, open an Album; export to clipboard; then press the Import button; this will play your data using wotja://api_load\n\nTip: wotja://api_load may take a few seconds to respond.\n",
                color = Color.Black,
                modifier = Modifier.padding(horizontal = 10.dp)
                )
        }

        TextButton(onClick = {
            wotjaAPI.showAboutWotja()
        }) {
            Text("About Wotja")
        }

        TextButton(onClick = {
            wotjaAPI.showWotjaEULA()
        }) {
            Text("Wotja EULA")
        }

        if (isWotjaInstalled == true) {
            TextButton(onClick = {
                wotjaAPI.showWotja()
            }) {
                Text("Show Wotja")
            }
        }

        TextButton(onClick = {
            wotjaAPI.showDeveloperFAQ()
        }) {
            Text("Developer FAQ")
        }
    }
}


@Preview(showBackground = true)
@Composable
fun ShowSimpleUIPreview() {
    WotjaAPI_Android_demoTheme {
        App()
    }
}
