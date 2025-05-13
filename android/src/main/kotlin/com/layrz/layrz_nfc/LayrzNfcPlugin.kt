@file:Suppress("DEPRECATION", "SpellCheckingInspection", "MissingPermission", "KotlinConstantConditions")
package com.layrz.layrz_nfc

import android.app.Activity
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.nfc.NfcAdapter
import android.nfc.Tag
import android.nfc.tech.Ndef
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.embedding.engine.plugins.activity.ActivityAware
import io.flutter.embedding.engine.plugins.activity.ActivityPluginBinding
import io.flutter.plugin.common.PluginRegistry
import android.os.Handler
import android.os.Looper
import android.widget.Toast
import io.flutter.Log
import java.util.concurrent.atomic.AtomicBoolean


/** LayrzNfcPlugin */
class LayrzNfcPlugin: LayrzNfcPlatformChannel, FlutterPlugin, ActivityAware, PluginRegistry.NewIntentListener {

  private var activity: Activity? = null
  private var mainLooper: Handler? = null
  private lateinit var context: Context

  companion object {
    private const val TAG = "LayrzNfcPlugin/Android"
  }

  private var nfcAdapter: NfcAdapter? = null
  private var pendingIntent: PendingIntent? = null
  private var intentFiltersArray: Array<IntentFilter>? = null

  override fun onAttachedToEngine(binding: FlutterPlugin.FlutterPluginBinding) {
    LayrzNfcPlatformChannel.setUp(binding.binaryMessenger, this)
    context = binding.applicationContext
    mainLooper = Handler(Looper.getMainLooper())
  }

  override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {
    mainLooper = null
  }
  override fun onNewIntent(intent: Intent): Boolean {
    if (NfcAdapter.ACTION_NDEF_DISCOVERED == intent.action ||
      NfcAdapter.ACTION_TECH_DISCOVERED == intent.action ||
      NfcAdapter.ACTION_TAG_DISCOVERED == intent.action
    ) {
      Log.d(TAG, "Receive Nfc Intent: ${intent.action}")

      // Get the NFC Tag object from the Intent
      val tag: Tag? = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG)
      if (tag != null) {
        // Log basic information about the tag
        Log.d(TAG, "NFC Tag detected: $tag")

        // Log the list of supported technologies
        val techList = tag.techList
        Log.d(TAG, "Supported technologies: ${techList.joinToString(", ")}")

        // Example: Log the ID of the tag (if available)
        val tagId = tag.id
        if (tagId != null) {
          val tagIdHex = tagId.joinToString("") { String.format("%02X", it) }
          Log.d(TAG, "Tag ID (hex): $tagIdHex")
        }

        // If the tag supports NDEF, log the NDEF message
        val ndef = Ndef.get(tag)
        if (ndef != null) {
          ndef.connect()
          val ndefMessage = ndef.ndefMessage
          if (ndefMessage != null) {
            for (record in ndefMessage.records) {
              val payload = String(record.payload)
              Log.d(TAG, "NDEF Record Payload: $payload")
            }
          } else {
            Log.d(TAG, "No NDEF message found on the tag")
          }
          ndef.close()
        } else {
          Log.d(TAG, "Tag does not support NDEF")
        }
      } else {
        Log.e(TAG, "No NFC Tag found in the Intent")
      }

      return true
    }
    return false
  }
  override fun onAttachedToActivity(binding: ActivityPluginBinding) {
    activity = binding.activity
    binding.addOnNewIntentListener(this)
    nfcAdapter = NfcAdapter.getDefaultAdapter(context)
    if (nfcAdapter != null) {
      pendingIntent = PendingIntent.getActivity(
        activity,
        0,
        Intent(activity, activity!!::class.java).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP),
        PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
      )
      // Define intent filters to handle different NFC actions
      val ndef = IntentFilter(NfcAdapter.ACTION_NDEF_DISCOVERED).apply {
        try {
          addDataType("*/*") // Handle any MIME type
        } catch (e: IntentFilter.MalformedMimeTypeException) {
          e.printStackTrace()
        }
      }

      val tag = IntentFilter(NfcAdapter.ACTION_TAG_DISCOVERED)
      val tech = IntentFilter(NfcAdapter.ACTION_TECH_DISCOVERED)

// Combine all filters into a single array
      intentFiltersArray = arrayOf(ndef, tag, tech)
    } else {
      Log.w(TAG, "NFC is not supported in this device")
    }
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

  override fun bindScanners(callback: (Result<Unit>) -> Unit) {
    callback(Result.success(bindScanners()))
  }

  override fun checkCapabilities(callback: (Result<Boolean>) -> Unit) {
    if (nfcAdapter != null) {
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
    Log.w(TAG, "Start Reading ...")
    if (nfcAdapter != null) {
      nfcAdapter?.enableForegroundDispatch(activity, pendingIntent, intentFiltersArray, null);
      callback(Result.success(true))
      return
    }
    callback(Result.success(false))
    return
  }

  override fun stopReading(callback: (Result<Boolean>) -> Unit) {
    Log.w(TAG, "Stop Reading ...")
    if (nfcAdapter != null && activity != null) {
      try {
        nfcAdapter?.disableForegroundDispatch(activity)
        Log.w(TAG, "NFC deshabilitado correctamente")
        callback(Result.success(true))
      } catch (e: Exception) {
        Log.e(TAG, "Error al deshabilitar NFC: ${e.message}")
        callback(Result.success(false))
      }
    }
    callback(Result.success(false))
  }

  private fun canNfc(): Boolean {
    if (nfcAdapter == null) {
      nfcAdapter = NfcAdapter.getDefaultAdapter(context)
    }
    if (nfcAdapter?.isEnabled != true) {
      Log.w(TAG, "NFC unsuported")

      return false
    }
    return true
  }

  fun bindScanners() {
    Log.w(TAG, "BindScanners is not implemented in Android")
  }


}


