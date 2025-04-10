package com.soundleap.app.model;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.util.Log;
import android.view.MotionEvent;
import android.widget.Button;
import android.widget.LinearLayout;

import androidx.core.graphics.ColorUtils;

import lombok.RequiredArgsConstructor;

/**
 * This class represents a touch-sensitive UI pad button in the application.
 */
@RequiredArgsConstructor
@SuppressWarnings("MagicNumber")
public class Pad {

    /**
     * Android context used to create UI components.
     */
    private final Context context;

    /**
     * Byte code to identify the pad, used for callback communication.
     */
    private final byte byteCode;

    /**
     * Hexadecimal color string (e.g., "#FF0000") used to style the pad.
     */
    private final String colorHex;

    /**
     * Tracks whether the pad has been clicked at least once.
     */
    private boolean hasBeenClicked = false;

    /**
     * Listener interface to handle pad click events.
     */
    public interface PadClickListener {

        /**
         * This method is called when a pad is clicked.
         *
         * @param byteCode the byte code associated with the clicked pad
         */
        void onPadClicked(byte byteCode);
    }

    /**
     * This method creates and returns a customized `Button` instance representing this pad.
     * The button has custom shape, color, margin, and click behavior.
     * Touch interaction darkens the button on press and restores it on release.
     *
     * @param contentDescription a content description for accessibility
     * @param padClickListener   listener to notify when the pad is clicked
     * @return a fully configured `Button` instance
     */
    @SuppressLint("ClickableViewAccessibility")
    public Button getButton(final String contentDescription, final PadClickListener padClickListener) {

        Button button = new Button(context);

        GradientDrawable shape = new GradientDrawable();
        shape.setShape(GradientDrawable.RECTANGLE);
        shape.setCornerRadius(32f);
        shape.setColor(getArgb());
        button.setBackground(shape);
        button.setContentDescription(contentDescription);

        button.setHeight(250);
        button.setWidth(250);
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
        );
        layoutParams.setMargins(16, 16, 16, 16);
        button.setLayoutParams(layoutParams);

        button.setOnClickListener(v -> {
            hasBeenClicked = true;
            padClickListener.onPadClicked(byteCode);
        });

        button.setOnTouchListener((v, event) -> {
            GradientDrawable drawable = createGradientDrawable(getArgb());

            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    int darkerColor = ColorUtils.blendARGB(getArgb(), Color.BLACK, 0.50f);
                    drawable.setColor(darkerColor);
                    break;
                case MotionEvent.ACTION_UP:
                    drawable.setColor(getArgb());
                    break;
                default:
                    Log.d("Pad", "Unhandled action: " + event.getAction());
                    break;
            }

            button.setBackground(drawable);
            return false;
        });

        return button;
    }

    /**
     * This method creates a GradientDrawable with the given color and corner radius.
     *
     * @param color The color to set for the drawable.
     * @return A new GradientDrawable with the specified color and corner radius.
     */
    private GradientDrawable createGradientDrawable(final int color) {
        GradientDrawable drawable = new GradientDrawable();
        drawable.setCornerRadius(32f);
        drawable.setColor(color);
        return drawable;
    }

    /**
     * This method checks whether this pad has been clicked at least once.
     *
     * @return true if clicked, false otherwise
     */
    public boolean isClicked() {
        return hasBeenClicked;
    }

    /**
     * This method parses the hex color string into an ARGB integer value.
     *
     * @return the ARGB representation of the color
     */
    private int getArgb() {
        return Color.parseColor(colorHex);
    }
}
