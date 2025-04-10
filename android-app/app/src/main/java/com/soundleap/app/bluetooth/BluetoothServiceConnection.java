package com.soundleap.app.bluetooth;

import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.IBinder;

import lombok.RequiredArgsConstructor;

/**
 * This class implements a custom connection handler for binding to the {@link BluetoothLeService}.
 * It facilitates the connection and disconnection of the service by providing callback methods to
 * notify the caller about the service connection status.
 */
@RequiredArgsConstructor
public class BluetoothServiceConnection implements ServiceConnection {

    /**
     * Listener for receiving notifications about the Bluetooth service connection status.
     */
    private final BluetoothServiceConnectionListener listener;

    /**
     * Interface definition for a listener that receives the service connection events.
     * Provides callback methods for both when the service is connected and when it is disconnected.
     */
    public interface BluetoothServiceConnectionListener {

        /**
         * This method is called when the {@link BluetoothLeService} is successfully connected.
         *
         * @param bluetoothLeService The connected {@link BluetoothLeService} instance.
         */
        void onServiceConnected(BluetoothLeService bluetoothLeService);

        /**
         * This method is called when the {@link BluetoothLeService} is disconnected.
         */
        void onServiceDisconnected();
    }

    /**
     * This method is called when the service is successfully connected.
     * This method retrieves the service binder and notifies the listener that the service is connected.
     *
     * @param name    The name of the connected service.
     * @param service The {@link IBinder} object that allows interaction with the service.
     */
    @Override
    public void onServiceConnected(final ComponentName name, final IBinder service) {
        BluetoothLeService.LocalBinder binder = (BluetoothLeService.LocalBinder) service;
        listener.onServiceConnected(binder.getService());
    }

    /**
     * This method is called when the service is disconnected.
     * This method notifies the listener that the service has been disconnected.
     *
     * @param name The name of the disconnected service.
     */
    @Override
    public void onServiceDisconnected(final ComponentName name) {
        listener.onServiceDisconnected();
    }
}
