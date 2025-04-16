package com.soundleap.app;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityManager;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;

import androidx.activity.EdgeToEdge;
import androidx.activity.OnBackPressedCallback;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.soundleap.app.bluetooth.BluetoothLeService;
import com.soundleap.app.bluetooth.BluetoothServiceConnection;
import com.soundleap.app.model.Game;
import com.soundleap.app.util.Constants;
import com.soundleap.app.util.GameListAdapter;

import java.io.IOException;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;

/**
 * This activity handles the user interface for selecting a game from a list of available games.
 * It also manages the Bluetooth connection and interactions with the game settings configuration.
 */
public class GameSelectionActivity extends AppCompatActivity implements BluetoothServiceConnection.BluetoothServiceConnectionListener {

    /**
     * The AccessibilityManager used to manage accessibility events.
     */
    private AccessibilityManager accessibilityManager;

    /**
     * A list of games fetched from the server.
     */
    private final List<Game> games = new ArrayList<>();

    /**
     * Adapter for displaying games in the ListView.
     */
    private ArrayAdapter<Game> adapter;

    /**
     * The UID of the currently selected game.
     */
    private String selectedGameUid = null;

    /**
     * OkHttpClient used for making network requests.
     */
    private final OkHttpClient httpClient = new OkHttpClient();

    /**
     * Gson instance for parsing JSON responses.
     */
    private final Gson gson = new Gson();

    /**
     * The BluetoothLeService instance for managing Bluetooth connections.
     */
    private BluetoothLeService bluetoothLeService;

    /**
     * Flag indicating whether the Bluetooth service is bound.
     */
    private boolean isBound = false;

    /**
     * Executor service for running network requests in a background thread.
     */
    private final ExecutorService executorService = Executors.newSingleThreadExecutor();

    /**
     * BluetoothServiceConnection used to bind and interact with the BluetoothLeService.
     */
    private BluetoothServiceConnection bluetoothServiceConnection;

    /**
     * This method is called when the activity is created.
     * It sets up UI elements, initializes Bluetooth service, and starts fetching game data from the server.
     *
     * @param savedInstanceState The saved instance state of the activity, if available.
     */
    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_game_selection);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        accessibilityManager = (AccessibilityManager) getSystemService(Context.ACCESSIBILITY_SERVICE);

        bluetoothServiceConnection = new BluetoothServiceConnection(this);

        ListView listView = findViewById(R.id.game_list);
        adapter = new GameListAdapter(this, games);
        listView.setAdapter(adapter);

        TextView selectedGame = findViewById(R.id.selectedGame);

        Button backButton = findViewById(R.id.back_btn);
        backButton.setOnClickListener(v -> {
            bluetoothLeService.disconnect();
            finish();
        });

        Button okButton = findViewById(R.id.ok_btn);
        okButton.setEnabled(false);
        okButton.setOnClickListener(v -> {
            Intent intent = new Intent(this, GameConfigActivity.class);
            intent.putExtra("uid", selectedGameUid);
            startActivity(intent);
        });

        listView.setOnItemClickListener((parent, view, position, id) -> {
            Game newSelectedGame = games.get(position);
            if (Objects.equals(selectedGameUid, newSelectedGame.getUid())) {
                selectedGameUid = null;
                selectedGame.setText(R.string.default_selected_text);

                if (accessibilityManager.isEnabled()) {
                    AccessibilityEvent event = AccessibilityEvent.obtain(AccessibilityEvent.TYPE_ANNOUNCEMENT);
                    event.getText().add("Disselected game");
                    event.setClassName(getClass().getName());
                    event.setPackageName(getPackageName());
                    accessibilityManager.sendAccessibilityEvent(event);
                }

                okButton.setEnabled(false);
            } else {
                selectedGameUid = newSelectedGame.getUid();
                TextView nameOfGame = view.findViewById(R.id.game_name);
                String text = String.format("%s %s", getString(R.string.default_selected_text), nameOfGame.getText());

                selectedGame.setText(text);

                if (accessibilityManager.isEnabled()) {
                    AccessibilityEvent event = AccessibilityEvent.obtain(AccessibilityEvent.TYPE_ANNOUNCEMENT);
                    event.getText().add("Selected " + nameOfGame.getText());
                    event.setClassName(getClass().getName());
                    event.setPackageName(getPackageName());
                    accessibilityManager.sendAccessibilityEvent(event);
                }
                okButton.setEnabled(true);
            }
        });


        getOnBackPressedDispatcher().addCallback(this, new OnBackPressedCallback(true) {
            @Override
            public void handleOnBackPressed() {
                bluetoothLeService.disconnect();
                setEnabled(false);
                getOnBackPressedDispatcher().onBackPressed();
            }
        });

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
            popupText.setText(R.string.info_game_selection);

            Button closePopupButton = helpPopupView.findViewById(R.id.popup_close_button);
            closePopupButton.setOnClickListener(v -> helpPopupWindow.dismiss());
        });

        fetchGamesFromServer();
    }

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
     * This method is called when the activity is destroyed.
     * It ensures Bluetooth is disconnected, unbinds the Bluetooth service and shuts down the ExecutorService.
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();

        // Ensure Bluetooth is disconnected before unbinding
        if (bluetoothLeService != null) {
            bluetoothLeService.disconnect();
        }

        // Unbind Bluetooth Service safely
        if (isBound) {
            unbindService(bluetoothServiceConnection);
            isBound = false;
        }

        // Shut down ExecutorService to prevent memory leaks
        if (!executorService.isShutdown()) {
            executorService.shutdown();
        }
    }

    /**
     * This method fetches the list of games from the server and updates the UI with the game data.
     */
    private void fetchGamesFromServer() {
        executorService.execute(() -> {
            try {
                Request request = new Request.Builder()
                        .url(Constants.API_BASE_URL)
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

                    Type listType = new TypeToken<List<Game>>() {
                    }.getType();
                    List<Game> fetchedGames = gson.fromJson(jsonString, listType);

                    runOnUiThread(() -> {
                        games.clear();
                        games.addAll(fetchedGames);


                        adapter.clear();
                        adapter.addAll(fetchedGames);
                        adapter.notifyDataSetChanged();
                    });
                }
            } catch (IOException e) {
                Log.e("GameSelectionActivity", "Error fetching games: " + e.getMessage());
            }
        });
    }

    /**
     * This method is called when the Bluetooth service is connected.
     *
     * @param service The connected BluetoothLeService instance.
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
}
