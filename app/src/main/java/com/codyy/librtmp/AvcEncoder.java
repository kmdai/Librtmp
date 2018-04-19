package com.codyy.librtmp;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;
import android.view.Surface;

import com.kmdai.rtmppush.LibrtmpManager;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;


public class AvcEncoder {

    private MediaCodec mediaCodec;
    int m_width;
    int m_height;
    byte[] m_info = null;
    private Surface mSurface;
    DatagramSocket mSocket;
    InetAddress mInetAddress;
    private Callback mCallback;
    ByteBuffer mByteBuffer;
    private final int TIMEOUT_US = 10000;
    boolean mIsStop;
    private LibrtmpManager mLibrtmpManager;
    private int mTimeStamp;
//                RTMPMuxer mRTMPMuxer;
    private HandlerThread mHandlerThread;
    private Handler mHandler;
    long indexTime = 0;

    public AvcEncoder(int width, int height, final int framerate, int bitrate) {
        mIsStop = false;
        m_width = width;
        m_height = height;
        mTimeStamp = 1000 / framerate;
        mLibrtmpManager = new LibrtmpManager();
        mLibrtmpManager.rtmpInit();
//        Log.d("------", "mLibrtmpManager.setUrl");
        mLibrtmpManager.setUrl("rtmp://10.5.225.38:1935/srs/kmdai");
//        try {
//            mediaCodec = MediaCodec.createEncoderByType("video/avc");
//        } catch (IOException e) {
//            e.printStackTrace();
//            return;
////        }
        reset();
//        mRTMPMuxer = new RTMPMuxer();
//        mRTMPMuxer.open("rtmp://10.5.225.38:1935/srs/kmdai", width, height);
//       mRTMPMuxer.write_flv_header(false,true);
        try {
            mSocket = new DatagramSocket();
            mInetAddress = InetAddress.getByName("192.168.3.126");
        } catch (SocketException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
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
                    mLibrtmpManager.setSpsPps(frame.getData(), frame.mSize);
//                    mLibrtmpManager.sendSpsPPs(sps, sps.length, pps, pps.length);
                } else {
                    indexTime += 33;
                    byte[] data = new byte[frame.mSize - 4];
                    System.arraycopy(frame.getData(), 4, data, 0, data.length);
//                    int time = getTimeIndex();
//                    if (time <= 0) {
//                        return false;
//                    }
                    mLibrtmpManager.sendChunk(data, data.length, frame.isKeyFrame() ? 1 : 0, getTimeIndex());
                }
                return false;
            }
        });
        MediaFormat mediaFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, bitrate);
        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, framerate);
        mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AVCProfileMain);
        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);
        MediaCodecList mediaCodecList = new MediaCodecList(MediaCodecList.REGULAR_CODECS);
        MediaCodecInfo[] mediaCodecInfos = mediaCodecList.getCodecInfos();
        String name = mediaCodecList.findEncoderForFormat(mediaFormat);
        mByteBuffer = ByteBuffer.allocate(8);
        Log.d("AvcEncoder---", name);
        for (MediaCodecInfo mediaCodecInfo : mediaCodecInfos) {
            String[] strings = mediaCodecInfo.getSupportedTypes();
            String str = "";
            for (String type : strings) {
                str += type;
            }
            Log.d("AvcEncoder---", mediaCodecInfo.getName() + ":" + str);
        }
        try {
            mediaCodec = MediaCodec.createByCodecName(name);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        mSurface = mediaCodec.createInputSurface();
//        mediaCodec.setCallback(new MediaCodec.Callback() {
//            @Override
//            public void onInputBufferAvailable(@NonNull MediaCodec mediaCodec, int i) {
//
//            }
//
//            @Override
//            public void onOutputBufferAvailable(@NonNull MediaCodec mediaCodec, int i, @NonNull MediaCodec.BufferInfo bufferInfo) {
//                ByteBuffer outputBuffer = mediaCodec.getOutputBuffer(i);
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
//                mediaCodec.releaseOutputBuffer(i, false);//释放
//            }
//
//            @Override
//            public void onError(@NonNull MediaCodec mediaCodec, @NonNull MediaCodec.CodecException e) {
//                Log.d("AvcEncoder-", "onError:" + e.toString());
//            }
//
//            @Override
//            public void onOutputFormatChanged(@NonNull MediaCodec mediaCodec, @NonNull MediaFormat mediaFormat) {
//
//            }
//        });
        mediaCodec.start();
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
            ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();
            ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers();
            int inputBufferIndex = mediaCodec.dequeueInputBuffer(-1);
            if (inputBufferIndex >= 0) {
                ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
                inputBuffer.clear();
//                inputBuffer.put(yuv420);
//                mediaCodec.queueInputBuffer(inputBufferIndex, 0, yuv420.length, 0, 0);
            }

            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            int outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);

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

                mediaCodec.releaseOutputBuffer(outputBufferIndex, false);
                outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0);
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
        int mPackageSize = 50000;

        @Override
        public void run() {
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            for (; ; ) {
                if (mIsStop) {
                    break;
                }
                int outputBufferId = mediaCodec.dequeueOutputBuffer(bufferInfo, TIMEOUT_US);
                if (outputBufferId >= 0) {
                    ByteBuffer outputBuffer = mediaCodec.getOutputBuffer(outputBufferId);
                    byte[] outData = new byte[bufferInfo.size];
                    outputBuffer.get(outData, 0, bufferInfo.size);
                    calcTotalTime(bufferInfo.presentationTimeUs / 1000);
//                    try {
//                        mRtmpClient.write(outData);
//                    } catch (IOException e) {
//                        e.printStackTrace();
//                    }
                    Message message = mHandler.obtainMessage();
                    message.obj = new Frame(outData, bufferInfo.size, bufferInfo.flags, bufferInfo.flags == MediaCodec.BUFFER_FLAG_KEY_FRAME);
                    mHandler.sendMessage(message);
                }
                if (outputBufferId >= 0 && bufferInfo.flags == MediaCodec.BUFFER_FLAG_CODEC_CONFIG) {
//                    ByteBuffer outputBuffer = mediaCodec.getOutputBuffer(outputBufferId);
//                    byte[] outData = new byte[bufferInfo.mSize];
//                    outputBuffer.get(outData, 0, bufferInfo.mSize);
////                    mLibrtmpManager.setSpsPps(outData, bufferInfo.mSize);
                }
                if (outputBufferId >= 0 && bufferInfo.flags != MediaCodec.BUFFER_FLAG_CODEC_CONFIG) {

//                    ByteBuffer outputBuffer = mediaCodec.getOutputBuffer(outputBufferId);
//                    mByteBuffer.clear();
//                    Log.d("---", "bufferInfo.presentationTimeUs:" + bufferInfo.presentationTimeUs);
//                    byte[] outData = new byte[bufferInfo.mSize];
//                    outputBuffer.get(outData, 0, bufferInfo.mSize);
//                    Log.d("---", "bufferInfo.mSize:" + bufferInfo.mSize);
//                    if (bufferInfo.flags == MediaCodec.BUFFER_FLAG_KEY_FRAME) {
//                        mLibrtmpManager.sendChunk(outData, bufferInfo.mSize, 1, mTimeStamp);
//                    } else {
//                        mLibrtmpManager.sendChunk(outData, bufferInfo.mSize, 0, mTimeStamp);
//                    }
//                    mByteBuffer.putLong(bufferInfo.presentationTimeUs);
//                    byte[] presentationTimeUs = mByteBuffer.array();
//                    Log.d("---10", ":" + bufferInfo.mSize);
//                    Log.d("---16", ":" + Integer.toString(bufferInfo.mSize, 16));
//                    if (bufferInfo.mSize > mPackageSize) {
//                        presentationTimeUs[0] = 0x1;
//                        byte[] outData = new byte[bufferInfo.mSize];
//                        outputBuffer.get(outData, 0, bufferInfo.mSize);
//
//                        byte[] out1 = new byte[mPackageSize + 8];
//                        System.arraycopy(outData, 0, out1, 8, mPackageSize);
//                        System.arraycopy(presentationTimeUs, 0, out1, 0, 8);
//                        send(out1);
//                        Log.d("send---", "out1:" + out1.length);
//                        byte[] out2 = new byte[outData.length - mPackageSize + 8];
//                        System.arraycopy(outData, mPackageSize, out2, 8, outData.length - mPackageSize);
//                        System.arraycopy(presentationTimeUs, 0, out2, 0, 8);
//                        send(out2);
//                        Log.d("send---", "out2:" + out2.length);
//                    } else {
//                        byte[] outData = new byte[bufferInfo.mSize + 8];
//                        outputBuffer.get(outData, 8, bufferInfo.mSize);
//                        System.arraycopy(presentationTimeUs, 0, outData, 0, 8);
//                        send(outData);
//                    }

//                outputBuffer.get(outData);
//                    outputBuffer.get(outData, 8, bufferInfo.mSize);
//                    System.arraycopy(presentationTimeUs, 0, outData, 0, 8);
//                    if (outData.length > 60000) {
//                        presentationTimeUs[0] = 0x1;
//                        byte[] out1 = new byte[60008];
//                        out1[0] = 0x001;
//                        System.arraycopy(outData, 0, out1, 8, 60000);
//                        System.arraycopy(presentationTimeUs, 0, out1, 0, 8);
//                        send(out1);
//                        byte[] out2 = new byte[outData.length - 60000 + 8];
//                        System.arraycopy(outData, 60000, out2, 8, outData.length - 60000);
//                        System.arraycopy(presentationTimeUs, 0, out2, 0, 8);
//                        send(out2);
//                    } else {
//
//                    }
//                    DatagramPacket packet = new DatagramPacket(outData, outData.length, mInetAddress, 5000);
//                    try {
//                        mSocket.send(packet);
//                    } catch (IOException e) {
//                        e.printStackTrace();
//                    }
                    mediaCodec.releaseOutputBuffer(outputBufferId, false);
                }
            }
            try {
                mediaCodec.stop();
                mediaCodec.release();
                Log.d("---", ":release");
                mLibrtmpManager.rtmpFree();
//                mRTMPMuxer.close();
                mHandlerThread.quitSafely();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void send(byte[] data) {
        DatagramPacket packet = new DatagramPacket(data, data.length, mInetAddress, 5000);
        try {
            mSocket.send(packet);
        } catch (IOException e) {
            e.printStackTrace();
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


