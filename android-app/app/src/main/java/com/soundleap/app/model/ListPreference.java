package com.soundleap.app.model;

import android.content.Context;
import android.graphics.Typeface;
import android.view.ContextThemeWrapper;
import android.view.Gravity;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.soundleap.app.R;

import java.util.List;

import lombok.Getter;

/**
 * This class is a {@link Preference} implementation that allows the user to select one item from a list.
 * It provides previous/next buttons to cycle through the available options.
 */
@Getter
@SuppressWarnings("MagicNumber")
public class ListPreference extends Preference {

    /**
     * The index of the default selected item in the list.
     */
    private final int defaultValue;

    /**
     * The list of string options that the user can select from.
     */
    private final List<String> list;

    /**
     * The index of the currently selected value.
     */
    private int value;

    /**
     * This class constructs a {@code ListPreference}.
     *
     * @param context      the Android context
     * @param name         the name or key of the preference
     * @param type         the type of the preference (typically "list")
     * @param defaultValue the index of the default value
     * @param list         the list of string options
     * @noinspection checkstyle:HiddenField
     */
    @SuppressWarnings("HiddenField")
    public ListPreference(final Context context, final String name, final String type, final int defaultValue, final List<String> list) {
        super(context, name, type);
        this.defaultValue = defaultValue;
        this.list = list;
        this.value = defaultValue;
    }

    /**
     * This method builds and returns a {@link LinearLayout} view for this preference,
     * including a label, the current value, and navigation buttons.
     *
     * @return a vertical LinearLayout containing the UI for this preference
     */
    public LinearLayout getLinearLayout() {
        // Create the TextView for the preference name
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

        // Create the TextView to display the current value
        TextView textView = new TextView(getContext());
        textView.setText(list.get(value));

        // Set layout parameters for the value TextView
        LinearLayout.LayoutParams textViewParams = new LinearLayout.LayoutParams(
                0, // Width is 0 so weight takes precedence
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        textViewParams.weight = 2f; // Give more weight to the text view
        textViewParams.gravity = Gravity.CENTER;
        textView.setLayoutParams(textViewParams);
        textView.setGravity(Gravity.CENTER);

        // Create the next button
        Button nextButton = new Button(new ContextThemeWrapper(this.getContext(), R.style.CustomButtonStyle), null, 0);
        nextButton.setText(">");
        nextButton.setOnClickListener(v -> {
            value = (value + 1) % list.size();
            textView.setText(list.get(value));
        });

        // Set layout parameters for the next button
        LinearLayout.LayoutParams nextButtonParams = new LinearLayout.LayoutParams(
                0,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        nextButtonParams.weight = 1f; // Less weight than the text view
        nextButton.setLayoutParams(nextButtonParams);

        // Create the previous button
        Button previousButton = new Button(new ContextThemeWrapper(this.getContext(), R.style.CustomButtonStyle), null, 0);
        previousButton.setText("<");
        previousButton.setOnClickListener(v -> {
            value = (value - 1 + list.size()) % list.size();
            textView.setText(list.get(value));
        });

        // Set layout parameters for the previous button
        LinearLayout.LayoutParams previousButtonParams = new LinearLayout.LayoutParams(
                0,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        previousButtonParams.weight = 1f; // Less weight than the text view
        previousButton.setLayoutParams(previousButtonParams);

        // Create a horizontal LinearLayout for the buttons and text view
        LinearLayout horizontalLayout = new LinearLayout(getContext());
        horizontalLayout.setOrientation(LinearLayout.HORIZONTAL);
        horizontalLayout.addView(previousButton);
        horizontalLayout.addView(textView);
        horizontalLayout.addView(nextButton);

        // Create the main vertical LinearLayout
        LinearLayout mainLayout = new LinearLayout(getContext());
        mainLayout.setOrientation(LinearLayout.VERTICAL);
        mainLayout.addView(preferenceName);
        mainLayout.addView(horizontalLayout);

        return mainLayout;
    }

    /**
     * This method updates the selected value index.
     *
     * @param value the new index to set, must be within bounds
     */
    @SuppressWarnings("HiddenField")
    public void setValue(final int value) {
        if (value >= 0 && value < list.size()) {
            this.value = value;
        }
    }

    /**
     * This method returns the currently selected string element from the list.
     *
     * @return the selected item
     */
    public String getElement() {
        return list.get(value);
    }
}
