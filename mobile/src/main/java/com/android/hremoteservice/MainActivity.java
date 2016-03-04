package com.android.hremoteservice;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.content.Intent;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button button = (Button) findViewById(R.id.button);
        if (MyService.serviceRunning) {
            button.setText("Stop");
            button.setOnClickListener(stopListener);
        }
        else {
            button.setText("Start");
            button.setOnClickListener(startListener);
        }
    }

    protected class StartListener implements View.OnClickListener {
        @Override
        public void onClick(View v) {

            try
            {
                Process sh = Runtime.getRuntime().exec("su");
                OutputStream os = sh.getOutputStream();
                os.write("chmod 666 /dev/uinput".getBytes());
            } catch (IOException e) {
                Toast toast = Toast.makeText(getApplicationContext(),
                        getResources().getString(R.string.root_need_msg), Toast.LENGTH_LONG);
                toast.setGravity(Gravity.CENTER, 0, 0);
                toast.show();
                return;
            }

            startService(new Intent(MainActivity.this, MyService.class));
            Button button = (Button) v;
            button.setText("Stop");
            button.setOnClickListener(stopListener);
        }
    }

    protected class StopListener implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            try
            {
                Process sh = Runtime.getRuntime().exec("su");
                OutputStream os = sh.getOutputStream();
                os.write("chmod 660 /dev/uinput".getBytes());
            } catch (IOException e) {
            }

            stopService(new Intent(MainActivity.this, MyService.class));
            Button button = (Button) v;
            button.setText("Start");
            button.setOnClickListener(startListener);
        }
    }

    private StartListener startListener = new StartListener();
    private StopListener stopListener = new StopListener();
}
