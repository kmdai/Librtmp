package com.codyy.librtmp;


import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.media.projection.MediaProjection;
import android.media.projection.MediaProjectionManager;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.View;
import android.widget.Toast;

import java.io.BufferedReader;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class MainActivity extends Activity {
    public final static int PERMISSION_CODE = 0x0a1;
    DatagramSocket socket;
    InetAddress address;

    AvcEncoder avcCodec;
    private int mScreenDensity;
    private int mDisplayWidth;
    private int mDisplayHeight;

    int width = mDisplayWidth = 960;
    int height = mDisplayHeight = 540;
    int framerate = 30;
    int bitrate = 60 * 1000 * 1000;
    private MediaProjectionManager mProjectionManager;
    private MediaProjection mMediaProjection;
    private VirtualDisplay mVirtualDisplay;
    byte[] h264 = new byte[width * height * 3 / 2];

    @SuppressLint("NewApi")
    @Override
    protected void onCreate(Bundle savedInstanceState) {


        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        avcCodec = new AvcEncoder(width, height, framerate, bitrate);
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        mScreenDensity = metrics.densityDpi;
        BufferedReader bufferedReader;
        mProjectionManager =
                (MediaProjectionManager) getSystemService(Context.MEDIA_PROJECTION_SERVICE);
        try {
            socket = new DatagramSocket();
            address = InetAddress.getByName("192.168.3.126");
        } catch (SocketException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        avcCodec.setCallback(new AvcEncoder.Callback() {
            @Override
            public void onOutputBufferAvailable(byte[] data) {
//                DatagramPacket packet = new DatagramPacket(data, data.length, mInetAddress, 5000);
//                try {
////                    mSocket.send(packet);
//                } catch (IOException e) {
//                    e.printStackTrace();
//                }
            }
        });
    }


    private void shareScreen() {
        if (mMediaProjection == null) {
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
        avcCodec.start();
    }

    private class MediaProjectionCallback extends MediaProjection.Callback {
        @Override
        public void onStop() {
            mMediaProjection = null;
            stopScreenSharing();
        }
    }

    private void stopScreenSharing() {
        if (mVirtualDisplay != null) {
            mVirtualDisplay.release();
            mMediaProjection.stop();
            mVirtualDisplay = null;
            avcCodec.close();
        }
    }

}
