package com.codyy.librtmp;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.media.MediaRecorder;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;
import android.widget.Toast;

import com.kmdai.rtmppush.LibrtmpManager;
import com.kmdai.srslibrtmp.SRSLibrtmpManager;

import java.io.IOException;
import java.nio.ByteBuffer;


public class MediaEncoder {

    private MediaCodec mVideoMediaCodec;
    private MediaCodec mAudioCodec;
    private AudioRecord mAudioRecord;
    int m_width;
    int m_height;
    byte[] m_info = null;
    private Surface mSurface;
    private double mFrameRate;
    private int mBitrate;
    private final int TIMEOUT_US = 10000;
    private final int AUDIO_SAMPLE_RATE = 44100;
    private final int AUDIO_CHANNEL_COUNT = 2;
    boolean mIsStop;
    private LibrtmpManager mLibrtmpManager;
    //    private String mRtmpUrl = "rtmp://172.96.16.188:1935/srs/kmdai";
    private String mRtmpUrl = "rtmp://10.5.225.38:1935/mobile/kmdai";
    //                RTMPMuxer mRTMPMuxer;
    long indexTime = 0;
    private SRSLibrtmpManager mSRSLibrtmpManager;

    private int mMiniAudioBufferSize;

    public MediaEncoder(int width, int height, double framerate, int bitrate) {
        mIsStop = false;
        m_width = width;
        m_height = height;
        mFrameRate = framerate;
        mBitrate = bitrate;
        mSRSLibrtmpManager = new SRSLibrtmpManager();
        mLibrtmpManager = new LibrtmpManager();
        reset();
        MediaFormat mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
        mediaFormat.setFloat(MediaFormat.KEY_FRAME_RATE, (float) framerate);
        mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AVCProfileMain);
        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);
        MediaCodecList mediaCodecList = new MediaCodecList(MediaCodecList.REGULAR_CODECS);
        MediaCodecInfo[] mediaCodecInfos = mediaCodecList.getCodecInfos();
        String name = mediaCodecList.findEncoderForFormat(mediaFormat);
        for (MediaCodecInfo mediaCodecInfo : mediaCodecInfos) {
            String[] strings = mediaCodecInfo.getSupportedTypes();
            String str = "";
            for (String type : strings) {
                str += type;
            }
            Log.e("MediaEncoder---", mediaCodecInfo.getName() + ":" + str);
        }
        if (TextUtils.isEmpty(name)) {
            Log.e("-------", "name is null");
            return;
        }
        try {
            mVideoMediaCodec = MediaCodec.createByCodecName(name);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mVideoMediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mSurface = mVideoMediaCodec.createInputSurface();
        mVideoMediaCodec.start();
    }

    /**
     *
     */
    private void audioInit() {
        MediaFormat mediaFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, AUDIO_SAMPLE_RATE, AUDIO_CHANNEL_COUNT);
        MediaCodecList mediaCodecList = new MediaCodecList(MediaCodecList.REGULAR_CODECS);
        MediaCodecInfo[] mediaCodecInfos = mediaCodecList.getCodecInfos();
        String name = mediaCodecList.findEncoderForFormat(mediaFormat);
        try {
            mAudioCodec = MediaCodec.createByCodecName(name);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mAudioCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        //最小的缓冲区
        mMiniAudioBufferSize = AudioRecord.getMinBufferSize(AUDIO_SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT);
        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                AUDIO_SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT,
                mMiniAudioBufferSize);
    }

    public Surface getSurface() {
        return mSurface;
    }


    public void close() {
        mIsStop = true;
    }

    public void start() {
        new Thread(new SendRunable()).start();
    }

    public class SendRunable implements Runnable {

        @Override
        public void run() {
//            mLibrtmpManager.rtmpInit();
//            mLibrtmpManager.setUrl(mRtmpUrl);
            if (!mSRSLibrtmpManager.setUrl(mRtmpUrl)) {
                return;
            }
            mSRSLibrtmpManager.setFrameRate(mFrameRate);
            mSRSLibrtmpManager.setHeight(m_height);
            mSRSLibrtmpManager.setWidth(m_width);
            mSRSLibrtmpManager.setVideodatarate(mBitrate);
            mSRSLibrtmpManager.setAudiodatarate(29);
            mSRSLibrtmpManager.setAudiosamplerate(44100);
            mSRSLibrtmpManager.setAudiosamplesize(16);
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            int time = 0;
            for (; ; ) {
                if (mIsStop) {
                    break;
                }
                int outputBufferId = mVideoMediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_US);
                if (outputBufferId >= 0) {
                    ByteBuffer outputBuffer = mVideoMediaCodec.getOutputBuffer(outputBufferId);
                    byte[] outData = new byte[bufferInfo.size];
                    outputBuffer.get(outData, 0, bufferInfo.size);
                    calcTotalTime(bufferInfo.presentationTimeUs / 1000);
//                    Log.d("----","offset--"+bufferInfo.offset);
                    if (bufferInfo.flags == MediaCodec.BUFFER_FLAG_CODEC_CONFIG) {
                        mSRSLibrtmpManager.addFrame(outData, outData.length, bufferInfo.flags, 0);
//                        mLibrtmpManager.setSpsPps(outData, outData.length);
                    } else {
//                        Log.d("----", "getTimeIndex()--" + getTimeIndex());
//                        mLibrtmpManager.sendChunk(outData, outData.length, bufferInfo.flags, getTimeIndex());
//                        time+=30;
                        mSRSLibrtmpManager.addFrame(outData, outData.length, bufferInfo.flags, getTimeIndex());
                    }
                }
                if (outputBufferId >= 0) {
                    mVideoMediaCodec.releaseOutputBuffer(outputBufferId, false);
                }
            }
            try {
                mVideoMediaCodec.stop();
                mVideoMediaCodec.release();
                Log.d("---", ":release");
                mSRSLibrtmpManager.release();
                reset();
//                mLibrtmpManager.rtmpFree();
//                mRTMPMuxer.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


    private long lastTimeUs;
    private int timeIndex;

    public void calcTotalTime(long currentTimeUs) {
        if (lastTimeUs <= 0) {
            this.lastTimeUs = currentTimeUs;
        }
        timeIndex = (int) (currentTimeUs - lastTimeUs);
    }

    public void reset() {
        lastTimeUs = 0;
        timeIndex = 0;
    }

    public int getTimeIndex() {
        return timeIndex;
    }
}


