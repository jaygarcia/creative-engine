#include "BSoundPlayer.h"

BSoundPlayer soundPlayer;

// #include "Music.h"

#include "Audio.h"
#include "libxmp/xmp.h"


#ifdef __XTENSA__

#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include <string.h> // For memset

#else
// Todo: @Jay Anything else? (SDL2?)
//#include SDL2 lib
#include <SDL.h>
#include <SDL_audio.h>
#endif



#ifdef __XTENSA__

#define SAMPLE_RATE (22050)
#define TIMER_LENGTH 50
#define AUDIO_BUFF_SIZE 12

#else

#define SAMPLE_RATE (44100)
#define TIMER_LENGTH 50
#define AUDIO_BUFF_SIZE 12

#endif


xmp_context xmpContext;

volatile int8_t current_song = -1;
bool musicFileLoaded = false;

// Prototype static prototype methods
static void loadSamples();
static int loadSong(BRaw *aSong);

BSoundPlayer::BSoundPlayer() {
  mMusicVolume = 16;
}

BSoundPlayer::~BSoundPlayer() {
  Reset();
  xmp_end_player(xmpContext);
#ifndef __XTENSA__
  //TODO: Tear down SDL
#endif
}

#ifdef __XTENSA__


bool WARNED_OF_PLAY_BUFFER = false;

int x = 0;
static void timerCallback(void *arg) {
// Should we test for XMP_STATE_UNLOADED, XMP_STATE_PLAYING?
//
//  if (x == 0) {
//      printf("musicFileLoaded = %i :: current_song = %i\n", musicFileLoaded , current_song);fflush(stdout);
//  }

  if (musicFileLoaded && (current_song > -1)) {
    int result = xmp_play_buffer(xmpContext, audio.mAudioBuffer, AUDIO_BUFF_SIZE, 0);

    if (result != 0) {
      if (!WARNED_OF_PLAY_BUFFER) {
        // Something really bad happened, and audio stopped :(
        printf("play_audioBufferer not zero (result = %i)!\n", result);fflush(stdout);
        WARNED_OF_PLAY_BUFFER = true;
      }
      memset(audio.mAudioBuffer, 0, AUDIO_BUFF_SIZE);
    }
  }
  else {
    if (x == 0) {
    }
    memset(audio.mAudioBuffer, 0, AUDIO_BUFF_SIZE);
  }

  x++;
  if (x > 500) {
    x = 0;
  }

  audio.Submit(audio.mAudioBuffer, AUDIO_BUFF_SIZE >> 2);
}

void BSoundPlayer::Init() {
  printf("BSoundPlayer::%s\n", __func__);fflush(stdout);

  audio.Init(&timerCallback);

  xmpContext = xmp_create_context();
  loadSamples();
}



#else

void BSoundPlayer::Init() {
//  audio.Init(timerCallback);
//
//  xmpContext = xmp_create_context();
//  loadSamples();
}


#endif


// Called EVERY Time we load a song because we have to destroy the context.
static void loadSamples() {
  xmp_start_smix(xmpContext, 4/* Channels */, 9 /* Samples */);
//  xmp_smix_load_sample_from_memory(xmpContext, 0, (void *)_SFX_boss_explode_wav_start, _SFX_boss_explode_wav_start - _SFX_boss_explode_wav_end);
//  xmp_smix_load_sample_from_memory(xmpContext, 1, (void *)_SFX_enemy_explode_wav_start, _SFX_enemy_explode_wav_start - _SFX_enemy_explode_wav_end);
//  xmp_smix_load_sample_from_memory(xmpContext, 2, (void *)_SFX_enemy_flyby_wav_start, _SFX_enemy_flyby_wav_start - _SFX_enemy_flyby_wav_end);
//  xmp_smix_load_sample_from_memory(xmpContext, 3, (void *)_SFX_enemy_shoot_wav_start, _SFX_enemy_shoot_wav_start - _SFX_enemy_shoot_wav_end);
//  xmp_smix_load_sample_from_memory(xmpContext, 4, (void *)_SFX_next_attract_char_wav_start, _SFX_next_attract_char_wav_start - _SFX_next_attract_char_wav_end);
//  xmp_smix_load_sample_from_memory(xmpContext, 5, (void *)_SFX_next_attract_screen_wav_start, _SFX_next_attract_screen_wav_start - _SFX_next_attract_screen_wav_end);
//  xmp_smix_load_sample_from_memory(xmpContext, 6, (void *)_SFX_player_hit_wav_start, _SFX_player_hit_wav_start - _SFX_player_hit_wav_end);
//  xmp_smix_load_sample_from_memory(xmpContext, 7, (void *)_SFX_player_shoot_wav_start, _SFX_player_shoot_wav_start - _SFX_player_shoot_wav_end);
//  xmp_smix_load_sample_from_memory(xmpContext, 8, (void *)_SFX_speed_boost_wav_start, _SFX_speed_boost_wav_start - _SFX_speed_boost_wav_end);
}


static int loadSong(BRaw *aSong) {

//  xmp_end_player(xmpContext);
//  xmp_end_smix(xmpContext);
  xmp_stop_module(xmpContext);
//  xmp_free_context(xmpContext);
//  xmpContext = xmp_create_context();
//  loadSamples();
//

  printf("Loading Song: %i\n", aSong->mSize); fflush(stdout);
  int loadResult = xmp_load_module_from_memory(xmpContext, (void *)aSong->mData, aSong->mSize);


  printf("loadResult = %i\n", loadResult); fflush(stdout);
  return loadResult;
}


TBool BSoundPlayer::PlayMusic(BRaw *aSong, TBool aLoop) {
  current_song = -1;
  musicFileLoaded = false;
  audio.MuteMusic(ETrue);

  xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, 0);

  int loadResult = loadSong(aSong);

  if (loadResult < 0) {
    // printf("Could not open song %i! %i\n", tempSongId, loadResult);
    // Sometimes XMP fails for no obvious reason. Try one more time for good measure.
    loadResult = loadSong(aSong);
  }

  if (loadResult == 0) {
    musicFileLoaded = true;
  }

  if (!musicFileLoaded) {
    printf("MUSIC LOADING FAILED!\n"); fflush(stdout);
    return EFalse;
  }

  xmp_start_player(xmpContext, SAMPLE_RATE, 0);
  xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, mMusicVolume);
  xmp_set_player(xmpContext, XMP_PLAYER_MIX, 0);


  audio.MuteMusic(EFalse);
  printf("current_song = %i\n", current_song); fflush(stdout);

  return ETrue;
}

//
//TBool BSoundPlayer::PlayMusic(int8_t aSongId, TBool aLoop) {
//  printf("BSoundPlayer::%s %i\n", __func__, aSongId); fflush(stdout);
//  if (aSongId == -1) {
//    current_song = -1;
//    return EFalse;
//  }
//
//  if (current_song != aSongId) {
//    current_song = -1;
//    musicFileLoaded = false;
//    audio.MuteMusic(ETrue);
//
//    xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, 0);
//
//    int loadResult = loadSong(aSongId);
//
//    if (loadResult < 0) {
//      // printf("Could not open song %i! %i\n", tempSongId, loadResult);
//      // Sometimes XMP fails for no obvious reason. Try one more time for good measure.
//      loadResult = loadSong(aSongId);
//    }
//
//    if (loadResult == 0) {
//      musicFileLoaded = true;
//    }
//
//    if (!musicFileLoaded) {
//      printf("MUSIC LOADING FAILED!\n"); fflush(stdout);
//      return EFalse;
//    }
//
//    xmp_start_player(xmpContext, SAMPLE_RATE, 0);
//    xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, mMusicVolume);
//    xmp_set_player(xmpContext, XMP_PLAYER_MIX, 0);
//
//
//    audio.MuteMusic(EFalse);
//    current_song = aSongId;
//    printf("current_song = %i\n", current_song); fflush(stdout);
//
//    return ETrue;
//  }
//
//  return EFalse;
//}

TUint8 sfxChannel = 0;

TBool BSoundPlayer::StopMusic() {
  // Should we test for XMP_STATE_UNLOADED, XMP_STATE_PLAYING?
  xmp_stop_module(xmpContext);
  current_song = -1;
  return true;
}

TBool BSoundPlayer::Reset() {
  StopMusic();
//  xmp_end_player(xmpContext);
  musicFileLoaded = false;
  return true;
}

TBool BSoundPlayer::SetVolume(TFloat aPercent) {
  if (xmpContext) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }
    if (aPercent < 0.0f) {
      aPercent = 0;
    }

    mMusicVolume = (TUint8)(aPercent * 255);
    mEffectsVolume = mMusicVolume;

    xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, mMusicVolume);
    return true;
  }

  return false;
}

TBool BSoundPlayer::SetMusicVolume(TFloat aPercent) {
  if (xmpContext) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }
    if (aPercent < 0.0f) {
      aPercent = 0;
    }

    mMusicVolume = (TUint8)(aPercent * 255);

    xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, mMusicVolume);
    return true;
  }

  return false;
}

TBool BSoundPlayer::SetEffectsVolume(TFloat aPercent) {
  if (xmpContext) {
    if (aPercent > 1.0f) {
      aPercent = 1.0f;
    }

    if (aPercent < 0.0f) {
      aPercent = 0;
    }

    mEffectsVolume = (TUint8)(aPercent * 255);

    xmp_set_player(xmpContext, XMP_PLAYER_SMIX_VOLUME, mEffectsVolume);
    return true;
  }

  return false;
}



TBool BSoundPlayer::PlaySound(TInt aSoundNumber, TInt aPriority, TBool aLoop) {
  //Todo: priority?
  if (musicFileLoaded == false || current_song == -1) {
    return false;
  }

//  if (id == SFX_SPEED_BOOST && speedBoostSfxPlaying == false) {
//    xmp_smix_play_sample(ctx, id, 40, 128, 4);
//    speedBoostSfxPlaying = true;
//  }
//  else {
    xmp_smix_play_sample(xmpContext, aSoundNumber, 60, 32 /* Volume */, sfxChannel);
    sfxChannel++;
    if (sfxChannel >= 3) {
      sfxChannel = 0;
    }
//  }
  return true;
}

TBool BSoundPlayer::MuteMusic(TBool aMuted) {
    audio.MuteMusic(aMuted);
    return ETrue;
}
