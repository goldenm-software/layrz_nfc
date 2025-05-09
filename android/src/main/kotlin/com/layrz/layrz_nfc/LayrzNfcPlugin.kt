@file:Suppress("DEPRECATION", "SpellCheckingInspection", "MissingPermission", "KotlinConstantConditions")
package com.layrz.layrz_nfc

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.nfc.NfcAdapter
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.embedding.engine.plugins.activity.ActivityAware
import io.flutter.embedding.engine.plugins.activity.ActivityPluginBinding
import io.flutter.plugin.common.PluginRegistry
import android.os.Handler
import android.os.Looper
import io.flutter.Log


/** LayrzNfcPlugin */
class LayrzNfcPlugin: LayrzNfcPlatformChannel, FlutterPlugin, ActivityAware, PluginRegistry.ActivityResultListener {
  private var activity: Activity? = null
  private var mainLooper: Handler? = null
  private lateinit var context: Context
  companion object {
    private const val TAG = "LayrzNfcPlugin/Android"
  }
  private var nfcAdapter: NfcAdapter? = null


  override fun onAttachedToEngine(binding: FlutterPlugin.FlutterPluginBinding) {
    LayrzNfcPlatformChannel.setUp(binding.binaryMessenger, this)
    context = binding.applicationContext
    mainLooper = Handler(Looper.getMainLooper())
  }

  override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {
    mainLooper = null
  }

  override fun onAttachedToActivity(binding: ActivityPluginBinding) {
    activity = binding.activity
    binding.addActivityResultListener(this)
  }

  override fun onDetachedFromActivityForConfigChanges() {
  }

  override fun onReattachedToActivityForConfigChanges(binding: ActivityPluginBinding) {
  }

  override fun onDetachedFromActivity() {
    activity = null
  }

  override fun checkCapabilities(callback: (Result<Boolean>) -> Unit) {
    if (nfcAdapter != null) {
      callback(Result.success(true))
      return
    }
    nfcAdapter = NfcAdapter.getDefaultAdapter(context)
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
  override fun read(callback: (Result<Boolean>) -> Unit) {
    Log.w(TAG, "Call Read")
    callback(Result.success(true))
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

  override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?): Boolean {
    return true
  }

}

