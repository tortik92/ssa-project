package com.soundleap.app.model;

import android.content.Context;
import android.graphics.Typeface;
import android.view.ContextThemeWrapper;
import android.view.Gravity;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.soundleap.app.R;

import lombok.Getter;

/**
 * This class is a {@link Preference} implementation that allows users to select a numeric value
 * within a defined range using increment and decrement buttons.
 */
@Getter
@SuppressWarnings("MagicNumber")
public class NumberPreference extends Preference {

    /**
     * The default numeric value.
     */
    private final int defaultValue;

    /**
     * The minimum allowed value.
     */
    private final int min;

    /**
     * The maximum allowed value.
     */
    private final int max;

    /**
     * The currently selected value.
     */
    private int value;

    /**
     * This method constructs a {@code NumberPreference}.
     *
     * @param context      the Android context
     * @param name         the name or key of the preference
     * @param type         the type of the preference (e.g., "number")
     * @param defaultValue the initial default value
     * @param min          the minimum value allowed
     * @param max          the maximum value allowed
     */
    @SuppressWarnings("HiddenField")
    public NumberPreference(final Context context, final String name, final String type, final int defaultValue, final int min, final int max) {
        super(context, name, type);
        this.defaultValue = defaultValue;
        this.min = min;
        this.max = max;
        this.value = defaultValue;
    }

    /**
     * This method builds and returns a {@link LinearLayout} UI for this preference.
     * The layout includes a title, the current value and +/- buttons for changing it.
     *
     * @return a vertical LinearLayout containing this preference's UI
     */
    public LinearLayout getLinearLayout() {
        // Create a TextView for the preference name
        TextView preferenceName = new TextView(getContext());
        preferenceName.setText(getName());
        preferenceName.setTextSize(18f);
        preferenceName.setTypeface(null, Typeface.BOLD);

        // Set layout parameters for the preference name
        LinearLayout.LayoutParams nameParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        nameParams.gravity = Gravity.CENTER;
        nameParams.topMargin = 8;
        preferenceName.setLayoutParams(nameParams);
        preferenceName.setGravity(Gravity.CENTER);

        // Create a TextView to display the current value
        TextView textView = new TextView(getContext());
        textView.setText(String.valueOf(value));

        // Set layout parameters for the value TextView
        LinearLayout.LayoutParams textViewParams = new LinearLayout.LayoutParams(
                0, // Width is 0 so weight takes precedence
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        textViewParams.weight = 2f; // Give more weight to the text view
        textViewParams.gravity = Gravity.CENTER;
        textView.setLayoutParams(textViewParams);
        textView.setGravity(Gravity.CENTER);

        // Create the increment button
        Button incrementButton = new Button(new ContextThemeWrapper(this.getContext(), R.style.CustomButtonStyle), null, 0);

        incrementButton.setText("+");
        incrementButton.setOnClickListener(v -> {
            if (value < max) {
                value++;
                textView.setText(String.valueOf(value));
            }
        });

        // Set layout parameters for the increment button
        LinearLayout.LayoutParams incrementButtonParams = new LinearLayout.LayoutParams(
                0,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        incrementButtonParams.weight = 1f; // Less weight than the text view
        incrementButton.setLayoutParams(incrementButtonParams);

        // Create the decrement button
        Button decrementButton = new Button(new ContextThemeWrapper(this.getContext(), R.style.CustomButtonStyle), null, 0);
        decrementButton.setText("-");
        decrementButton.setOnClickListener(v -> {
            if (value > min) {
                value--;
                textView.setText(String.valueOf(value));
            }
        });

        // Set layout parameters for the decrement button
        LinearLayout.LayoutParams decrementButtonParams = new LinearLayout.LayoutParams(
                0,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        decrementButtonParams.weight = 1f; // Less weight than the text view
        decrementButton.setLayoutParams(decrementButtonParams);

        // Create a horizontal LinearLayout for the buttons and text view
        LinearLayout horizontalLayout = new LinearLayout(getContext());
        horizontalLayout.setOrientation(LinearLayout.HORIZONTAL);
        horizontalLayout.addView(decrementButton);
        horizontalLayout.addView(textView);
        horizontalLayout.addView(incrementButton);

        // Create the main vertical LinearLayout
        LinearLayout mainLayout = new LinearLayout(getContext());
        mainLayout.setOrientation(LinearLayout.VERTICAL);
        mainLayout.addView(preferenceName);
        mainLayout.addView(horizontalLayout);

        return mainLayout;
    }

    /**
     * This method sets the preference value if within the allowed range.
     *
     * @param value the new value to set
     */
    @SuppressWarnings("HiddenField")
    public void setValue(final int value) {
        if (value >= min && value <= max) {
            this.value = value;
        }
    }
}
