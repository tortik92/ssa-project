package com.soundleap.app.util;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.soundleap.app.R;
import com.soundleap.app.model.Game;

import java.util.List;

/**
 * This class is a custom ArrayAdapter for displaying Game objects in a ListView.
 */
public class GameListAdapter extends ArrayAdapter<Game> {

    /**
     * The list of {@link Game} objects to be displayed.
     */
    private final List<Game> games;

    /**
     * This method constructs the {@link GameListAdapter}.
     *
     * @param context the context used for inflating views
     * @param games   the list of games to be displayed
     * @noinspection checkstyle:HiddenField
     */
    @SuppressWarnings("checkstyle:hiddenfield")
    public GameListAdapter(@NonNull final Context context, final List<Game> games) {
        super(context, 0, games);
        this.games = games;
    }

    /**
     * This method returns a view for the specified position in the data set.
     * This method reuses views whenever possible for performance.
     *
     * @param position    the position of the item within the adapter's data set
     * @param convertView the old view to reuse, if possible
     * @param parent      the parent that this view will eventually be attached to
     * @return the View corresponding to the data at the specified position
     */
    @NonNull
    @Override
    public View getView(final int position, @Nullable final View convertView, @NonNull final ViewGroup parent) {
        View view = convertView;
        if (view == null) {
            view = LayoutInflater.from(getContext()).inflate(R.layout.list_item_layout, parent, false);
        }

        Game game = games.get(position);

        TextView gameName = view.findViewById(R.id.game_name);
        TextView gameDescription = view.findViewById(R.id.game_description);
        TextView gameVersion = view.findViewById(R.id.game_version);

        // Set text for each TextView
        gameName.setText(game.getName());
        gameDescription.setText(game.getDescription());
        gameVersion.setText(game.getVersion());

        return view;
    }
}
