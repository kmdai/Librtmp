package com.kmdai.srslibrtmp;

public class SRSLibrtmpManager {
    public final static int NODE_TYPE_AUDIO = 1;
    public final static int NODE_TYPE_VIDEO = 2;

    static {
        System.loadLibrary("srsPush");
    }

    /**
     * 设置url
     *
     * @param url
     */
    public native boolean setUrl(String url);

    /**
     * @param data
     * @param size
     * @param type {@link SRSLibrtmpManager#NODE_TYPE_AUDIO} ,{@link SRSLibrtmpManager#NODE_TYPE_AUDIO}
     * @param flag
     * @param time
     */
    public native void addFrame(byte[] data, int size, int type, int flag, int time);

    public native void release();

    public native void setFrameRate(double frameRate);

    public native void setVideodatarate(double videodatarate);

    public native void setWidth(double width);

    public native void setHeight(double height);

    public native void setChannelCount(int channelCount);

    public native void setAudiodatarate(double audiodatarate);

    public native void setAudiosamplerate(double audiosamplerate);

    public native void setAudiosamplesize(double audiosamplesize);

    public native Object getSurface();
}
