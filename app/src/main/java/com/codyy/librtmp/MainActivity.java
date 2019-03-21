package com.codyy.librtmp;


import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.AppCompatEditText;
import android.support.v7.widget.AppCompatSpinner;
import android.util.DisplayMetrics;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Toast;

import org.json.JSONArray;
import org.json.JSONException;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class MainActivity extends Activity {
    public final static int PERMISSION_CODE = 0x0a1;

    MediaEncoder avcCodec;
    private int mScreenDensity;
    private int mDisplayWidth;
    private int mDisplayHeight;

    int width = mDisplayWidth = 720;
    int height = mDisplayHeight = 1280;
    int framerate = 25;
    int bitrate = 1600000;
    private MediaProjectionManager mProjectionManager;
    private MediaProjection mMediaProjection;
    private VirtualDisplay mVirtualDisplay;
    private final static int RECORD_AUDIO = 0x006;
    private AppCompatEditText mEdittext;
    private AppCompatSpinner mSpinner;
    private JSONArray mJsonArray;
    private String mUrl = "";
    SharedPreferences.Editor editor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mEdittext = findViewById(R.id.edittext);
        mSpinner = findViewById(R.id.spinner);
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        mScreenDensity = metrics.densityDpi;
        String[] permission = {Manifest.permission.RECORD_AUDIO};
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                requestPermissions(permission, RECORD_AUDIO);
            }
        }
        SharedPreferences sharedPreferences = getSharedPreferences("url", MODE_PRIVATE);
        editor = sharedPreferences.edit();
        try {
            mJsonArray = new JSONArray(sharedPreferences.getString("url", "[]"));
            String[] list = new String[mJsonArray.length()];
            for (int i = 0; i < mJsonArray.length(); i++) {
                mUrl = list[i] = mJsonArray.optString(i);
            }
            ArrayAdapter<String> arrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_multiple_choice, list);
            mSpinner.setAdapter(arrayAdapter);
            mEdittext.setText(mUrl);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (grantResults.length > 0) {
            for (int grant : grantResults) {
                if (grant != PackageManager.PERMISSION_GRANTED) {
                    finish();
                    return;
                }
            }
        } else {
            finish();
        }
    }

    private void shareScreen() {
        if (mMediaProjection == null) {
            mProjectionManager =
                    (MediaProjectionManager) getSystemService(Context.MEDIA_PROJECTION_SERVICE);
            avcCodec = new MediaEncoder(width, height, framerate, bitrate);
            startActivityForResult(mProjectionManager.createScreenCaptureIntent(),
                    PERMISSION_CODE);
            return;
        }
        mVirtualDisplay = createVirtualDisplay();
    }

    private VirtualDisplay createVirtualDisplay() {
        return mMediaProjection.createVirtualDisplay("ScreenSharingDemo",
                mDisplayWidth, mDisplayHeight, mScreenDensity,
                DisplayManager.VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR,
                avcCodec.getSurface(), null /*Callbacks*/, null /*Handler*/);
    }

    public void start(View view) {
        shareScreen();
    }

    public void stop(View view) {
        stopScreenSharing();
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode != PERMISSION_CODE) {
            return;
        }
        if (resultCode != RESULT_OK) {
            Toast.makeText(this,
                    "User denied screen sharing permission", Toast.LENGTH_SHORT).show();
            return;
        }
        mMediaProjection = mProjectionManager.getMediaProjection(resultCode, data);
        mMediaProjection.registerCallback(new MediaProjectionCallback(), null);
        mVirtualDisplay = createVirtualDisplay();
        avcCodec.start(mEdittext.getText().toString());
        if (!mUrl.equals(mEdittext.getText().toString())) {
            mJsonArray.put(mEdittext.getText().toString());
        }
    }

    private class MediaProjectionCallback extends MediaProjection.Callback {
        @Override
        public void onStop() {
            stopScreenSharing();
        }
    }

    private void stopScreenSharing() {
        if (mVirtualDisplay != null) {
            mVirtualDisplay.release();
            mMediaProjection.stop();
            mMediaProjection = null;
            mVirtualDisplay = null;
            avcCodec.close();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        editor.putString("url", mJsonArray.toString());
        editor.commit();
    }
}
