package com.soundleap.app;

import android.annotation.SuppressLint;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.soundleap.app.bluetooth.BluetoothLeService;
import com.soundleap.app.bluetooth.BluetoothServiceConnection;
import com.soundleap.app.util.Constants;

import java.util.Locale;
import java.util.Map;
import java.util.Objects;

/**
 * This activity is responsible for displaying the real-time game statistics.
 * It communicates with a Bluetooth Low Energy (BLE) service to track game status and allows users to pause, resume or stop the game.
 */
@SuppressWarnings("MagicNumber")
public class GameStatsActivity extends AppCompatActivity implements BluetoothServiceConnection.BluetoothServiceConnectionListener {

    /**
     * The AccessibilityManager used to manage accessibility events.
     */
    private AccessibilityManager accessibilityManager;

    /**
     * Button for pausing or resuming the game.
     */
    private Button pauseButton;

    /**
     * TextView for displaying the elapsed time.
     */
    private TextView elapsedTimeText;

    /**
     * Bluetooth service connection to communicate with the BluetoothLowEnergy service.
     */
    private BluetoothServiceConnection bluetoothServiceConnection;

    /**
     * BluetoothLowEnergy service instance.
     */
    private BluetoothLeService bluetoothLeService;

    /**
     * Gson instance used for serializing and deserializing JSON data.
     */
    private final Gson gson = new Gson();

    /**
     * Boolean flag to track if the service is bound.
     */
    private boolean isBound = false;

    /**
     * Handler used to post delayed updates.
     */
    private final Handler handler = new Handler();

    /**
     * Start time of the game.
     */
    private long startTime = 0L;

    /**
     * Boolean flag to track if the game is running.
     */
    private boolean isRunning = true;

    /**
     * Broadcast receiver for receiving game status updates.
     * This updates the game status UI whenever a new status is broadcast.
     */
    private final BroadcastReceiver gameStatusChangedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(final Context context, final Intent intent) {
            String jsonString = Objects.requireNonNull(intent.getStringExtra("gameStatus"));
            Map<String, String> map = gson.fromJson(jsonString, new TypeToken<Map<String, String>>() {
            }.getType());

            LinearLayout linearLayout = findViewById(R.id.game_status);

            for (int i = linearLayout.getChildCount() - 1; i > 0; i--) { // Start from the last child
                View child = linearLayout.getChildAt(i);
                if (child instanceof TextView) {
                    linearLayout.removeViewAt(i);
                }
            }

            map.forEach((key, value) -> {
                TextView textView = new TextView(GameStatsActivity.this);
                textView.setTextSize(TypedValue.COMPLEX_UNIT_SP, 24);
                textView.setText(String.format("%s: %s", key, value));

                LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.WRAP_CONTENT,
                        LinearLayout.LayoutParams.WRAP_CONTENT
                );

                textView.setLayoutParams(layoutParams);

                linearLayout.addView(textView);
            });
        }
    };

    /**
     * Broadcast receiver for receiving indicator that a game ends.
     * This updates the game status UI that the game ended and closes screen.
     */
    private final BroadcastReceiver gameEndedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(final Context context, final Intent intent) {
            LinearLayout linearLayout = findViewById(R.id.game_status);
            TextView textView = new TextView(GameStatsActivity.this);
            textView.setTextSize(TypedValue.COMPLEX_UNIT_SP, 24);
            textView.setText(getString(R.string.game_stat_finished));
            textView.setPadding(0, 0, 0, 20);

            LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    LinearLayout.LayoutParams.WRAP_CONTENT
            );

            textView.setLayoutParams(layoutParams);
            linearLayout.addView(textView);

            if (accessibilityManager.isEnabled()) {
                AccessibilityEvent event = AccessibilityEvent.obtain(AccessibilityEvent.TYPE_ANNOUNCEMENT);
                event.getText().add("Disselected game");
                event.setClassName(getClass().getName());
                event.setPackageName(getPackageName());
                accessibilityManager.sendAccessibilityEvent(event);
            }

            isRunning = false;

            // Delay closing the activity using a Handler
            new Handler(Looper.getMainLooper()).postDelayed(new Runnable() {
                @Override
                public void run() {
                    bluetoothLeService.write(new byte[]{Constants.BYTECODE_CANCEL_GAME});
                    finish();
                }
            }, 2500); // Delay in milliseconds
        }
    };


    /**
     * Runnable that periodically updates the elapsed game time every second.
     */
    private final Runnable runnable = new Runnable() {
        @Override
        public void run() {
            if (isRunning) {
                // Calculate elapsed time
                long elapsedTime = System.currentTimeMillis() - startTime;

                // Convert milliseconds to hours, minutes, and seconds
                int hours = (int) (elapsedTime / (1000 * 60 * 60));
                int minutes = (int) ((elapsedTime / (1000 * 60)) % 60);
                int seconds = (int) ((elapsedTime / 1000) % 60);

                // Update the TextView
                String time = String.format(Locale.US, "%02d:%02d:%02d", hours, minutes, seconds);
                elapsedTimeText.setText(String.format("Time played: %s", time));

                // Post again after 1 second
                handler.postDelayed(this, 1000);
            }
        }
    };

    /**
     * Called when the activity is created. Initializes the UI and starts the game timer.
     *
     * @param savedInstanceState The saved instance state if available.
     */
    @Override
    @SuppressLint("CutPasteId")
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_game_stats);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        Button stopButton = findViewById(R.id.stop_btn);
        TextView title = findViewById(R.id.top_title);
        elapsedTimeText = findViewById(R.id.elapsed_time_text);

        accessibilityManager = (AccessibilityManager) getSystemService(Context.ACCESSIBILITY_SERVICE);

        bluetoothServiceConnection = new BluetoothServiceConnection(this);

        String nameOfGame = getIntent().getStringExtra("selectedGameName");
        title.setText(String.format("Game status for '%s'", nameOfGame));

        Button helpButton = findViewById(R.id.help_button);

        helpButton.setOnClickListener(view -> {
            LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
            View helpPopupView = inflater.inflate(R.layout.help_popup_layout, findViewById(android.R.id.content), false);

            final PopupWindow helpPopupWindow = new PopupWindow(
                    helpPopupView,
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    true // Focusable
            );

            helpPopupWindow.showAtLocation(view, Gravity.CENTER, 0, 0);

            Button closePopupButton = helpPopupView.findViewById(R.id.popup_close_button);
            closePopupButton.setOnClickListener(v -> helpPopupWindow.dismiss());

            TextView popupText = helpPopupView.findViewById(R.id.popup_text);
            popupText.setText(R.string.info_game_stats);
        });

        stopButton.setOnClickListener(view -> {

            LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
            View quitConfirmationPopupView = inflater.inflate(R.layout.confirmation_popup_layout, findViewById(android.R.id.content), false);

            final PopupWindow quitConfirmationPopupWindow = new PopupWindow(
                    quitConfirmationPopupView,
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    true // Focusable
            );

            quitConfirmationPopupWindow.showAtLocation(view, Gravity.CENTER, 0, 0);

            Button cancelPopupButton = quitConfirmationPopupView.findViewById(R.id.popup_cancel_button);
            cancelPopupButton.setOnClickListener(v -> quitConfirmationPopupWindow.dismiss());

            Button quitPopupButton = quitConfirmationPopupView.findViewById(R.id.popup_confirmation_button);
            quitPopupButton.setOnClickListener(v -> {
                quitConfirmationPopupWindow.dismiss();
                bluetoothLeService.write(new byte[]{Constants.BYTECODE_CANCEL_GAME});
                finish();
            });
        });

        pauseButton = findViewById(R.id.pause_btn);

        pauseButton.setOnClickListener(v -> {
            if (isRunning) {
                isRunning = false;
                pauseButton.setText(R.string.btn_text_resume);
            } else {
                isRunning = true;
                startTime = System.currentTimeMillis() - getElapsedMilliseconds(); // Adjust start time
                handler.post(runnable);
                pauseButton.setText(R.string.btn_text_pause);
            }
        });

        startTime = System.currentTimeMillis();
        handler.post(runnable);
    }

    /**
     * Called when the activity starts. It binds the Bluetooth service and registers the receiver for game status updates.
     */
    @Override
    protected void onStart() {
        super.onStart();
        Intent intent = new Intent(this, BluetoothLeService.class);
        bindService(intent, bluetoothServiceConnection, Context.BIND_AUTO_CREATE);

        IntentFilter filterGameStatusChanged = new IntentFilter(Constants.GAME_STATUS_DATA_ACTION);
        LocalBroadcastManager.getInstance(this).registerReceiver(gameStatusChangedReceiver, filterGameStatusChanged);

        IntentFilter filterGameEndedReceiver = new IntentFilter(Constants.GAME_ENDED_ACTION);
        LocalBroadcastManager.getInstance(this).registerReceiver(gameEndedReceiver, filterGameEndedReceiver);
    }

    /**
     * This method is called when the activity is destroyed. It unbinds the Bluetooth service and unregisters the receiver.
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (isBound) {
            unbindService(bluetoothServiceConnection);
            isBound = false;
        }

        try {
            LocalBroadcastManager.getInstance(this).unregisterReceiver(gameStatusChangedReceiver);
            LocalBroadcastManager.getInstance(this).unregisterReceiver(gameEndedReceiver);
        } catch (IllegalArgumentException e) {
            e.printStackTrace(); // Log the error but prevent a crash
        }
    }

    /**
     * This method is called when the Bluetooth service is connected. It sets the Bluetooth service instance.
     *
     * @param service The BluetoothLeService instance.
     */
    @Override
    public void onServiceConnected(final BluetoothLeService service) {
        this.bluetoothLeService = service;
        isBound = true;
    }


    /**
     * This method is called when the Bluetooth service is disconnected.
     */
    @Override
    public void onServiceDisconnected() {
        isBound = false;
    }

    /**
     * This method gets the elapsed time in milliseconds from the displayed time.
     *
     * @return The elapsed time in milliseconds.
     */
    private long getElapsedMilliseconds() {
        String[] timeParts = elapsedTimeText.getText().toString().split(" ")[2].split(":");
        int hours = Integer.parseInt(timeParts[0]);
        int minutes = Integer.parseInt(timeParts[1]);
        int seconds = Integer.parseInt(timeParts[2]);
        return (hours * 60L * 60L + minutes * 60L + seconds) * 1000L;
    }
}
