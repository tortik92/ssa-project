package com.soundleap.app.model;

import android.content.Context;

import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.RequiredArgsConstructor;

/**
 * This class is used for representing user preferences within the application.
 */
@RequiredArgsConstructor
@NoArgsConstructor(force = true)
@Getter
public abstract class Preference {

    /**
     * Android context, typically used for rendering views or accessing resources.
     */
    private final Context context;

    /**
     * Name or identifier of the preference, used as a label or key (e.g., "sound", "volume").
     */
    private final String name;

    /**
     * Type of the preference ({@link BooleanPreference}, {@link NumberPreference}, {@link TextPreference}, {@link ListPreference}).
     */
    private final String type;
}
