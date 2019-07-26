package com.kmdai.rtmppush;

import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.util.Log;

/**
 * Created by kmdai on 18-1-25.
 */

public class LibrtmpManager {
    public final static int NODE_TYPE_AUDIO = 1;
    public final static int NODE_TYPE_VIDEO = 2;

    {
        System.loadLibrary("kpush");
    }

    public static class Builder {
        int width;
        int height;
        int frameRate;
        int channel;
        int vBitrate;
        int aBitrate;
        int sampleRate;
        int audioRate;

        public Builder setWidth(int width) {
            this.width = width;
            return this;
        }


        public Builder setAudioRate(int audioRate) {
            this.audioRate = audioRate;
            return this;
        }

        public Builder setHeight(int height) {
            this.height = height;
            return this;
        }

        public Builder setFrameRate(int frameRate) {
            this.frameRate = frameRate;
            return this;
        }

        public Builder setChannel(int channel) {
            this.channel = channel;
            return this;
        }

        public Builder setvBitrate(int vBitrate) {
            this.vBitrate = vBitrate;
            return this;
        }

        public Builder setaBitrate(int aBitrate) {
            this.aBitrate = aBitrate;
            return this;
        }

        public Builder setSampleRate(int sampleRate) {
            this.sampleRate = sampleRate;
            return this;
        }

        public LibrtmpManager build() {
            LibrtmpManager librtmpManager = new LibrtmpManager();
            librtmpManager.setAudioBitrate(aBitrate);
            librtmpManager.setAudioSampleRate(audioRate);
            librtmpManager.setFrameRate(frameRate);
            librtmpManager.setAudioSampleRate(sampleRate);
            librtmpManager.setChannelCount(channel);
            librtmpManager.setVideoBitRate(vBitrate);
            librtmpManager.setWidth(width);
            librtmpManager.setHeight(height);
            String name = findCodecName();
            if (TextUtils.isEmpty(name)) {
                return null;
            }
            Log.d("-------", name);
            librtmpManager.init(name);
            return librtmpManager;
        }

        private String findCodecName() {
            MediaFormat mediaFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, channel);
            MediaCodecList mediaCodecList = new MediaCodecList(MediaCodecList.REGULAR_CODECS);
            String name = mediaCodecList.findEncoderForFormat(mediaFormat);
            return name;
        }
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
     * @param type {@link LibrtmpManager#NODE_TYPE_AUDIO} ,{@link LibrtmpManager#NODE_TYPE_AUDIO}
     * @param flag
     * @param time
     */
    public native void addFrame(byte[] data, int size, int type, int flag, int time);

    public native void release();

    private native void init(String audioCodecName);


    private native void setFrameRate(int frameRate);

    private native void setVideoBitRate(int videodatarate);

    private native void setWidth(int width);

    private native void setHeight(int height);

    private native void setChannelCount(int channelCount);

    private native void setAudioBitrate(int audiodatarate);

    private native void setAudioSampleRate(int audiosamplerate);


    private native Object getSurface();

}
