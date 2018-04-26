package com.kmdai.srslibrtmp;

import android.os.AsyncTask;

public class SRSLibrtmpManager {
    static {
        System.loadLibrary("srsPush");
    }

    /**
     * 设置url
     *
     * @param url
     */
    public native boolean setUrl(String url);

    public native void addFrame(byte[] data, int size, int type, int time);

    public native void release();
}
