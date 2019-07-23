package com.kmdai.srslibrtmp;



/**
 * @author by kmdai 2019-07-22
 */
public class SrsNativeMedia {
    public void config() {

    }

    class Build {
        int sampleRate;
        int channel;
        int bitrate;

        public Build setSampleRate(int sampleRate) {
            this.sampleRate = sampleRate;
            return this;
        }

        public Build setChannel(int channel) {
            this.channel = channel;
            return this;
        }

        public Build setBitrate(int bitrate) {
            this.bitrate = bitrate;
            return this;
        }
    }

    private native void ndk_config( int samplerate, int channel, int bitrate);
}
