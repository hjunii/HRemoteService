package com.android.hremoteservice;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.content.Intent;

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
            startService(new Intent(MainActivity.this, MyService.class));
            Button button = (Button) v;
            button.setText("Stop");
            button.setOnClickListener(stopListener);
        }
    }

    protected class StopListener implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            stopService(new Intent(MainActivity.this, MyService.class));
            Button button = (Button) v;
            button.setText("Start");
            button.setOnClickListener(startListener);
        }
    }

    private StartListener startListener = new StartListener();
    private StopListener stopListener = new StopListener();
}
