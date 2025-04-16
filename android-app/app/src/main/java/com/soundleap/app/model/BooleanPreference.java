package com.soundleap.app.model;

import android.content.Context;
import android.graphics.Typeface;
import android.view.ContextThemeWrapper;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.soundleap.app.R;

import lombok.Getter;

/**
 * This class is a {@link Preference} implementation representing a boolean option.
 * Displays a toggle button labeled accordingly to reflect true/false values.
 */
@Getter
@SuppressWarnings("MagicNumber")
public class BooleanPreference extends Preference {

    /**
     * The default boolean value of the preference.
     */
    private final boolean defaultValue;

    /**
     * The current boolean value of the preference.
     */
    private boolean value;

    /**
     * This method constructs a new {@code BooleanPreference}.
     *
     * @param context      the Android context
     * @param name         the name of the preference
     * @param type         the type of the preference (e.g., "boolean")
     * @param defaultValue the default value of the preference
     * @noinspection checkstyle:HiddenField
     */
    @SuppressWarnings("checkstyle:hiddenfield")
    public BooleanPreference(final Context context, final String name, final String type, final boolean defaultValue) {
        super(context, name, type);
        this.defaultValue = defaultValue;
        this.value = defaultValue;
    }

    /**
     * This method builds and returns a {@link RelativeLayout} containing the preference UI.
     * The layout includes a bold title and a toggle button for switching the value.
     *
     * @return a RelativeLayout with the preference name and a toggle button
     */
    public RelativeLayout getLinearLayout() {
        // Create a RelativeLayout
        RelativeLayout layout = new RelativeLayout(getContext());
        layout.setLayoutParams(new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.WRAP_CONTENT
        ));

        // Create a TextView for the preference name
        TextView preferenceName = new TextView(getContext());
        preferenceName.setId(View.generateViewId()); // Set a unique ID for the TextView
        preferenceName.setText(getName());
        preferenceName.setTextSize(18f);
        preferenceName.setTypeface(null, Typeface.BOLD); // Set text to bold
        preferenceName.setGravity(Gravity.CENTER);

        // Set layout parameters for the TextView
        RelativeLayout.LayoutParams nameParams = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.WRAP_CONTENT,
                RelativeLayout.LayoutParams.WRAP_CONTENT
        );
        nameParams.addRule(RelativeLayout.CENTER_HORIZONTAL); // Center horizontally
        nameParams.topMargin = 20; // Top margin in pixels
        preferenceName.setLayoutParams(nameParams);

        // Create a Button to toggle the boolean value
        Button toggleButton = new Button(new ContextThemeWrapper(this.getContext(), R.style.CustomButtonStyle), null, 0);
        toggleButton.setId(View.generateViewId()); // Set a unique ID for the Button
        toggleButton.setText(value ? "Ja" : "Nein");
        toggleButton.setOnClickListener(v -> {
            value = !value;
            toggleButton.setText(value ? "Ja" : "Nein");
        });

        // Set layout parameters for the Button
        RelativeLayout.LayoutParams buttonParams = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.WRAP_CONTENT,
                RelativeLayout.LayoutParams.WRAP_CONTENT
        );
        buttonParams.addRule(RelativeLayout.BELOW, preferenceName.getId()); // Position below the TextView
        buttonParams.addRule(RelativeLayout.CENTER_HORIZONTAL); // Center horizontally
        buttonParams.topMargin = 10; // Space between TextView and Button
        toggleButton.setLayoutParams(buttonParams);

        // Add the TextView and Button to the RelativeLayout
        layout.addView(preferenceName);
        layout.addView(toggleButton);

        return layout;
    }
}
