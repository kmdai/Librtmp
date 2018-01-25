package com.kmdai.rtmppush;

/**
 * Created by kmdai on 18-1-25.
 */

public class LibrtmpManager {


    public native boolean rtmpInit();

    public native boolean rtmpFree();

    public native void setUrl(String url);

    public native void sendChunk(byte[] chunk);
}
