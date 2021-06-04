#include "faf_audio.h"
#include "mathlib.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

const int NUM_TRACKS = 9;
const char *PLAYLIST[9] = {"assets/music/Countdown.wav",
						   "assets/music/RideOut.wav",
			   			   "assets/music/WeOwnIt.wav",
						   "assets/music/GangUp.wav",
						   "assets/music/GoOff.wav",
                           "assets/music/Offset.wav",
                           "assets/music/BlastOff.wav",
                           "assets/music/SeizeTheBlock.wav",
                           "assets/music/GoHardOrGoHome.wav"};
const int LOOPS = 0;
const int SONG_CHANNEL = 1;
const int CAR_SOUND_CHANNEL = 2;
const int ENVIRONMENT_SOUND_CHANNEL = 3;
const int COUNTDOWN_CHANNEL = 4;

Mix_Chunk *music[9];
Mix_Chunk *car_sound;
Mix_Chunk *environment_sound;
bool countdown_played = false;

void faf_audio_init() {
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD |
             MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_OPUS);
    Mix_OpenAudio(44100, AUDIO_S16SYS, 4, 2048);
    Mix_Volume(SONG_CHANNEL, 50);
    Mix_Volume(CAR_SOUND_CHANNEL, 25);
    Mix_Volume(ENVIRONMENT_SOUND_CHANNEL, 15);
    Mix_Volume(COUNTDOWN_CHANNEL, 100);
    for (size_t i = 0; i < NUM_TRACKS; i++) {
        music[i] = Mix_LoadWAV(PLAYLIST[i]);
    }
    car_sound = Mix_LoadWAV("assets/car/Acceleration.wav");
    environment_sound = Mix_LoadWAV("assets/car/Honking.wav");
}

void faf_audio_start_race() {
    if (!countdown_played) {
        Mix_HaltChannel(SONG_CHANNEL);
        Mix_PlayChannel(COUNTDOWN_CHANNEL, music[0], LOOPS);
        countdown_played = true;
    }

    if (!Mix_Playing(CAR_SOUND_CHANNEL)) {
        Mix_Volume(CAR_SOUND_CHANNEL, 25);
        // LOOPS - 1 means to loop audio indefinitely
        Mix_PlayChannel(CAR_SOUND_CHANNEL, car_sound, LOOPS - 1);
    }
}

void faf_audio_end_race() {
    Mix_Volume(CAR_SOUND_CHANNEL, 0);
    countdown_played = false;
}

void faf_audio_play_music() {
    if (!Mix_Playing(SONG_CHANNEL) && !Mix_Playing(COUNTDOWN_CHANNEL)) {
        int song_idx = mathlib_rand_in_range(1, NUM_TRACKS);
		Mix_PlayChannel(SONG_CHANNEL, music[song_idx], LOOPS);
    }
}

void faf_audio_honk() {
    Mix_PlayChannelTimed(ENVIRONMENT_SOUND_CHANNEL, environment_sound, LOOPS, 3000);
}

void faf_audio_set_volume(int channel, int volume) {
    Mix_Volume(channel, volume);
}

void faf_audio_free() {
    for (size_t i = 0; i < NUM_TRACKS; i++) {
        Mix_FreeChunk(music[i]);
    }
    Mix_FreeChunk(car_sound);
    Mix_FreeChunk(environment_sound);
    Mix_Quit();
}