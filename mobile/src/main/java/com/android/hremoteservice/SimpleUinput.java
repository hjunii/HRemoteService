package com.android.hremoteservice;

public class SimpleUinput {
    static {
        System.loadLibrary("jniSimpleUinput");
    }

    public SimpleUinput() {
        mFD = openUinput();
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        closeUinput(mFD);
    }

    public void send(short type, short code, int value) {
        send(mFD, type, code, value);
    }

    private native int openUinput();
    private native void closeUinput(int fd);
    private native void send(int fd, short type, short code, int value);
    private int mFD;
}
