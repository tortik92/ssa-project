package com.soundleap.app;

import android.Manifest;
import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.google.android.material.textfield.TextInputEditText;
import com.soundleap.app.bluetooth.BluetoothLeService;
import com.soundleap.app.bluetooth.BluetoothServiceConnection;
import com.soundleap.app.util.Constants;

import java.util.Arrays;
import java.util.Objects;
import java.util.stream.Stream;

/**
 * This activity is responsible for user login, Bluetooth device discovery, and connecting to a Bluetooth device
 * based on a 6-digit code input by the user. It provides interaction with the Bluetooth hardware and manages
 * communication with a remote Bluetooth device.
 */
public class LoginActivity extends AppCompatActivity implements BluetoothServiceConnection.BluetoothServiceConnectionListener {

    /**
     * The BluetoothLeService instance responsible for managing Bluetooth connections.
     */
    private BluetoothLeService bluetoothLeService;

    /**
     * A flag to track whether the Bluetooth service is bound.
     */
    private boolean isBound = false;

    /**
     * ActivityResultLauncher to handle Bluetooth enable request result.
     */
    private ActivityResultLauncher<Intent> enableBluetoothLauncher;

    /**
     * User input from the input field (6-digit code).
     */
    private String userInput;

    /**
     * Bluetooth service connection object used for binding to BluetoothLeService.
     */
    private BluetoothServiceConnection bluetoothServiceConnection;

    /**
     * BroadcastReceiver to listen for discovered Bluetooth devices.
     * It checks if the discovered device matches the inputted 6-digit code and connects to it.
     */
    private final BroadcastReceiver deviceFoundReceiver = new BroadcastReceiver() {
        @Override
        @SuppressLint("MissingPermission")
        public void onReceive(final Context context, final Intent intent) {
            if (hasPermissions(Manifest.permission.BLUETOOTH_CONNECT)) {
                BluetoothDevice foundDevice = intent.getParcelableExtra("device");
                if (foundDevice == null) {
                    return;
                }
                String foundDeviceName = foundDevice.getName();
                String foundDeviceAddress = foundDevice.getAddress();
                if (foundDeviceName != null && foundDeviceAddress != null && foundDeviceName.equals(Constants.DEVICE_PREFIX + userInput)) {
                    if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED) {
                        bluetoothLeService.connect(foundDevice);
                    } else {
                        requestBluetoothPermissions();
                    }
                    Intent gameSelectionIntent = new Intent(context, GameSelectionActivity.class);
                    gameSelectionIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
                    startActivity(gameSelectionIntent);
                }
            }
        }
    };

    /**
     * This method initializes the activity, sets up UI, and prepares Bluetooth permissions and connection.
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_login);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.login), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (!hasPermissions(android.Manifest.permission.BLUETOOTH_SCAN, android.Manifest.permission.BLUETOOTH_CONNECT)) {
                requestBluetoothPermissions();
            }
        }

        Button connectButton = findViewById(R.id.connect_btn);
        connectButton.setEnabled(false);
        TextInputEditText inputField = findViewById(R.id.login_input);

        bluetoothServiceConnection = new BluetoothServiceConnection(this);

        enableBluetoothLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
            if (result.getResultCode() == RESULT_OK) {
                startBluetoothDiscovery();
            } else {
                Toast.makeText(LoginActivity.this, "Bluetooth is required to connect", Toast.LENGTH_SHORT).show();
            }
        });

        inputField.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(final CharSequence s, final int start, final int count, final int after) {
            }

            @Override
            public void onTextChanged(final CharSequence s, final int start, final int before, final int count) {
                connectButton.setEnabled(s.length() == Constants.MAX_INPUT_LENGTH && s.toString().matches("\\d+"));
            }

            @Override
            public void afterTextChanged(final Editable s) {
                if (s.length() > Constants.MAX_INPUT_LENGTH) {
                    s.delete(Constants.MAX_INPUT_LENGTH, s.length());
                }
            }
        });

        inputField.setOnKeyListener((v, keyCode, event) -> {
            if (event.getAction() == KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_ENTER) {
                connectButton.performClick();
                return true;
            } else {
                return false;
            }
        });

        connectButton.setOnClickListener(v -> {
            userInput = Objects.requireNonNull(inputField.getText()).toString();
            if (!bluetoothLeService.isBluetoothOn()) {
                Intent enableBluetoothIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                enableBluetoothLauncher.launch(enableBluetoothIntent);
            } else {
                startBluetoothDiscovery();
            }
        });

//        connectButton.setOnClickListener(v -> {
//            Intent intent = new Intent(this, GameSelectionActivity.class);
//            startActivity(intent);
//        });

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
            popupText.setText(R.string.info_login);

            Button closePopupButton = helpPopupView.findViewById(R.id.popup_close_button);
            closePopupButton.setOnClickListener(v -> helpPopupWindow.dismiss());
        });
    }

    /**
     * This method registers the device found receiver and binds the Bluetooth service when the activity starts.
     */
    @Override
    protected void onStart() {
        super.onStart();
        IntentFilter filter = new IntentFilter(Constants.DEVICE_FOUND_ACTION);
        LocalBroadcastManager.getInstance(this).registerReceiver(deviceFoundReceiver, filter);

        Intent intent = new Intent(this, BluetoothLeService.class);
        bindService(intent, bluetoothServiceConnection, Context.BIND_AUTO_CREATE);
    }

    /**
     * This method unregisters the device found receiver and unbinds the Bluetooth service when the activity is destroyed.
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();

        // Unregister Broadcast Receiver properly
        LocalBroadcastManager.getInstance(this).unregisterReceiver(deviceFoundReceiver);

        // Unbind Bluetooth Service safely
        if (isBound) {
            unbindService(bluetoothServiceConnection);
            isBound = false;
        }
    }

    /**
     * This method starts Bluetooth device discovery by calling BluetoothLeService.
     */
    private void startBluetoothDiscovery() {
        if (isBound) {
            bluetoothLeService.startDiscovery();
        }
    }

    /**
     * This method handles permission results for Bluetooth permissions.
     *
     * @param requestCode  The request code.
     * @param permissions  The requested permissions.
     * @param grantResults The grant results for each permission.
     */
    @Override
    public void onRequestPermissionsResult(final int requestCode, @NonNull final String[] permissions, @NonNull final int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == Constants.REQUEST_BLUETOOTH_PERMISSIONS) {
            if (grantResults.length > 0 && areAllPermissionsGranted(grantResults)) {
                startBluetoothDiscovery();
            } else {
                Toast.makeText(this, "Permissions required for Bluetooth functionality", Toast.LENGTH_SHORT).show();
            }
        }
    }

    /**
     * This method checks if all permissions were granted.
     *
     * @param grantResults The array of grant results for each permission.
     * @return true if all permissions are granted, otherwise false.
     */
    private boolean areAllPermissionsGranted(final int[] grantResults) {
        return Arrays.stream(grantResults).allMatch(grantResult -> grantResult == PackageManager.PERMISSION_GRANTED);
    }

    /**
     * This method requests Bluetooth-related permissions if they haven't been granted.
     */
    private void requestBluetoothPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            ActivityCompat.requestPermissions(this,
                    new String[]{
                            android.Manifest.permission.BLUETOOTH_SCAN,
                            android.Manifest.permission.BLUETOOTH_CONNECT
                    },
                    Constants.REQUEST_BLUETOOTH_PERMISSIONS);
        }
    }

    /**
     * This method is called when the Bluetooth service is connected. This method binds the Bluetooth service and sets the service connection to true.
     *
     * @param service The connected BluetoothLeService.
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
     * This method checks if the specified permissions are granted.
     *
     * @param permissions The permissions to check.
     * @return True or false if all permissions are granted.
     */
    private boolean hasPermissions(final String... permissions) {
        return Stream.of(permissions).allMatch(permission -> ActivityCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED);
    }
}
