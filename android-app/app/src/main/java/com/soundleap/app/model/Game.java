package com.soundleap.app.model;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import androidx.annotation.NonNull;

import java.io.IOException;
import java.util.List;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.RequiredArgsConstructor;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.ResponseBody;

/**
 * This class represents a game with various properties, such as its name, description, version, unique ID (UID) and a list of preferences associated with the game.
 */
@Getter
@AllArgsConstructor
@RequiredArgsConstructor
public class Game {

    /**
     * The name of the game.
     */
    private final String name;

    /**
     * A brief description of the game.
     */
    private final String description;

    /**
     * The version of the game.
     */
    private final String version;

    /**
     * A unique identifier for the game.
     */
    private final String uid;

    /**
     * A list of preferences associated with the game.
     */
    private List<Preference> preferences;

    /**
     * This method converts the game instance to a formatted string.
     * The string includes the game's name, UID, and a list of preferences in a key-value format.
     * Each preference is processed based on its type (e.g., `ListPreference`, `BooleanPreference`, etc.).
     *
     * @return A string representing the game and its preferences in a readable format.
     */
    @NonNull
    @Override
    public String toString() {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(String.format("%s=%s;", "name", this.name));
        stringBuilder.append(String.format("%s=%s;", "uid", this.uid));
        this.preferences.forEach(preference -> {
            if (preference instanceof ListPreference) {
                stringBuilder.append(String.format("%s=%s;", preference.getName(), ((ListPreference) preference).getElement()));
            } else if (preference instanceof BooleanPreference) {
                stringBuilder.append(String.format("%s=%s;", preference.getName(), ((BooleanPreference) preference).isValue()));
            } else if (preference instanceof NumberPreference) {
                stringBuilder.append(String.format("%s=%s;", preference.getName(), ((NumberPreference) preference).getValue()));
            } else if (preference instanceof TextPreference) {
                stringBuilder.append(String.format("%s=%s;", preference.getName(), ((TextPreference) preference).getValue()));
            }
        });
        stringBuilder.append("\n");
        return stringBuilder.toString();
    }

    /**
     * Retrieves the game code associated with this game's UID..
     *
     * @param context  the {@link Context} used to access application assets; must not be null.
     * @param callback The method which is called after the end of the funtcion.
     *                 an empty string if the file is empty, or {@code null} if an {@link IOException} occurs.
     */
    public void getGameCode(final Context context, final GameCodeCallback callback) {
        new Thread(() -> {
            OkHttpClient client = new OkHttpClient();
            String url = "https://www.cakelab.co.nl/ssa-server/gamecode.php?uid=" + uid;

            Request request = new Request.Builder()
                    .url(url)
                    .build();

            try (Response response = client.newCall(request).execute()) {
                if (!response.isSuccessful()) {
                    Log.e("getGameCode", "Unexpected code " + response);
                    // Make sure callback is executed on the main thread
                    new Handler(Looper.getMainLooper()).post(() -> callback.onGameCodeReceived(null));
                    return;
                }

                ResponseBody body = response.body();
                if (body == null) {
                    Log.e("getGameCode", "Response body is null");
                    // Make sure callback is executed on the main thread
                    new Handler(Looper.getMainLooper()).post(() -> callback.onGameCodeReceived(null));
                    return;
                }

                String content = body.string();
                content = content.replaceAll("\n", "").replaceAll("\t", "");
                content += "EOF";
                Log.d("getGameCode", content);

                // Make sure callback is executed on the main thread
                String finalContent = content;
                Log.d("getGameCode", "FINAL CONTENT " + finalContent);
                new Handler(Looper.getMainLooper()).post(() -> callback.onGameCodeReceived(finalContent));
            } catch (IOException e) {
                e.printStackTrace();
                // Make sure callback is executed on the main thread
                new Handler(Looper.getMainLooper()).post(() -> callback.onGameCodeReceived(null));
            }
        }).start();
    }


    public interface GameCodeCallback {
        void onGameCodeReceived(String gameCode);
    }
}
