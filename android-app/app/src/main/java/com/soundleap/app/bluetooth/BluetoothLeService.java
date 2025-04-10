package com.soundleap.app.bluetooth;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.BluetoothLeScanner;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Binder;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.google.gson.Gson;
import com.soundleap.app.LoginActivity;
import com.soundleap.app.util.Constants;

import java.nio.charset.StandardCharsets;
import java.util.Objects;
import java.util.stream.Stream;

/**
 * This class represents a {@link Service} that manages Bluetooth Low Energy (BLE) connections.
 * Handles device discovery, connections, service/characteristic interactions, and data transmission.
 */
@SuppressLint("MissingPermission")
@SuppressWarnings("MagicNumber")
public class BluetoothLeService extends Service {

    /**
     * Binder given to clients for accessing service methods.
     */
    private final IBinder binder = new LocalBinder();

    /**
     * GATT connection object. Used for interacting with BLE devices.
     */
    private BluetoothGatt bluetoothGatt;

    /**
     * Bluetooth adapter for managing BLE connections.
     */
    private BluetoothAdapter bluetoothAdapter;

    /**
     * Device name to search for during discovery (optional usage).
     */
    private String deviceNameToSearchFor;

    /**
     * Flag indicating whether a device has been found during discovery.
     */
    private boolean deviceFound = false;

    /**
     * Gson instance for JSON serialization/deserialization (optional usage).
     */
    private final Gson gson = new Gson();

    /**
     * Characteristic used for writing data to the connected BLE device.
     */
    private BluetoothGattCharacteristic writableCharacteristic;

    /**
     * This class is used for returning an instance of the service.
     */
    public class LocalBinder extends Binder {

        /**
         * Method returns this service instance.
         *
         * @return the {@link BluetoothLeService} instance
         */
        public BluetoothLeService getService() {
            return BluetoothLeService.this;
        }
    }

    /**
     * This method binds the service to an activity or component.
     *
     * @param intent The intent that is used to bind the service.
     * @return A binder object that clients can use to interact with the service.
     */
    @Nullable
    @Override
    public IBinder onBind(final Intent intent) {
        return binder;
    }

    /**
     * This method is called when the service is created. It initializes the Bluetooth manager and adapter, sets up Bluetooth LE scanning and registers a receiver for discovered devices.
     */
    @Override
    public void onCreate() {
        super.onCreate();

        BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();
        BluetoothLeScanner bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothDevice.ACTION_FOUND);
        registerReceiver(deviceDiscoveredReceiver, filter);
    }

    /**
     * This method is called when the service is destroyed. It stops device discovery, closes the GATT connection and unregisters the device discovery receiver.
     */
    @Override
    public void onDestroy() {
        super.onDestroy();
        stopDiscovery();
        if (bluetoothGatt != null) {
            bluetoothGatt.close();
            bluetoothGatt = null;
        }
        unregisterReceiver(deviceDiscoveredReceiver);
    }

    /**
     * This method starts the Bluetooth device discovery process if Bluetooth is enabled.
     */
    public void startDiscovery() {
        if (bluetoothAdapter.isEnabled()) {
            bluetoothAdapter.startDiscovery();
        }
    }

    /**
     * This method stops the Bluetooth device discovery process if it is in progress.
     */
    private void stopDiscovery() {
        if (bluetoothAdapter.isDiscovering()) {
            bluetoothAdapter.cancelDiscovery();
        }
    }

    /**
     * This method initiates a connection to the specified Bluetooth device.
     *
     * @param device The Bluetooth device to connect to.
     */
    public void connect(final BluetoothDevice device) {
        if (hasPermissions(Manifest.permission.BLUETOOTH_CONNECT)) {
            if (bluetoothGatt != null) {
                bluetoothGatt.disconnect();
                bluetoothGatt.close();
            }
            bluetoothGatt = device.connectGatt(this, false, gattCallback);
            deviceFound = true;
            stopDiscovery();
        }
    }

    /**
     * This class serves as a {@link BroadcastReceiver} that listens for discovered Bluetooth devices and other related actions.
     */
    private final BroadcastReceiver deviceDiscoveredReceiver = new BroadcastReceiver() {

        /**
         * This method handles Bluetooth device discovery events. It listens for the discovery of new devices and sends a broadcast containing the discovered device's details.
         *
         * @param context The context in which the broadcast is received.
         * @param intent The intent containing the action and device information.
         */
        @Override
        public void onReceive(final Context context, final Intent intent) {
            switch (Objects.requireNonNull(intent.getAction())) {
                case BluetoothDevice.ACTION_FOUND:
                    if (hasPermissions(Manifest.permission.BLUETOOTH_CONNECT)) {
                        BluetoothDevice device = getBluetoothDeviceFromIntent(intent);
                        if (device != null) {
                            Intent foundIntent = new Intent(context, LoginActivity.class);
                            foundIntent.setAction(Constants.DEVICE_FOUND_ACTION);
                            foundIntent.putExtra("device", device); // Include the discovered device
                            LocalBroadcastManager.getInstance(BluetoothLeService.this).sendBroadcast(foundIntent);
                        }
                    }
                    break;
                case BluetoothAdapter.ACTION_DISCOVERY_FINISHED:
                    if (!deviceFound) {
                        Log.d("BluetoothLeService", "Device not found!");
                    }
                    break;
                default:
                    break;
            }
        }
    };

    /**
     * This class serves as a callback for Bluetooth GATT events, handling connection state changes, service discovery and characteristic updates.
     */
    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {

        /**
         * This method is invoked when the connection state of a Bluetooth GATT connection changes.
         * It handles state transitions such as connecting, connected, disconnecting, and disconnected.
         *
         * @param gatt The BluetoothGatt instance representing the current connection.
         * @param status The status of the connection operation. It can provide more information about errors or successes.
         * @param newState The new connection state as per BluetoothProfile constants (e.g., STATE_CONNECTED, STATE_DISCONNECTED).
         */
        @Override
        public void onConnectionStateChange(final BluetoothGatt gatt, final int status, final int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                gatt.discoverServices();
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                Toast.makeText(BluetoothLeService.this, "Disconnected from GATT server", Toast.LENGTH_SHORT).show();
            }
        }

        /**
         * This method is called when the GATT services are discovered.
         * This method iterates through the available services and their characteristics.
         * It checks if the characteristics have write or notify properties and sets up necessary notifications.
         *
         * @param gatt The BluetoothGatt instance that was used for service discovery.
         * @param status The result of the service discovery. GATT_SUCCESS indicates success.
         */
        @Override
        public void onServicesDiscovered(final BluetoothGatt gatt, final int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                for (BluetoothGattService service : gatt.getServices()) {
                    for (BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {
                        int properties = characteristic.getProperties();
                        if ((properties & BluetoothGattCharacteristic.PROPERTY_WRITE) != 0) {
                            writableCharacteristic = characteristic;
                        }
                        if ((properties & BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE) != 0) {
                            writableCharacteristic = characteristic;
                        }
                        if ((properties & BluetoothGattCharacteristic.PROPERTY_NOTIFY) != 0) {
                            gatt.setCharacteristicNotification(characteristic, true);
                        }
                    }
                }
            } else {
                Toast.makeText(BluetoothLeService.this, "Service discovery failed", Toast.LENGTH_SHORT).show();
            }
        }

        /**
         * This method is called when a Bluetooth characteristic's value has changed.
         * It reads the updated value of the characteristic and sends a broadcast with the data.
         *
         * @param gatt The BluetoothGatt instance representing the current connection.
         * @param characteristic The BluetoothGattCharacteristic that has changed its value.
         */
        @Override
        public void onCharacteristicChanged(final BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic) {
            int receivedData = Integer.parseInt(new String(characteristic.getValue()));
            int expectedData = Constants.BYTECODE_GAME_ENDED & 0xFF;

            Intent foundIntent = new Intent(BluetoothLeService.this, LoginActivity.class);

            Log.d("HEYYYYYYYYYYYYY", "receivedData: " + receivedData);
            Log.d("HEYYYYYYYYYYYYY", "expectedData: " + expectedData);

            if (receivedData == expectedData) {
                foundIntent.setAction(Constants.GAME_ENDED_ACTION);
                Log.d("HEYYYYYYYYYYYYY", "HALLO");
            } else {
                foundIntent.setAction(Constants.GAME_STATUS_DATA_ACTION);
                foundIntent.putExtra("gameStatus", receivedData);
                Log.d("HEYYYYYYYYYYYYY", "HALLOHALLO");
            }
            LocalBroadcastManager.getInstance(BluetoothLeService.this).sendBroadcast(foundIntent);

        }
    };

    /**
     * This method writes the provided byte data to the specified Bluetooth characteristic.
     *
     * @param gatt           The BluetoothGatt object representing the connection.
     * @param characteristic The characteristic to write to.
     * @param data           The byte data to write to the characteristic.
     */
    private void writeToCharacteristic(final BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic, final byte[] data) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            gatt.writeCharacteristic(characteristic, data, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
        } else {
            characteristic.setValue(data);
            characteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
            gatt.writeCharacteristic(characteristic);
        }
    }

    /**
     * This method extracts a Bluetooth device from the given Intent.
     *
     * @param intent The Intent containing the Bluetooth device.
     * @return The BluetoothDevice instance, or null if not found.
     */
    public BluetoothDevice getBluetoothDeviceFromIntent(final Intent intent) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            return intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE, BluetoothDevice.class);
        } else {
            return intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        }
    }

    /**
     * This method disconnects from the currently connected Bluetooth device.
     */
    public void disconnect() {
        if (bluetoothGatt != null) {
            bluetoothGatt.disconnect();
        }
    }

    /**
     * This method checks if Bluetooth is enabled on the device.
     *
     * @return True if Bluetooth is enabled, false otherwise.
     */
    public boolean isBluetoothOn() {
        return bluetoothAdapter.isEnabled();
    }

    /**
     * This method checks if the application has the required permissions.
     *
     * @param permissions The permissions to check.
     * @return True if all permissions are granted, false otherwise.
     */
    private boolean hasPermissions(final String... permissions) {
        return Stream.of(permissions)
                .allMatch(permission -> ActivityCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED);
    }

    /**
     * This method writes the given byte array to the writable Bluetooth characteristic.
     *
     * @param byteCode The byte array to write.
     */
    public void write(final byte[] byteCode) {
        if (bluetoothGatt != null && writableCharacteristic != null) {
            writeToCharacteristic(bluetoothGatt, writableCharacteristic, byteCode);
        } else {
            Log.e("BLE", "BluetoothGatt oder writableCharacteristic ist null!");
        }
    }

    /**
     * This method writes the given string message to the writable Bluetooth characteristic.
     *
     * @param message The string message to write.
     */
    public void write(final String message) {
        if (bluetoothGatt != null && writableCharacteristic != null) {
            byte[] data = message.getBytes(StandardCharsets.UTF_8);
            writeToCharacteristic(bluetoothGatt, writableCharacteristic, data);
        } else {
            Log.e("BLE", "BluetoothGatt oder writableCharacteristic ist null!");
        }
    }
}
