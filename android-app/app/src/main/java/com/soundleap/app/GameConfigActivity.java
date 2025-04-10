package com.soundleap.app;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
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

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonParseException;
import com.soundleap.app.bluetooth.BluetoothLeService;
import com.soundleap.app.model.BooleanPreference;
import com.soundleap.app.model.Game;
import com.soundleap.app.model.ListPreference;
import com.soundleap.app.model.NumberPreference;
import com.soundleap.app.model.Preference;
import com.soundleap.app.model.TextPreference;
import com.soundleap.app.util.Constants;
import com.soundleap.app.util.PreferenceDeserializer;

import org.apache.http.client.utils.URIBuilder;

import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;

/**
 * This activity is responsible for displaying and configuring game settings.
 * It allows users to adjust preferences, such as difficulty level and number of players,
 * before proceeding to the pad configuration screen.
 */
public class GameConfigActivity extends AppCompatActivity {

    /**
     * Executor service for running tasks in the background.
     */
    private final ExecutorService executorService = Executors.newSingleThreadExecutor();

    /**
     * HTTP client used for making network requests.
     */
    private final OkHttpClient httpClient = new OkHttpClient();

    /**
     * Gson instance used for serializing and deserializing JSON.
     */
    private final Gson gson = createGson();

    /**
     * The game object representing the current game settings.
     */
    private Game game;

    /**
     * Layout container that holds all the game configuration preferences.
     */
    private LinearLayout parentLayout;

    /**
     * Flag indicating whether the Bluetooth service is bound.
     */
    private boolean isBound = false;

    /**
     * Service connection used to bind to BluetoothLeService.
     */
    private final ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(final ComponentName name, final IBinder service) {
            BluetoothLeService.LocalBinder binder = (BluetoothLeService.LocalBinder) service;
            BluetoothLeService bluetoothLeService = binder.getService();
            isBound = true;
        }

        @Override
        public void onServiceDisconnected(final ComponentName name) {
            isBound = false;
        }
    };

    /**
     * This method is called when the activity is started. It binds the Bluetooth service.
     */
    @Override
    protected void onStart() {
        super.onStart();

        Intent intent = new Intent(this, BluetoothLeService.class);
        bindService(intent, serviceConnection, Context.BIND_AUTO_CREATE);
    }

    /**
     * This method is called when the activity is destroyed. It unbinds the Bluetooth service if bound.
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (isBound) {
            unbindService(serviceConnection);
        }
    }

    /**
     * This method is called when the activity is created. It initializes the UI elements and sets up button listeners.
     * Fetches game configurations based on the provided UID.
     *
     * @param savedInstanceState The saved instance state of the activity, if available.
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_game_config);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        parentLayout = findViewById(R.id.game_configs);

        Button okButton = findViewById(R.id.ok_btn);
        okButton.setOnClickListener(v -> {
            Intent intent = new Intent(this, PadsConfigActivity.class);
            intent.putExtra("selectedGameName", game.getName());
            intent.putExtra("gameConfigs", game.toString());

            game.getGameCode(this, receivedGameCode -> {
                Log.d("getGameCode", "received DATA: " + receivedGameCode);
                if (receivedGameCode != null) {
                    intent.putExtra("gameCode", receivedGameCode);
                } else {
                    Log.e("getGameCode", "Failed to receive game code");
                }

                startActivity(intent);
                finish();
            });

        });

        Button backButton = findViewById(R.id.back_btn);
        backButton.setOnClickListener(v -> finish());

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
            popupText.setText(R.string.info_game_config);

            Button closePopupButton = helpPopupView.findViewById(R.id.popup_close_button);
            closePopupButton.setOnClickListener(v -> helpPopupWindow.dismiss());
        });

        String uid = getIntent().getStringExtra("uid");
        if (uid != null && !uid.isEmpty()) {
            fetchGameConfigurations(uid);
        } else {
            Log.e("GameConfigActivity", "UID is null, finishing activity.");
            finish();
        }
    }

    /**
     * This method creates a new Gson instance with a custom deserializer for preferences.
     *
     * @return A new Gson instance.
     */
    private Gson createGson() {
        return new GsonBuilder()
                .registerTypeAdapter(Preference.class, new PreferenceDeserializer(this))
                .create();
    }

    /**
     * This method fetches game configuration settings from a remote API using the provided UID.
     * The configurations are then parsed and displayed in the UI.
     *
     * @param uid The unique identifier for the game configuration.
     */
    private void fetchGameConfigurations(final String uid) {
        executorService.execute(() -> {
            try {
                URIBuilder uriBuilder = new URIBuilder(Constants.API_BASE_URL);
                uriBuilder.addParameter("uid", uid);

                Request request = new Request.Builder()
                        .url(uriBuilder.build().toString())
                        .build();

                try (Response response = httpClient.newCall(request).execute()) {
                    if (!response.isSuccessful()) {
                        throw new IOException("Unexpected response code: " + response.code());
                    }

                    ResponseBody body = response.body();
                    if (body == null) {
                        Log.e("GameSelectionActivity", "Response body is null");
                        return;
                    }

                    String jsonString = body.string();
                    if (jsonString.isEmpty()) {
                        Log.e("GameSelectionActivity", "Received empty response body");
                        return;
                    }

                    game = gson.fromJson(jsonString, Game.class);

                    runOnUiThread(() -> {
                        if (game.getPreferences() != null) {
                            for (Preference preference : game.getPreferences()) {
                                LinearLayout linearLayout = null;
                                switch (preference.getType()) {
                                    case "bool":
                                        BooleanPreference boolPreference = (BooleanPreference) preference;
                                        parentLayout.addView(boolPreference.getLinearLayout());
                                        break;
                                    case "number":
                                        NumberPreference numberPreference = (NumberPreference) preference;
                                        linearLayout = numberPreference.getLinearLayout();
                                        break;
                                    case "list":
                                        ListPreference listPreference = (ListPreference) preference;
                                        linearLayout = listPreference.getLinearLayout();
                                        break;
                                    case "text":
                                        TextPreference textPreference = (TextPreference) preference;
                                        linearLayout = textPreference.getLinearLayout();
                                        break;
                                    default:
                                        throw new UnsupportedOperationException("This type of preference is not supported!");
                                }
                                if (linearLayout != null) {
                                    parentLayout.addView(linearLayout);
                                }
                            }
                        }
                    });
                }
            } catch (IOException e) {
                Log.e("GameConfigActivity", "Error fetching games: " + e.getMessage(), e);
            } catch (JsonParseException e) {
                Log.e("GameConfigActivity", "Error parsing JSON: " + e.getMessage(), e);
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        });
    }
}
