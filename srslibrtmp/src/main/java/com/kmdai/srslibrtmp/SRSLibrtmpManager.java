package com.kmdai.srslibrtmp;

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

    public native void setFrameRate(double frameRate);

    public native void setVideodatarate(double videodatarate);

    public native void setWidth(double width);

    public native void setHeight(double height);

    public native void setAudiodatarate(double audiodatarate);

    public native void setAudiosamplerate(double audiosamplerate);

    public native void setAudiosamplesize(double audiosamplesize);

    public native Object getSurface();
}
