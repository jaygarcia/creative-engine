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
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
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

bool musicFileLoaded = false;

// Prototype static prototype methods
static int loadSong(BRaw *aSong);

BSoundPlayer::BSoundPlayer() {
  xmpContext = xmp_create_context();

  mMusicVolume = 16;
  mMuted = false;

}

BSoundPlayer::~BSoundPlayer() {
  Reset();
  xmp_end_player(xmpContext);
#ifndef __XTENSA__
  //TODO: Tear down SDL
#endif
}

bool WARNED_OF_PLAY_BUFFER = false;

#ifdef __XTENSA__



static void timerCallback(void *arg) {
// Should we test for XMP_STATE_UNLOADED, XMP_STATE_PLAYING?
//

  if (musicFileLoaded) {
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
    memset(audio.mAudioBuffer, 0, AUDIO_BUFF_SIZE);
  }


  audio.Submit(audio.mAudioBuffer, AUDIO_BUFF_SIZE >> 2);
}

void BSoundPlayer::Init(TUint8 aNumberEffects) {
  printf("BSoundPlayer::%s\n", __func__);fflush(stdout);

  audio.Init(&timerCallback);
}


#else

static void timerCallback(void *udata, Uint8 *audioBuffer, int length) {

  if (musicFileLoaded) {
    int result = xmp_play_buffer(xmpContext, audioBuffer, length, 0);


    if (result != 0) {
      if (!WARNED_OF_PLAY_BUFFER) {
        // Something really bad happened, and audio stopped :(
        printf("xmp_play_buffer not zero (result = %i)!\n", result);fflush(stdout);
        WARNED_OF_PLAY_BUFFER = true;
      }
      memset(audioBuffer, 0, AUDIO_BUFF_SIZE);
    }
  }
  else {
    memset(audioBuffer, 0, AUDIO_BUFF_SIZE);
  }
}



void BSoundPlayer::Init(TUint8 aNumberEffects) {
  audio.Init(&timerCallback);
  xmp_start_smix(xmpContext, 4/* Channels */, aNumberEffects /* Samples */);

//  loadSamples();
}


#endif


static int loadSong(BRaw *aSong) {

//  xmp_end_player(xmpContext);
//  xmp_end_smix(xmpContext);
  xmp_stop_module(xmpContext);

//  xmp_free_context(xmpContext);
//  xmpContext = xmp_create_context();
//  loadSamples();
//

  printf("Loading Song: %i\n", aSong->mSize); fflush(stdout);
  int loadResult = xmp_load_module_from_memory(xmpContext, (uint8_t *)aSong->mData, aSong->mSize);


  printf("loadResult = %i\n", loadResult); fflush(stdout);
  return loadResult;
}

TBool BSoundPlayer::LoadEffect(BRaw *aWavFile, TUint8 aSlotNumber, TBool aLoop) {
  printf("LoadEffect slot=%i, size=%i\n", aSlotNumber, aWavFile->mSize);
  xmp_smix_load_sample_from_memory(xmpContext, aSlotNumber, aWavFile->mData, aWavFile->mSize);
}

TBool BSoundPlayer::PlayMusic(BRaw *aSong, TBool aLoop) {
#ifndef __XTENSA__
  SDL_PauseAudio(1);
#endif


  musicFileLoaded = false;
  MuteMusic(ETrue);

  xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, 0);

  int loadResult = loadSong(aSong);

  if (loadResult < 0) {
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


  MuteMusic(EFalse);
#ifndef __XTENSA__
  SDL_PauseAudio(0);
#endif
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
  if (musicFileLoaded == false) {
    printf("No Music file loaded\n");
    return false;
  }


  printf("xmp_smix_play_sample %i\n", aSoundNumber);
  xmp_smix_play_sample(xmpContext, aSoundNumber, 60, mEffectsVolume, sfxChannel);
  sfxChannel++;
  if (sfxChannel >= 3) {
    sfxChannel = 0;
  }
  return true;
}
