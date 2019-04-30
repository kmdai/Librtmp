package com.kmdai.srslibrtmp;

public class SRSLibrtmpManager {
    public final static int NODE_TYPE_AUDIO = 1;
    public final static int NODE_TYPE_VIDEO = 2;

    static {
        System.loadLibrary("ndkmedia");
    }

    private SRSLibrtmpManager() {

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

        public SRSLibrtmpManager build() {
            SRSLibrtmpManager srsLibrtmpManager = new SRSLibrtmpManager();
            srsLibrtmpManager.setAudioBitrate(aBitrate);
            srsLibrtmpManager.setAudioSampleRate(audioRate);
            srsLibrtmpManager.setFrameRate(frameRate);
            srsLibrtmpManager.setChannelCount(channel);
            srsLibrtmpManager.setVideoBitRate(vBitrate);
            srsLibrtmpManager.setWidth(width);
            srsLibrtmpManager.setHeight(height);
            return srsLibrtmpManager;
        }
    }


    public native void init();

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

    public native void setFrameRate(int frameRate);

    public native void setVideoBitRate(int videodatarate);

    public native void setWidth(int width);

    public native void setHeight(int height);

    public native void setChannelCount(int channelCount);

    public native void setAudioBitrate(int audiodatarate);

    public native void setAudioSampleRate(int audiosamplerate);

    public native void setAudiosamplesize(int audiosamplesize);

    public native Object getSurface();

    public native void openAudioRecord();

    public native void startScreenRecord();
}
