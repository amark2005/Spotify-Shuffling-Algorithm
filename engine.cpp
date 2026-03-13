#include <raylib.h>
#include <raygui.h>
#include <fstream>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>


static const int32_t SONG_COUNT = 26;

using std::cout;
using std::string;
using json = nlohmann::json;


struct Song {
    std::string song_name;
    int32_t     song_id;
    std::string artist_name;
    int32_t     artist_id;
    int32_t     duration_ms;
    int32_t     play_count;
    int32_t     skip_count;
    bool        liked;
    float       weight_score;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Song, song_name, song_id, artist_name, artist_id,
                                    duration_ms, play_count, skip_count, liked, weight_score);


struct PlayerState {
    Song    songs[SONG_COUNT];
    int32_t playlist[SONG_COUNT];
    int32_t song_count;
    int32_t music_cursor;

    PlayerState() : song_count(0), music_cursor(0) {
        for (int32_t i = 0; i < SONG_COUNT; i++) {
            playlist[i] = i;
        }
    }
};

static int32_t randi(const int32_t min, const int32_t max) {
    static std::mt19937 rng(std::random_device{}());
    return std::uniform_int_distribution<int32_t>(min, max)(rng);
}

static bool loadsong(PlayerState& state) {
    bool success = false;
    std::ifstream file("song.json");

    if (file.is_open()) {
        json data;
        file >> data;

        int32_t count = static_cast<int32_t>(data.size()); 
        if (count > SONG_COUNT) { count = SONG_COUNT; }

        for (int32_t i = 0; i < count; i++) {
            state.songs[i].song_name    = data[i]["song_name"].get<string>();
            state.songs[i].song_id      = data[i]["song_id"].get<int32_t>();
            state.songs[i].artist_name  = data[i]["artist_name"].get<string>();
            state.songs[i].artist_id    = data[i]["artist_id"].get<int32_t>();
            state.songs[i].duration_ms  = data[i]["duration_ms"].get<int32_t>();
            state.songs[i].play_count   = data[i]["play_count"].get<int32_t>();
            state.songs[i].skip_count   = data[i]["skip_count"].get<int32_t>();
            state.songs[i].liked        = data[i]["liked"].get<bool>();
            state.songs[i].weight_score = data[i]["weight_score"].get<float>();
        }

        state.song_count = count;
        success = true;
    } else {
        cout << "No json file found\n";
    }

    return success; 
}

static void compute_weights(PlayerState& state) {
    for (int32_t i = 0; i < state.song_count; i++) {
        Song& s = state.songs[i];
        s.weight_score = 1.0f
            + static_cast<float>(s.play_count)
            + (s.liked ? 5.0f : 0.0f)
            - static_cast<float>(s.skip_count) * 2.0f;

        if (s.weight_score < 0.1f) { s.weight_score = 0.1f; }
    }
}

static void shuffle(PlayerState& state) {
    for (int32_t i = state.song_count - 1; i > 0; i--) {
        const int32_t j = randi(0, i);
        if (state.songs[state.playlist[j]].weight_score
         <= state.songs[state.playlist[i]].weight_score) {
            const int32_t tmp  = state.playlist[i];
            state.playlist[i]  = state.playlist[j];
            state.playlist[j]  = tmp;
        }
    }
}

static void rebuild_playlist(PlayerState& state) {
    for (int32_t i = 0; i < state.song_count; i++) {
        state.playlist[i] = i;
    }
}

int32_t main() {
    PlayerState state;

    if (!loadsong(state)) { return 1; } 

    cout << "Songs loaded: " << state.song_count << "\n";

    compute_weights(state);
    rebuild_playlist(state);
    shuffle(state);

    const int32_t WID     = 500;
    const int32_t HEIG    = 300;
    const int32_t CENTWID = WID / 2;

    InitWindow(WID, HEIG, "Shuffle");

    static int32_t scroll_index = 0;
    static int32_t list_active  = 0;

    std::string list_items;
    for (int32_t i = 0; i < state.song_count; i++) {
        list_items += state.songs[state.playlist[i]].song_name;
        if (i < state.song_count - 1) { list_items += ";"; }
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        if(IsKeyPressed(KEY_Q))break;
        const Rectangle next_btn    = { CENTWID + 60.0f,  100.0f, 100.0f, 40.0f };
        const Rectangle prev_btn    = { CENTWID - 160.0f, 100.0f, 100.0f, 40.0f };
        const Rectangle shuffle_btn = { CENTWID - 50.0f,  150.0f, 100.0f, 40.0f };

        DrawText("Now Playing", CENTWID - 40, 35, 12, WHITE);
        DrawText(state.songs[state.playlist[state.music_cursor]].song_name.c_str(),
                 CENTWID - 60, 60, 18, WHITE);

        if (GuiButton(next_btn, "Next")) {
            state.music_cursor++;
            if (state.music_cursor >= state.song_count) { state.music_cursor = 0; }
        }

        if (GuiButton(prev_btn, "Previous")) {
            state.music_cursor--;
            if (state.music_cursor < 0) { state.music_cursor = state.song_count - 1; }
        }

        if (GuiButton(shuffle_btn, "Shuffle")) {
            compute_weights(state);
            rebuild_playlist(state);
            shuffle(state);
            state.music_cursor = 0;

            list_items.clear();
            for (int32_t i = 0; i < state.song_count; i++) {
                list_items += state.songs[state.playlist[i]].song_name;
                if (i < state.song_count - 1) { list_items += ";"; }
            }
        }

        GuiListView({ 10.0f, 200.0f, 480.0f, 200.0f },
                    list_items.c_str(), &scroll_index, &list_active);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}