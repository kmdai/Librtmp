package com.codyy.librtmp;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.support.annotation.NonNull;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;

import com.kmdai.srslibrtmp.SRSLibrtmpManager;

import java.io.IOException;
import java.nio.ByteBuffer;


public class AvcEncoder {

    private MediaCodec mMediaCodec;
    int m_width;
    int m_height;
    byte[] m_info = null;
    private Surface mSurface;
    private double mFrameRate;
    private int mBitrate;
    ByteBuffer mByteBuffer;
    private final int TIMEOUT_US = 10000;
    boolean mIsStop;
    //    private LibrtmpManager mLibrtmpManager;
    //                RTMPMuxer mRTMPMuxer;
    long indexTime = 0;
    private SRSLibrtmpManager mSRSLibrtmpManager;

    public AvcEncoder(int width, int height, double framerate, int bitrate) {
        mIsStop = false;
        m_width = width;
        m_height = height;
        mFrameRate = framerate;
        mBitrate = bitrate;
        mSRSLibrtmpManager = new SRSLibrtmpManager();
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
            Log.e("AvcEncoder---", mediaCodecInfo.getName() + ":" + str);
        }
        if (TextUtils.isEmpty(name)) {
            Log.e("-------", "name is null");
            return;
        }
        mByteBuffer = ByteBuffer.allocate(8);

        try {
            mMediaCodec = MediaCodec.createByCodecName(name);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mMediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mSurface = mMediaCodec.createInputSurface();
        mMediaCodec.start();
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
            if (!mSRSLibrtmpManager.setUrl("rtmp://10.5.225.38:1935/srs/kmdai")) {
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
            for (; ; ) {
                if (mIsStop) {
                    break;
                }
                int outputBufferId = mMediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_US);
                if (outputBufferId >= 0) {
                    ByteBuffer outputBuffer = mMediaCodec.getOutputBuffer(outputBufferId);
                    byte[] outData = new byte[bufferInfo.size];
                    outputBuffer.get(outData, 0, bufferInfo.size);
                    calcTotalTime(bufferInfo.presentationTimeUs / 1000);
//                    Log.d("----","offset--"+bufferInfo.offset);
                    if (bufferInfo.flags == MediaCodec.BUFFER_FLAG_CODEC_CONFIG) {
                        mSRSLibrtmpManager.addFrame(outData, outData.length, bufferInfo.flags, 0);
                    } else {
                        mSRSLibrtmpManager.addFrame(outData, outData.length, bufferInfo.flags, getTimeIndex());
                    }

//                    try {
//                        mRtmpClient.write(outData);
//                    } catch (IOException e) {
//                        e.printStackTrace();
//                    }
//                    Message message = mHandler.obtainMessage();
//                    message.obj = new Frame(outData, bufferInfo.size, bufferInfo.flags, bufferInfo.flags == MediaCodec.BUFFER_FLAG_KEY_FRAME);
//                    mHandler.sendMessage(message);
                }
                if (outputBufferId >= 0 && bufferInfo.flags == MediaCodec.BUFFER_FLAG_CODEC_CONFIG) {
                }
                if (outputBufferId >= 0) {

                    mMediaCodec.releaseOutputBuffer(outputBufferId, false);
                }
            }
            try {
                mMediaCodec.stop();
                mMediaCodec.release();
                Log.d("---", ":release");
                mSRSLibrtmpManager.release();
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
        int delta = (int) (currentTimeUs - lastTimeUs);
//        this.lastTimeUs = currentTimeUs;
        timeIndex = delta;
    }

    public void reset() {
        lastTimeUs = 0;
        timeIndex = 0;
    }

    public int getTimeIndex() {
        return timeIndex;
    }
}


