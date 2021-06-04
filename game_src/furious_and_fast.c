#include "faf_audio.h"
#include "faf_menu.h"
#include "mathlib.h"
#include "sdl_wrapper.h"
#include "window.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

extern const vector_t FAF_WINDOW_DIMENSIONS;

void faf_on_key(char key, key_event_type_t type, double held_time, window_t *window) {
    assert(window);

    window_on_key(window, key, type, held_time);
}

int main(int argc, char *argv[]) {
    srand((unsigned)time(0));
    sdl_init(VEC_ZERO, FAF_WINDOW_DIMENSIONS);
    sdl_on_key((key_handler_t)faf_on_key);

    // Game start also initializes the audio system
    window_t *window = faf_game_start();

    while (!sdl_is_done(window)) {
        double dt = time_since_last_tick();
        window_tick(window, dt);
        sdl_render_window(window);
        faf_audio_play_music();
    }

    window_free(window);
    faf_audio_free();
    return 0;
}