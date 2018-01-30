package com.kmdai.rtmppush;

/**
 * Created by kmdai on 18-1-25.
 */

public class LibrtmpManager {

    {
        System.loadLibrary("push");
    }

    public native boolean rtmpInit();

    public native boolean rtmpFree();

    /**
     * 设置URL
     *
     * @param url
     */
    public native void setUrl(String url);

    /**
     * 发送编码数据
     *
     * @param chunk
     */
    public native void sendChunk(byte[] chunk);

    /**
     * 设置sps、pps数据
     *
     * @param data
     */
    public native void setSpsPps(byte[] data, int size);
}
