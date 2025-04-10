package com.soundleap.app.util;

import android.content.Context;

import com.google.gson.JsonDeserializationContext;
import com.google.gson.JsonDeserializer;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;
import com.soundleap.app.model.BooleanPreference;
import com.soundleap.app.model.ListPreference;
import com.soundleap.app.model.NumberPreference;
import com.soundleap.app.model.Preference;
import com.soundleap.app.model.TextPreference;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;

import lombok.RequiredArgsConstructor;

/**
 * This class is a custom deserializer for {@link Preference} objects using Gson.
 */
@RequiredArgsConstructor
public class PreferenceDeserializer implements JsonDeserializer<Preference> {

    /**
     * The context passed during instantiation, used when creating new preferences.
     */
    private final Context context;

    /**
     * This method deserializes a {@link JsonElement} into a {@link Preference} object.
     * This method examines the "preference_type" field in the JSON to determine which concrete {@link Preference} subclass should be created.
     *
     * @param json                   the JSON element to deserialize
     * @param typeOfT                the type of the expected object
     * @param deserializationContext the context of the deserialization
     * @return the deserialized {@link Preference} object
     * @throws JsonParseException if there is an error during deserialization
     */
    @Override
    public Preference deserialize(final JsonElement json, final Type typeOfT, final JsonDeserializationContext deserializationContext) throws JsonParseException {
        JsonObject jsonObject = json.getAsJsonObject();
        String type = jsonObject.get("preference_type").getAsString();
        String name = jsonObject.get("preference_name").getAsString();

        switch (type) {
            case "bool":
                boolean defaultBool = jsonObject.get("default_value").getAsBoolean();
                return new BooleanPreference(context, name, type, defaultBool);

            case "number":
                int defaultNumber = jsonObject.get("default_value").getAsInt();
                int min = jsonObject.get("min_value").getAsInt();
                int max = jsonObject.get("max_value").getAsInt();
                return new NumberPreference(context, name, type, defaultNumber, min, max);

            case "list":
                int defaultList = jsonObject.get("default_value").getAsInt();
                List<String> list = new ArrayList<>();
                jsonObject.get("list").getAsJsonArray().forEach(element -> list.add(element.getAsString()));
                return new ListPreference(context, name, type, defaultList, list);

            case "text":
                String defaultText = jsonObject.get("default_value").getAsString();
                int minLength = jsonObject.get("min_length").getAsInt();
                int maxLength = jsonObject.get("max_length").getAsInt();
                return new TextPreference(context, name, type, defaultText, minLength, maxLength);

            default:
                throw new JsonParseException("Unknown preference type: " + type);
        }
    }
}
