package com.soundleap.app.model;

import android.content.Context;
import android.graphics.Typeface;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Gravity;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;

import com.google.android.material.textfield.TextInputEditText;
import com.google.android.material.textfield.TextInputLayout;

import lombok.Getter;

/**
 * This class is a {@link Preference} implementation that allows users to input and store a textual value.
 * It includes min and max length validation and generates a corresponding UI view.
 */
@Getter
@SuppressWarnings("MagicNumber")
public class TextPreference extends Preference {

    /**
     * The default text value assigned to this preference.
     */
    private final String defaultValue;

    /**
     * The minimum number of characters allowed.
     */
    private final int min;

    /**
     * The maximum number of characters allowed.
     */
    private final int max;

    /**
     * The current value of the preference.
     */
    private final String value;

    /**
     * This method constructs a new {@code TextPreference}.
     *
     * @param context      the Android context
     * @param name         the unique name or key for the preference
     * @param type         the type of preference (typically "text")
     * @param defaultValue the initial/default value
     * @param min          the minimum allowed length for the value
     * @param max          the maximum allowed length for the value
     */
    @SuppressWarnings("HiddenField")
    public TextPreference(final Context context, final String name, final String type, final String defaultValue, final int min, final int max) {
        super(context, name, type);
        this.defaultValue = defaultValue;
        this.min = min;
        this.max = max;
        this.value = defaultValue;
    }

    /**
     * This method builds and returns a {@link LinearLayout} view representing this text preference,
     * including a label and an input field with validation feedback.
     *
     * @return the constructed layout
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

        // Create the TextInputLayout
        TextInputLayout inputLayout = getTextInputLayout();

        // Create the main layout
        LinearLayout mainLayout = new LinearLayout(getContext());
        mainLayout.setOrientation(LinearLayout.VERTICAL);
        LinearLayout.LayoutParams mainLayoutParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        mainLayout.setLayoutParams(mainLayoutParams);
        mainLayout.addView(preferenceName);
        mainLayout.addView(inputLayout);

        return mainLayout;
    }

    @NonNull
    private TextInputLayout getTextInputLayout() {
        TextInputLayout inputLayout = new TextInputLayout(getContext());
        inputLayout.setHint("Enter your text");

        // Create the TextInputEditText
        TextInputEditText editText = new TextInputEditText(getContext());
        LinearLayout.LayoutParams editTextParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        editText.setLayoutParams(editTextParams);

        // Add a TextWatcher to handle text changes
        editText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(final CharSequence s, final int start, final int count, final int after) {
            }

            @Override
            public void onTextChanged(final CharSequence s, final int start, final int before, final int count) {
                int textLength = s != null ? s.length() : 0;
                if (textLength < min) {
                    inputLayout.setError("Minimum length is " + min + " characters");
                } else if (textLength > max) {
                    inputLayout.setError("Maximum length is " + max + " characters");
                } else {
                    inputLayout.setError(null);
                }
            }

            @Override
            public void afterTextChanged(final Editable s) {
            }
        });

        // Add the EditText to the TextInputLayout
        inputLayout.addView(editText);
        return inputLayout;
    }
}
