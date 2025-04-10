package com.soundleap.app;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.soundleap.app.bluetooth.BluetoothLeService;
import com.soundleap.app.bluetooth.BluetoothServiceConnection;
import com.soundleap.app.model.Pad;
import com.soundleap.app.util.Constants;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * This activity is responsible for configuring the pads required for a game.
 * It handles user interactions such as selecting pads, starting the game, and navigating back to the previous screen.
 */
@SuppressWarnings("MagicNumber")
public class PadsConfigActivity extends AppCompatActivity implements Pad.PadClickListener, BluetoothServiceConnection.BluetoothServiceConnectionListener {

    /**
     * List of pads to be used for the current game.
     */
    private final List<Pad> pads = new ArrayList<>();

    /**
     * Vertical layout that holds all pad buttons on the screen.
     */
    private LinearLayout verticalLayout;

    /**
     * Button to start the game once all pads are selected.
     */
    private Button startButton;

    /**
     * Name of the selected game.
     */
    private String gameName;

    /**
     * Unique identifier (UID) for the current game.
     */
    private String uid;

    /**
     * Byte code associated with the current game.
     */
    private byte gameByteCode;

    /**
     * BluetoothLeService instance used for communicating with the Bluetooth device.
     */
    private BluetoothLeService bluetoothLeService;

    /**
     * Flag indicating whether the Bluetooth service is bound.
     */
    private boolean isBound = false;

    /**
     * BluetoothServiceConnection instance used for binding and interacting with BluetoothLeService.
     */
    private BluetoothServiceConnection bluetoothServiceConnection;

    /**
     * This method is called when the activity is started. It binds the Bluetooth service.
     */
    @Override
    protected void onStart() {
        super.onStart();
        Intent intent = new Intent(this, BluetoothLeService.class);
        bindService(intent, bluetoothServiceConnection, Context.BIND_AUTO_CREATE);
    }

    /**
     * This method is called when the activity is destroyed. It unbinds the Bluetooth service if bound.
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (isBound) {
            unbindService(bluetoothServiceConnection);
        }
    }

    /**
     * This method is called when the activity is created. Initializes the UI elements, sets up buttons,
     * and prepares the pads for the selected game.
     *
     * @param savedInstanceState The saved instance state of the activity, if available.
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_pads_config);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        verticalLayout = findViewById(R.id.vertical_layout);
        startButton = findViewById(R.id.start_btn);
        Button backButton = findViewById(R.id.back_btn);

        startButton.setEnabled(false);

        gameName = getIntent().getStringExtra("selectedGameName");
        if (gameName == null) {
            gameName = "DefaultGameName";
        }

        addPadsForGame(gameName);

        startButton.setOnClickListener(v -> {
            Intent intent = new Intent(PadsConfigActivity.this, GameStatsActivity.class);
            intent.putExtra("selectedGameName", gameName);

            ExecutorService executor = Executors.newSingleThreadExecutor();
            executor.execute(() -> {
                bluetoothLeService.write(new byte[]{Constants.BYTECODE_START_GAME});

                try {
                    Thread.sleep(100);
                    String gameConfigs = getIntent().getStringExtra("gameConfigs");
                    if (gameConfigs != null) {
                        bluetoothLeService.write(gameConfigs.getBytes()); // convert to byte[] if needed
                    }

                    Thread.sleep(1000);
                    String gameCode = getIntent().getStringExtra("gameCode");
                    if (gameCode != null) {
                        int chunkSize = 200;
                        for (int i = 0; i < gameCode.length(); i += chunkSize) {
                            int end = Math.min(gameCode.length(), i + chunkSize);
                            String chunk = gameCode.substring(i, end);
                            bluetoothLeService.write(chunk.getBytes());
                            Thread.sleep(500);
                        }
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }

                runOnUiThread(() -> {
                    startActivity(intent);
                    finish();
                });
            });
        });


        backButton.setOnClickListener(v -> {
            Intent intent = new Intent(PadsConfigActivity.this, GameConfigActivity.class);
            startActivity(intent);
            finish();
        });

        bluetoothServiceConnection = new BluetoothServiceConnection(this);

        Button helpButton = findViewById(R.id.help_button);

        helpButton.setOnClickListener(view -> {
            LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
            View helpPopupView = inflater.inflate(R.layout.help_popup_layout, findViewById(android.R.id.content), false);

            final PopupWindow helpPopupWindow = new PopupWindow(
                    helpPopupView,
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    LinearLayout.LayoutParams.WRAP_CONTENT,
                    true
            );

            helpPopupWindow.showAtLocation(view, Gravity.CENTER, 0, 0);
            TextView popupText = helpPopupView.findViewById(R.id.popup_text);
            popupText.setText(R.string.info_pads_config);

            Button closePopupButton = helpPopupView.findViewById(R.id.popup_close_button);
            closePopupButton.setOnClickListener(v -> helpPopupWindow.dismiss());
        });
    }

    /**
     * This method adds the pads required for a game based on the game's name.
     *
     * @param nameOfGame The name of the game for which pads are added.
     */
    private void addPadsForGame(final String nameOfGame) {
        switch (nameOfGame) {
            case "Memory":
                pads.add(new Pad(this, (byte) 0x01, "#D62828"));
                pads.add(new Pad(this, (byte) 0x02, "#0077B6"));
                pads.add(new Pad(this, (byte) 0x03, "#38B000"));
                break;
            case "Reaction Game":
            case "Himmel und Hölle":
                pads.add(new Pad(this, (byte) 0x01, "#D62828"));
                pads.add(new Pad(this, (byte) 0x02, "#0077B6"));
                break;
            default:
                throw new UnsupportedOperationException("This game is not supported!");
        }
        showPadsOnScreen();
    }

    /**
     * This method displays the pads on the screen in a grid format.
     */
    private void showPadsOnScreen() {
        for (int i = 0; i < pads.size(); i += 2) {
            LinearLayout horizontalLayout = new LinearLayout(this);
            horizontalLayout.setOrientation(LinearLayout.HORIZONTAL);
            horizontalLayout.setGravity(Gravity.CENTER);

            for (int j = 0; j < 2; j++) {
                if (i + j < pads.size()) {
                    Pad pad = pads.get(i + j);
                    horizontalLayout.addView(pad.getButton("Pad " + i, this));
                }
            }
            verticalLayout.addView(horizontalLayout);
        }
    }

    /**
     * This method is called when a pad is clicked. Enables the start button when all pads are clicked.
     *
     * @param byteCode The byte code associated with the clicked pad.
     */
    @Override
    public void onPadClicked(final byte byteCode) {
        boolean allPadsClicked = pads.stream().allMatch(Pad::isClicked);
        startButton.setEnabled(allPadsClicked);
        bluetoothLeService.write(new byte[]{byteCode});
    }

    /**
     * This method is called when the Bluetooth service is connected. Sets the BluetoothLeService instance.
     *
     * @param service The connected BluetoothLeService instance.
     */
    @Override
    public void onServiceConnected(final BluetoothLeService service) {
        this.bluetoothLeService = service;
        isBound = true;
    }

    /**
     * This method is called when the Bluetooth service is disconnected. Updates the isBound flag.
     */
    @Override
    public void onServiceDisconnected() {
        isBound = false;
    }
}
