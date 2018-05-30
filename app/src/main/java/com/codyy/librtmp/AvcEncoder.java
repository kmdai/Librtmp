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
    private Callback mCallback;
    private double mFrameRate;
    private int mBitrate;
    ByteBuffer mByteBuffer;
    private final int TIMEOUT_US = 10000;
    boolean mIsStop;
    //    private LibrtmpManager mLibrtmpManager;
    //                RTMPMuxer mRTMPMuxer;
    private HandlerThread mHandlerThread;
    private Handler mHandler;
    long indexTime = 0;
    private SRSLibrtmpManager mSRSLibrtmpManager;

    public AvcEncoder(int width, int height, double framerate, int bitrate) {
        mIsStop = false;
        m_width = width;
        m_height = height;
        mFrameRate = framerate;
        mBitrate = bitrate;
        mSRSLibrtmpManager = new SRSLibrtmpManager();
//        mLibrtmpManager = new LibrtmpManager();
//        mLibrtmpManager.rtmpInit();
////        Log.d("------", "mLibrtmpManager.setUrl");
//        mLibrtmpManager.setUrl("rtmp://10.5.225.38:1935/srs/kmdai");
//        try {
//            mMediaCodec = MediaCodec.createEncoderByType("video/avc");
//        } catch (IOException e) {
//            e.printStackTrace();
//            return;
////        }
        reset();
//        mRTMPMuxer = new RTMPMuxer();
//        mRTMPMuxer.open("rtmp://10.5.225.38:1935/srs/kmdai", width, height);
//       mRTMPMuxer.write_flv_header(false,true);
        mHandlerThread = new HandlerThread("sendframe");
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper(), new Handler.Callback() {
            @Override
            public boolean handleMessage(Message msg) {
                Frame frame = (Frame) msg.obj;
//                Log.d("------", ":" + getTimeIndex());
//                mRTMPMuxer.writeVideo(frame.getData(), 0, frame.getSize(), getTimeIndex());
                if (frame.getType() == MediaCodec.BUFFER_FLAG_CODEC_CONFIG) {
//                    int i = 4;
//                    for (; ; ) {
//                        if (frame.mData[i++] == 0x00 && frame.mData[i++] == 0x00 && frame.mData[i++] == 0x00 && frame.mData[i++] == 0x01) {
//                            break;
//                        }
//                    }
//                    byte[] sps = new byte[i - 8];
//                    System.arraycopy(frame.getData(), 4, sps, 0, sps.length);
//                    byte[] pps = new byte[frame.getSize() - i];
//                    System.arraycopy(frame.getData(), i, pps, 0, pps.length);
//                    mLibrtmpManager.setSpsPps(frame.getData(), frame.mSize);
//                    mLibrtmpManager.sendSpsPPs(sps, sps.length, pps, pps.length);
                } else {
                    indexTime += 33;
                    byte[] data = new byte[frame.mSize - 4];
                    System.arraycopy(frame.getData(), 4, data, 0, data.length);
//                    int time = getTimeIndex();
//                    if (time <= 0) {
//                        return false;
//                    }
//                    mLibrtmpManager.sendChunk(data, data.length, frame.isKeyFrame() ? 1 : 0, getTimeIndex());
                }
                return false;
            }
        });
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
//        Log.d("AvcEncoder---", name);

        try {
            mMediaCodec = MediaCodec.createByCodecName(name);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mMediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mSurface = mMediaCodec.createInputSurface();
//        mMediaCodec.setCallback(new MediaCodec.Callback() {
//            @Override
//            public void onInputBufferAvailable(@NonNull MediaCodec mMediaCodec, int i) {
//
//            }
//
//            @Override
//            public void onOutputBufferAvailable(@NonNull MediaCodec mMediaCodec, int i, @NonNull MediaCodec.BufferInfo bufferInfo) {
//                ByteBuffer outputBuffer = mMediaCodec.getOutputBuffer(i);
//                mByteBuffer.clear();
////                Log.d("---", ":" + bufferInfo.presentationTimeUs);
//                mByteBuffer.putLong(bufferInfo.presentationTimeUs);
//                byte[] presentationTimeUs = mByteBuffer.array();
//                byte[] outData = new byte[bufferInfo.mSize + 8];
////                outputBuffer.get(outData);
//                outputBuffer.get(outData, 8, bufferInfo.mSize);
//                System.arraycopy(presentationTimeUs, 0, outData, 0, 8);
//                if (mCallback != null) {
//                    mCallback.onOutputBufferAvailable(outData);
//                }
//                mMediaCodec.releaseOutputBuffer(i, false);//释放
//            }
//
//            @Override
//            public void onError(@NonNull MediaCodec mMediaCodec, @NonNull MediaCodec.CodecException e) {
//                Log.d("AvcEncoder-", "onError:" + e.toString());
//            }
//
//            @Override
//            public void onOutputFormatChanged(@NonNull MediaCodec mMediaCodec, @NonNull MediaFormat mediaFormat) {
//
//            }
//        });
        mMediaCodec.start();
    }

    public Surface getSurface() {
        return mSurface;
    }

    public void setCallback(Callback callback) {
        mCallback = callback;
    }

    public void close() {
        mIsStop = true;
    }

    public void start() {
        new Thread(new SendRunable()).start();
    }

    public int offerEncoder(byte[] input, byte[] output) {
        int pos = 0;
//        swapYV12toI420(input, yuv420, m_width, m_height);
        try {
            ByteBuffer[] inputBuffers = mMediaCodec.getInputBuffers();
            ByteBuffer[] outputBuffers = mMediaCodec.getOutputBuffers();
            int inputBufferIndex = mMediaCodec.dequeueInputBuffer(-1);
            if (inputBufferIndex >= 0) {
                ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
                inputBuffer.clear();
//                inputBuffer.put(yuv420);
//                mMediaCodec.queueInputBuffer(inputBufferIndex, 0, yuv420.length, 0, 0);
            }

            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            int outputBufferIndex = mMediaCodec.dequeueOutputBuffer(bufferInfo, 0);

            while (outputBufferIndex >= 0) {
                ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];
                byte[] outData = new byte[bufferInfo.size];
                outputBuffer.get(outData);

                if (m_info != null) {
                    System.arraycopy(outData, 0, output, pos, outData.length);
                    pos += outData.length;

                } else {
                    ByteBuffer spsPpsBuffer = ByteBuffer.wrap(outData);
                    if (spsPpsBuffer.getInt() == 0x00000001) {
                        m_info = new byte[outData.length];
                        System.arraycopy(outData, 0, m_info, 0, outData.length);
                    } else {
                        return -1;
                    }
                }

                mMediaCodec.releaseOutputBuffer(outputBufferIndex, false);
                outputBufferIndex = mMediaCodec.dequeueOutputBuffer(bufferInfo, 0);
            }

            if (output[4] == 0x65) //key frame
            {
//                System.arraycopy(output, 0, yuv420, 0, pos);
                System.arraycopy(m_info, 0, output, 0, m_info.length);
//                System.arraycopy(yuv420, 0, output, m_info.length, pos);
                pos += m_info.length;
            }

        } catch (Throwable t) {
            t.printStackTrace();
        }

        return pos;
    }

    private void swapYV12toI420(byte[] yv12bytes, byte[] i420bytes, int width, int height) {
        System.arraycopy(yv12bytes, 0, i420bytes, 0, width * height);
        System.arraycopy(yv12bytes, width * height + width * height / 4, i420bytes, width * height, width * height / 4);
        System.arraycopy(yv12bytes, width * height, i420bytes, width * height + width * height / 4, width * height / 4);
    }

    public interface Callback {
        void onOutputBufferAvailable(byte[] data);
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
                    Log.d("----","offset--"+bufferInfo.offset);
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
                mHandlerThread.quitSafely();
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

    class Frame {
        byte[] mData;
        int mSize;
        int mType;
        boolean mKeyFrame;

        Frame(byte[] data, int size, int type, boolean keyFrame) {
            mData = data;
            this.mSize = size;
            mType = type;
            mKeyFrame = keyFrame;
        }

        public byte[] getData() {
            return mData;
        }

        public boolean isKeyFrame() {
            return mKeyFrame;
        }

        public void setKeyFrame(boolean keyFrame) {
            mKeyFrame = keyFrame;
        }

        public void setData(byte[] data) {
            mData = data;
        }

        public int getSize() {
            return mSize;
        }

        public int getType() {
            return mType;
        }

        public void setType(int type) {
            mType = type;
        }

        public void setSize(int size) {
            this.mSize = size;
        }
    }
}


