package com.android.hremoteservice;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.os.IBinder;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class MyService extends Service {

    private static final String TAG = "HRemoteSerivce";
    private static final String SERVICE_TYPE = "_http._tcp.";
    private String mServiceName = "HRemoteService";

    public MyService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "start");

        if (!serviceRunning) {
            serviceRunning = true;

            mThread = new Thread(new ServerThread());
            mThread.start();

            while (mPort == 0)
            {
                try {
                    Thread.sleep(100);
                } catch(InterruptedException e) {
                }
            }

            mNsdManager = (NsdManager) getSystemService(Context.NSD_SERVICE);
            initializeResolveListener();
            initializeRegistrationListener();
            NsdServiceInfo serviceInfo  = new NsdServiceInfo();
            serviceInfo.setPort(mPort);
            serviceInfo.setServiceName(mServiceName);
            serviceInfo.setServiceType(SERVICE_TYPE);
            mNsdManager.registerService(serviceInfo, NsdManager.PROTOCOL_DNS_SD, mRegistrationListener);
        }

        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "stop");

        if (serviceRunning) {
            serviceRunning = false;

            mThread.interrupt();

            try {
                mServerSocket.close();
            } catch (IOException ioe) {
            }

            try {
                mNsdManager.unregisterService(mRegistrationListener);
            } finally {
            }
            mRegistrationListener = null;
            mNsdManager = null;
        }
    }

    public void initializeResolveListener() {
        mResolveListener = new NsdManager.ResolveListener() {
            @Override
            public void onResolveFailed(NsdServiceInfo serviceInfo, int errorCode) {
                Log.e(TAG, "Resolve failed" + errorCode);
            }
            @Override
            public void onServiceResolved(NsdServiceInfo serviceInfo) {
                Log.e(TAG, "Resolve Succeeded. " + serviceInfo);
                /*if (serviceInfo.getServiceName().equals(mServiceName)) {
                    Log.d(TAG, "Same IP.");
                    return;
                }*/
                mService = serviceInfo;
            }
        };
    }

    public void initializeRegistrationListener() {
        mRegistrationListener = new NsdManager.RegistrationListener() {
            @Override
            public void onServiceRegistered(NsdServiceInfo NsdServiceInfo) {
                mServiceName = NsdServiceInfo.getServiceName();
                Log.d(TAG, "Service registered: " + mServiceName);
            }
            @Override
            public void onRegistrationFailed(NsdServiceInfo arg0, int arg1) {
                Log.d(TAG, "Service registration failed: " + arg1);
            }
            @Override
            public void onServiceUnregistered(NsdServiceInfo arg0) {
                Log.d(TAG, "Service unregistered: " + arg0.getServiceName());
            }
            @Override
            public void onUnregistrationFailed(NsdServiceInfo serviceInfo, int errorCode) {
                Log.d(TAG, "Service unregistration failed: " + errorCode);
            }
        };
    }

    class ServerThread implements Runnable {

        @Override
        public void run() {

            try {
                mServerSocket = new ServerSocket(0);
                mPort = mServerSocket.getLocalPort();

                while (!Thread.currentThread().isInterrupted()) {
                    mSocket = (Socket) mServerSocket.accept();

                    try {
                        DataInputStream in = new DataInputStream(mSocket.getInputStream());

                        while (!Thread.currentThread().isInterrupted()) {
                            int keyCode = in.readInt();
                            Log.d(TAG, "KeyCode: " + keyCode);
                        }
                    } catch(Exception e) {
                        Log.e(TAG, "Error ", e);
                    } finally {
                        mSocket.close();
                        Log.d(TAG, "Done");
                    }
                }
            } catch (IOException e) {
            }
        }
    }

    public static boolean  serviceRunning = false;

    private NsdManager mNsdManager;
    private NsdManager.ResolveListener mResolveListener;
    private NsdManager.RegistrationListener mRegistrationListener;
    private NsdServiceInfo mService;

    private ServerSocket mServerSocket = null;
    private Socket mSocket;
    private int mPort = 0;

    private Thread mThread = null;
}
