package com.kmdai.srslibrtmp;

public class SRSLibrtmpManager {
    static {
        System.loadLibrary("srsPush");
    }

    native void init(String url);
}
