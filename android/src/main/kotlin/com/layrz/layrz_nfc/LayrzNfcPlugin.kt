@file:Suppress("DEPRECATION", "SpellCheckingInspection", "MissingPermission", "KotlinConstantConditions")
package com.layrz.layrz_nfc

import android.app.Activity
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.nfc.NfcAdapter
import android.nfc.Tag
import android.nfc.tech.MifareClassic
import android.nfc.tech.Ndef
import android.nfc.tech.NfcA
import android.os.Build
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.embedding.engine.plugins.activity.ActivityAware
import io.flutter.embedding.engine.plugins.activity.ActivityPluginBinding
import io.flutter.plugin.common.PluginRegistry
import android.os.Handler
import android.os.Looper
import android.widget.Toast
import io.flutter.Log
import io.flutter.plugin.common.MethodChannel
import java.util.concurrent.atomic.AtomicBoolean


/** LayrzNfcPlugin */
class LayrzNfcPlugin: LayrzNfcPlatformChannel, FlutterPlugin, ActivityAware {
  private var activity: Activity? = null
  private var mainLooper: Handler? = null
  private lateinit var context: Context
  private var callbackChannel: LayrzNfcCallbackChannel? = null

  companion object {
    private const val TAG = "LayrzNfcPlugin/Android"
  }

  private var adapter: NfcAdapter? = null

  override fun onAttachedToEngine(binding: FlutterPlugin.FlutterPluginBinding) {
    LayrzNfcPlatformChannel.setUp(binding.binaryMessenger, this)
    context = binding.applicationContext
    mainLooper = Handler(Looper.getMainLooper())
    callbackChannel = LayrzNfcCallbackChannel(binding.binaryMessenger)
    adapter = NfcAdapter.getDefaultAdapter(context)
  }

  override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {
    mainLooper = null
    callbackChannel = null
  }

  override fun onAttachedToActivity(binding: ActivityPluginBinding) {
    activity = binding.activity
  }

  override fun onDetachedFromActivityForConfigChanges() {
    Log.w(TAG, "Call onDetachedFromActivityForConfigChanges")

  }

  override fun onReattachedToActivityForConfigChanges(binding: ActivityPluginBinding) {
    Log.w(TAG, "Call onReattachedToActivityForConfigChanges")
  }

  override fun onDetachedFromActivity() {
    Log.w(TAG, "Call onDetachedFromActivity")
    activity = null
  }

  override fun checkCapabilities(callback: (Result<Boolean>) -> Unit) {
    if (adapter != null) {
      callback(Result.success(true))
      return
    }
    Log.w(TAG, "Nfc unsupported")
    callback(Result.success(false))
  }

  override fun canRead(callback: (Result<Boolean>) -> Unit) {
    callback(Result.success(canNfc()))
  }

  override fun canWrite(callback: (Result<Boolean>) -> Unit) {
    callback(Result.success(canNfc()))
  }

  override fun canSimulate(callback: (Result<Boolean>) -> Unit) {
    callback(Result.success(canNfc()))
  }

  override fun startReading(callback: (Result<Boolean>) -> Unit) {
    if (adapter == null) {
      Log.w(TAG, "NFC unsupported")
      callback(Result.success(false))
      return
    }

    adapter!!.enableReaderMode(
       activity,
       nfcCallbacks,
       NfcAdapter.FLAG_READER_NFC_A or NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK,
       null
    )

    Log.w(TAG, "Start Reading ...")
    callback(Result.success(true))
    return
  }

  override fun stopReading(callback: (Result<Boolean>) -> Unit) {
    Log.w(TAG, "Stop Reading ...")

    if (adapter == null) {
      Log.w(TAG, "NFC unsupported")
      callback(Result.success(true))
      return
    }

    adapter!!.disableReaderMode(activity)
    callback(Result.success(true))
  }

  private fun canNfc(): Boolean {
    if (adapter?.isEnabled != true) {
      Log.w(TAG, "NFC unsuported")

      return false
    }
    return true
  }

  private val nfcCallbacks = NfcAdapter.ReaderCallback { tag ->
    Log.w(TAG, "onTagDiscovered")

    val techList = tag.techList
    val tagFormat = when {
      techList.contains("android.nfc.tech.MifareClassic") -> TagFormat.MIFARE_CLASSIC
      techList.contains("android.nfc.tech.NfcA") -> TagFormat.NFC_FORUM_TYPE2
      else -> TagFormat.UNKNOWN
    }

    Log.d(TAG, "TechList: ${techList.joinToString(", ")}")

    when (tagFormat) {
      TagFormat.MIFARE_CLASSIC -> {
        try {
          val mifare = MifareClassic.get(tag)
          mifare.connect()

          val keys = listOf(
            MifareClassic.KEY_DEFAULT,
            byteArrayOf(0xA0.toByte(), 0xA1.toByte(), 0xA2.toByte(), 0xA3.toByte(), 0xA4.toByte(), 0xA5.toByte()),
            byteArrayOf(0xD3.toByte(), 0xF7.toByte(), 0xD3.toByte(), 0xF7.toByte(), 0xD3.toByte(), 0xF7.toByte())
            // Add more if needed
          )

          var payload: ByteArray = byteArrayOf()
          val authMap = mutableMapOf<Int, Boolean>()

          for (block in 0 until mifare.blockCount) {
            if ((block + 1) % 4 == 0) continue // Skip trailer blocks

            val sector = mifare.blockToSector(block)
            val alreadyAuth = authMap[sector] ?: false

            if (!alreadyAuth) {
              var auth = false
              for (key in keys) {
                if (mifare.authenticateSectorWithKeyA(sector, key)) {
                  //Log.d(TAG, "Authenticated sector $sector with Key A")
                  auth = true
                  break
                } else if (mifare.authenticateSectorWithKeyB(sector, key)) {
                  //Log.d(TAG, "Authenticated sector $sector with Key B")
                  auth = true
                  break
                }
              }

              authMap[sector] = auth
            }

            if (!authMap[sector]!!) {
              Log.w(TAG, "Failed to authenticate sector $sector")
              continue
            }

            try {
              val data = mifare.readBlock(block)
              //Log.d(TAG, "Read block $block: $data")
              payload += data
            } catch (e: Exception) {
              Log.w(TAG, "Error reading block $block", e)
            }
          }

          mifare.close()

          if (payload.isNotEmpty()) {
            Log.d(TAG, "Sending payload to callback: $payload")

            mainLooper?.post {
              callbackChannel?.onRead(
                payloadArg = TagPayload(
                  payload = payload,
                  format = TagFormat.MIFARE_CLASSIC,
                )
              ) {}
            }
          }
        } catch (e: Exception) {
          Log.w(TAG, "Error reading MifareClassic tag $e")
        }
      }
      TagFormat.NFC_FORUM_TYPE2 -> {
        val nfcA = NfcA.get(tag)
        nfcA.connect()

        val startPage = 4
        val maxPage = 36

        var payload = byteArrayOf()
        for (page in startPage..maxPage step 4) {
          val cmd = byteArrayOf(0x30.toByte(), page.toByte())
          try {
            val data = nfcA.transceive(cmd)
            //Log.d(TAG, "Read page $page: $data")
            payload += data
          } catch (e: Exception) {
            Log.w(TAG, "Error reading page $page", e)
          }
        }

        nfcA.close()
        if (payload.isNotEmpty()) {
          Log.d(TAG, "Sending payload to callback: $payload")

          mainLooper?.post {
            callbackChannel?.onRead(
              payloadArg = TagPayload(
                payload = payload,
                format = TagFormat.NFC_FORUM_TYPE2,
              )
            ) {}
          }
        }
      }
      TagFormat.UNKNOWN -> {
        Log.w(TAG, "Tag format: Unknown")
      }
    }

    return@ReaderCallback


//    val ndef = Ndef.get(tag)
//    if (ndef == null) {
//      Log.w(TAG, "Ndef is null")
//      return@ReaderCallback
//    }
//
//    val ndefMessage = ndef.ndefMessage.toByteArray()
//    if (ndefMessage == null) {
//      Log.w(TAG, "NdefMessage is null")
//      return@ReaderCallback
//    }
//
//    mainLooper?.post {
//      callbackChannel?.onRead(
//        payloadArg = TagPayload(
//          payload = ndefMessage,
//          format = tagFormat,
//        )
//      ) {}
//    }
  }
}


