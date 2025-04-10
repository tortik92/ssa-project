package com.soundleap.app.util;

/**
 * This class holds constant values used across the application.
 * This class cannot be instantiated.
 */
public final class Constants {

    /**
     * This method is a private constructor to prevent instantiation.
     */
    private Constants() {
        throw new UnsupportedOperationException("This is a utility class and cannot be instantiated!");
    }

    /**
     * Maximum allowed input length for user fields.
     */
    public static final int MAX_INPUT_LENGTH = 6;

    /**
     * Base URL for API calls.
     */
    public static final String API_BASE_URL = "https://www.cakelab.co.nl/ssa-server";

    /**
     * Prefix used for identifying SoundLeap devices.
     */
    public static final String DEVICE_PREFIX = "SL-";

    /**
     * Request code for Bluetooth permission dialogs.
     */
    public static final int REQUEST_BLUETOOTH_PERMISSIONS = 101;

    /**
     * Broadcast action when a device is discovered.
     */
    public static final String DEVICE_FOUND_ACTION = "com.soundleap.app.DEVICE_FOUND";

    /**
     * Broadcast action for receiving game status data.
     */
    public static final String GAME_STATUS_DATA_ACTION = "com.soundleap.app.GAME_STATUS_DATA";

    /**
     * Broadcast action for receiving game status data.
     */
    public static final String GAME_ENDED_ACTION = "com.soundleap.app.GAME_ENDED";

    /**
     * ByteCode used to cancel an active game.
     */
    public static final byte BYTECODE_CANCEL_GAME = (byte) 0xFF;

    /**
     * ByteCode used to quit a finished game.
     */
    public static final byte BYTECODE_GAME_ENDED = (byte) 0xEE;

    /**
     * ByteCode used to start a game.
     */
    public static final byte BYTECODE_START_GAME = (byte) 0x14;

    //TODO Add constants for:
    // - ByteCode which is used to start any game
    // - ByteCode which is used to pause a game
}
