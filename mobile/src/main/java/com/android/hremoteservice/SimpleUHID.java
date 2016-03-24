package com.android.hremoteservice;

public class SimpleUHID {
    static {
        System.loadLibrary("jniSimpleUHID");
    }

    public SimpleUHID() {
        mFD = open();
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        close(mFD);
    }

    public void sendEvent(boolean btn1Down, boolean btn2Down, boolean btn3Down, short absX, short absY, short wheel) {
        sendEvent(mFD, btn1Down, btn2Down, btn3Down, absX, absY, wheel);
    }

    private native int open();
    private native void close(int fd);
    private native void sendEvent(int fd, boolean btn1Down, boolean btn2Down, boolean btn3Down, short absX, short absY, short wheel);
    private int mFD;
}
