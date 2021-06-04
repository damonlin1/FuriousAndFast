#ifndef __FAF_AUDIO_H__
#define __FAF_AUDIO_H__

#include <stdbool.h>

extern const int SONG_CHANNEL;
extern const int CAR_SOUND_CHANNEL;
extern const int ENVIRONMENT_SOUND_CHANNEL;
extern const int COUNTDOWN_CHANNEL;

/**
 * Initializes the program to play audio.
 */
void faf_audio_init();

/**
 * Plays the countdown and car engine noise.
 */
void faf_audio_start_race();

/**
 * Ends the race audio.
 */
void faf_audio_end_race();

/**
 * Plays the background music of a random track.
 */
void faf_audio_play_music();

/**
 * Plays a honking sound.
 */
void faf_audio_honk();

/**
 * Sets the volume for a channel.
 * 
 * @param channel the channel to change the volume for
 * @param volume the volume to set (0 to 128)
 */
void faf_audio_set_volume(int channel, int volume);

/**
 * Frees Mix_Music used to play audio.
 */
void faf_audio_free();

#endif // #ifndef __FAF_AUDIO_H__