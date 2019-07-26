package com.codyy.librtmp;

import android.graphics.Bitmap;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.media.MediaRecorder;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import android.view.Surface;

import com.codyy.librtmp.glec.EGLRender;
import com.kmdai.rtmppush.LibrtmpManager;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

import static android.media.MediaCodec.BUFFER_FLAG_CODEC_CONFIG;


public class MediaEncoder implements android.os.Handler.Callback, EGLRender.onFrameCallBack {
    public final static int START_PUSH = 0x04;
    private MediaCodec mVideoMediaCodec;
    private MediaCodec mAudioCodec;
    private AudioRecord mAudioRecord;
    private Queue<PCM> mPCMS;
    private ReentrantLock mLock;
    private Condition mCondition;
    int m_width;
    int m_height;
    byte[] m_info = null;
    private Surface mSurface;
    private double mFrameRate;
    private int mBitrate;
    private final int TIMEOUT_US = 1000;

    private final int AUDIO_SAMPLE_RATE = 44100;
    private final int AUDIO_BIT_RATE = 96000;
    private final int AUDIO_CHANNEL_COUNT = 1;
    private Handler mHandler;
    AtomicBoolean mIsStop = new AtomicBoolean(false);
    //            private LibrtmpManager mLibrtmpManager;
    private String mRtmpUrl = "rtmp://10.23.164.30:1935/srs/kmdai";
    //                RTMPMuxer mRTMPMuxer;
    long indexTime = 0;
//    private SRSLibrtmpManager mSRSLibrtmpManager;
                private LibrtmpManager mSRSLibrtmpManager;
    private int mMiniAudioBufferSize;
    EGLRender mEGLRender;

    public MediaEncoder(int width, int height, int framerate, int bitrate) {
        if (mIsStop.get()) {
            return;
        }
        m_width = width;
        m_height = height;
        mFrameRate = framerate;
        mBitrate = bitrate;
        mLock = new ReentrantLock();
        mCondition = mLock.newCondition();
        mHandler = new Handler(this);
        mPCMS = new LinkedList<>();
        mSRSLibrtmpManager = new LibrtmpManager.Builder()
                .setaBitrate(AUDIO_BIT_RATE)
                .setSampleRate(AUDIO_SAMPLE_RATE)
                .setChannel(AUDIO_CHANNEL_COUNT)
                .setFrameRate(framerate)
                .setvBitrate(bitrate)
                .setWidth(width)
                .setHeight(height)
                .build();
        reset();
        MediaFormat mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
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
//        mediaFormat.setInteger(MediaFormat.KEY_PROFILE, MediaCodecInfo.CodecProfileLevel.AVCProfileMain);
        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        mediaFormat.setFloat(MediaFormat.KEY_FRAME_RATE, framerate);
//        mediaFormat.setInteger(MediaFormat.KEY_REPEAT_PREVIOUS_FRAME_AFTER, 1000000 / framerate);
        mediaFormat.setInteger(MediaFormat.KEY_BITRATE_MODE, MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CBR);
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 0);

        try {
            mVideoMediaCodec = MediaCodec.createByCodecName(name);
//            mVideoMediaCodec=MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
            MediaCodecInfo.CodecProfileLevel[] profileLevels = mVideoMediaCodec.getCodecInfo().getCapabilitiesForType(MediaFormat.MIMETYPE_VIDEO_AVC).profileLevels;
            if (profileLevels.length > 0) {
                for (MediaCodecInfo.CodecProfileLevel profileLevel : profileLevels) {
                    profileLevel.profile = MediaCodecInfo.CodecProfileLevel.AVCProfileHigh;
                }

            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        mVideoMediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mSurface = mVideoMediaCodec.createInputSurface();
        mEGLRender = new EGLRender(mSurface, m_width, m_height, (int) mFrameRate);
        mEGLRender.setCallBack(this);
//        audioInit();
        mVideoMediaCodec.start();
//        mSRSLibrtmpManager.openAudioRecord();
//        mAudioRecord.startRecording();
//        mAudioCodec.start();
    }

    /**
     *
     */
    private void audioInit() {
        //最小的缓冲区
        mMiniAudioBufferSize = AudioRecord.getMinBufferSize(AUDIO_SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                AUDIO_SAMPLE_RATE,
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT,
                mMiniAudioBufferSize);
        MediaFormat mediaFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, AUDIO_SAMPLE_RATE, AUDIO_CHANNEL_COUNT);
        MediaCodecList mediaCodecList = new MediaCodecList(MediaCodecList.REGULAR_CODECS);
        MediaCodecInfo[] mediaCodecInfos = mediaCodecList.getCodecInfos();
        String name = mediaCodecList.findEncoderForFormat(mediaFormat);
        for (MediaCodecInfo mediaCodecInfo : mediaCodecInfos) {
            String[] strings = mediaCodecInfo.getSupportedTypes();
            String str = "";
            for (String type : strings) {
                str += type;
            }
            Log.e("mAudioCodec---", mediaCodecInfo.getName() + ":" + str);
        }
        try {
            Log.e("mAudioCodec---", "name---" + name);
            mAudioCodec = MediaCodec.createByCodecName(name);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        mediaFormat.setInteger(MediaFormat.KEY_CHANNEL_COUNT, AUDIO_CHANNEL_COUNT);
        mediaFormat.setInteger(MediaFormat.KEY_SAMPLE_RATE, AUDIO_SAMPLE_RATE);
//        mediaFormat.setInteger(MediaFormat.KEY_PCM_ENCODING, AudioFormat.ENCODING_PCM_16BIT);
        mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, mMiniAudioBufferSize);
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, AUDIO_BIT_RATE);

        mAudioCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);


    }

    boolean isStart = false;


    @Override
    public boolean handleMessage(Message msg) {
        if (msg.what == START_PUSH) {
            isStart = true;
//            new Thread(new RecordRunnable()).start();
//            new Thread(new RecordEncodec()).start();
//            new Thread(new RecordDecodec()).start();
            return true;
        }
        return false;
    }

    @Override
    public void onUpdate() {
    }

    @Override
    public void onCutScreen(Bitmap bitmap) {

    }

    public class RecordRunnable implements Runnable {
        public RecordRunnable() {

        }

        @Override
        public void run() {
            PCM.timeReset();
            for (; ; ) {
                if (mIsStop.get()) {
                    return;
                }

                byte[] data = new byte[mMiniAudioBufferSize];
                int length = mAudioRecord.read(data, 0, data.length);
                if (length == mMiniAudioBufferSize) {
                    PCM pcm = new PCM();
                    pcm.data = data;
                    pcm.time = pcm.currentTime();
//                    Log.d("----", "pcm-time" + pcm.time);
                    addPCM(pcm);
                } else {
                    switch (length) {
                        case AudioRecord.ERROR_BAD_VALUE:
                            Log.e("mAudioRecord.read---", "ERROR_BAD_VALUE");
                            break;
                        case AudioRecord.ERROR_INVALID_OPERATION:
                            Log.e("mAudioRecord.read---", "ERROR_INVALID_OPERATION");
                            break;
                        case AudioRecord.ERROR_DEAD_OBJECT:
                            Log.e("mAudioRecord.read---", "ERROR_DEAD_OBJECT");
                            break;
                        default:
                            Log.e("mAudioRecord.read---", "ERROR:" + length);
                    }
                }
            }
        }
    }

    public class RecordEncodec implements Runnable {

        @Override
        public void run() {
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            for (; ; ) {
                mLock.lock();
                PCM pcm = null;
                if (mIsStop.get()) {
                    mAudioCodec.stop();
                    mAudioCodec.release();
                    mLock.unlock();
                    mAudioRecord.stop();
                    Log.d("---", "mAudioRecord:release");
                    return;
                }
                while (mPCMS.size() == 0) {
                    try {
                        mCondition.await();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                pcm = mPCMS.poll();
                mLock.unlock();
//                Log.d("----", "pcm-length--" + pcm.data.length);
                int inputId = mAudioCodec.dequeueInputBuffer(TIMEOUT_US);
                if (inputId >= 0) {
                    ByteBuffer inputBuffer = mAudioCodec.getInputBuffer(inputId);
                    inputBuffer.clear();
                    inputBuffer.put(pcm.data, 0, pcm.data.length);
//                    Log.d("---", "queueInputBuffer:time:" + pcm.time / 1000);
                    mAudioCodec.queueInputBuffer(inputId, 0, pcm.data.length, pcm.time, 0);
                }
                int outputBufferId = mAudioCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_US);
                while (outputBufferId >= 0) {
                    ByteBuffer outputBuffer = mAudioCodec.getOutputBuffer(outputBufferId);
                    if (outputBuffer != null) {
                        outputBuffer.position(bufferInfo.offset);
                        outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
                        byte[] outData = new byte[bufferInfo.size];
                        outputBuffer.get(outData, bufferInfo.offset, bufferInfo.size);

                        if (bufferInfo.flags == BUFFER_FLAG_CODEC_CONFIG) {
//                            Log.d("RecordDecodec---", "BUFFER_FLAG_CODEC_CONFIG");
                            mSRSLibrtmpManager.addFrame(outData, outData.length, 1, bufferInfo.flags, 0);
                        } else {
                            calcTotalAudioTime(bufferInfo.presentationTimeUs / 1000);
//                            Log.d("RecordDecodec---", "bufferInfo.size:" + bufferInfo.size + "--bufferInfo.time:" + bufferInfo.presentationTimeUs + "--outData.length:" + outData.length + "--audioTimeIndex:" + audioTimeIndex);
                            mSRSLibrtmpManager.addFrame(outData, outData.length, 1, bufferInfo.flags, audioTimeIndex);
                        }
                        outputBuffer.clear();
                    }
                    mAudioCodec.releaseOutputBuffer(outputBufferId, false);
                    outputBufferId = mAudioCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_US);
                }
            }
        }
    }

    /**
     * 添加ADTS头
     *
     * @param packet
     * @param packetLen
     */
    private void addADTStoPacket(byte[] packet, int packetLen) {
        int profile = 2; // AAC LC
        int freqIdx = 4; // 44.1KHz
        int chanCfg = 1; // CPE

        // fill in ADTS data
        packet[0] = (byte) 0xFF;
        packet[1] = (byte) 0xF9;
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }

    public class RecordDecodec implements Runnable {

        @Override
        public void run() {
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            for (; ; ) {
                if (mIsStop.get()) {
                    break;
                }
                int outputBufferId = mAudioCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_US);
                if (outputBufferId >= 0) {
                    ByteBuffer outputBuffer = mAudioCodec.getOutputBuffer(outputBufferId);
                    if (outputBuffer != null) {
                        outputBuffer.position(bufferInfo.offset);
                        outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
                        byte[] outData = new byte[bufferInfo.size];
                        outputBuffer.get(outData, bufferInfo.offset, bufferInfo.size);

                        if (bufferInfo.flags == BUFFER_FLAG_CODEC_CONFIG) {
//                            Log.d("RecordDecodec---", "BUFFER_FLAG_CODEC_CONFIG");
                            mSRSLibrtmpManager.addFrame(outData, outData.length, 1, bufferInfo.flags, 0);
                        } else {
                            calcTotalAudioTime(bufferInfo.presentationTimeUs / 1000);
//                            Log.d("RecordDecodec---", "bufferInfo.size:" + bufferInfo.size + "--bufferInfo.time:" + bufferInfo.presentationTimeUs + "--outData.length:" + outData.length + "--audioTimeIndex:" + audioTimeIndex);
                            mSRSLibrtmpManager.addFrame(outData, outData.length, 1, bufferInfo.flags, audioTimeIndex);
                        }
                        outputBuffer.clear();
                    }
                    mAudioCodec.releaseOutputBuffer(outputBufferId, false);
                }
            }
            mAudioCodec.stop();
            mAudioCodec.release();
            Log.d("---", "mAudioRecord:release");
        }
    }

    private void addPCM(PCM pcm) {
        mLock.lock();
        mPCMS.add(pcm);
        mCondition.signal();
        mLock.unlock();
    }

    public Surface getSurface() {
        return mEGLRender.getDecodeSurface();
//        return mSurface;
    }


    public void close() {
        isStart = false;
        mIsStop.set(true);
        mLock.lock();
        mCondition.signal();
        mLock.unlock();
    }

    public void start(String url) {
        if (!TextUtils.isEmpty(url)) {
            mRtmpUrl = url;
        }
        new Thread(new SendRunable()).start();

    }

    public class SendRunable implements Runnable {

        @Override
        public void run() {
            if (!mSRSLibrtmpManager.setUrl(mRtmpUrl)) {
                Log.d("---", "connect false");
                return;
            }
//            mSRSLibrtmpManager.startScreenRecord();
            mHandler.sendEmptyMessage(START_PUSH);
//            if (!mSRSLibrtmpManager.setUrl(mRtmpUrl)) {
//                return;
//            }
            mEGLRender.start();
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            for (; ; ) {
                if (mIsStop.get()) {
                    break;
                }
                int outputBufferId = mVideoMediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_US);

                if (outputBufferId >= 0) {
                    ByteBuffer outputBuffer = mVideoMediaCodec.getOutputBuffer(outputBufferId);
                    if (outputBuffer != null) {
                        outputBuffer.position(bufferInfo.offset);
                        outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
                        byte[] outData = new byte[bufferInfo.size];
                        outputBuffer.get(outData, bufferInfo.offset, bufferInfo.size);

                        if (bufferInfo.flags == BUFFER_FLAG_CODEC_CONFIG) {
                            mSRSLibrtmpManager.addFrame(outData, outData.length, 2, bufferInfo.flags, 0);
                        } else {
                            calcTotalTime(bufferInfo.presentationTimeUs / 1000);
                            mSRSLibrtmpManager.addFrame(outData, outData.length, 2, bufferInfo.flags, getTimeIndex());
                        }
                        outputBuffer.clear();
                    }
                    mVideoMediaCodec.releaseOutputBuffer(outputBufferId, false);
                } else if (outputBufferId == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    Log.d("---", "INFO_OUTPUT_FORMAT_CHANGED:" + mVideoMediaCodec.getOutputFormat().toString());
                }
                try {
                    Thread.sleep(1000 / (int) mFrameRate);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            try {
                mEGLRender.stop();
                mVideoMediaCodec.stop();
                mVideoMediaCodec.release();
                Log.d("---", "mVideoMediaCodec:release");
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
    private int audioTimeIndex;
    private long lastAudioTime;

    public void calcTotalAudioTime(long currentTimeUs) {
        if (lastAudioTime <= 0) {
            this.lastAudioTime = currentTimeUs;
        }
        audioTimeIndex = currentTimeUs > lastAudioTime ? (int) (currentTimeUs - lastAudioTime) : audioTimeIndex;
    }

    public void calcTotalTime(long currentTimeUs) {
        if (lastTimeUs <= 0) {
            this.lastTimeUs = currentTimeUs;
        }
        timeIndex = (int) (currentTimeUs - lastTimeUs);
    }

    public void reset() {
        lastTimeUs = 0;
        timeIndex = 0;
        lastAudioTime = 0;
        audioTimeIndex = 0;
    }

    public int getTimeIndex() {
        return timeIndex;
    }

    static class PCM {
        byte[] data;
        long time;
        static long timeStart;

        public static void timeReset() {
            timeStart = 0;
        }

        public long currentTime() {
            return timeStart += (1 * 1000 * 1000 / 44100) * (data.length / 2);
        }
    }
}


