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

short *audioBuffer;

volatile int8_t current_song = -1;
bool musicFileLoaded = false;

// Prototype static prototype methods
static void loadSamples();
static int loadSong(int temp);

BSoundPlayer::BSoundPlayer() {

}

BSoundPlayer::~BSoundPlayer() {

}

#ifdef __XTENSA__ 


bool WARNED_OF_PLAY_BUFFER = false;

esp_timer_create_args_t timer_args;
esp_timer_handle_t timer;

static void timerCallback(void *arg) {
  if (musicFileLoaded && (current_song > -1)) {
    int result = xmp_play_buffer(xmpContext, audioBuffer, AUDIO_BUFF_SIZE, 0);

    if (result != 0) {
      if (!WARNED_OF_PLAY_BUFFER) {
        // Something really bad happened, and audio stopped :(
        printf("play_audioBufferer not zero (result = %i)!\n", result);fflush(stdout);
        WARNED_OF_PLAY_BUFFER = true;
      }
      memset(audioBuffer, 0, AUDIO_BUFF_SIZE);      
    }
  }
  else {
    memset(audioBuffer, 0, AUDIO_BUFF_SIZE);
  }
  
  audio.Submit(audioBuffer, AUDIO_BUFF_SIZE >> 2);
}


void BSoundPlayer::Init() {
  audio.Init(SAMPLE_RATE);
  
  audioBuffer = (short *)heap_caps_malloc(sizeof(short) * AUDIO_BUFF_SIZE, MALLOC_CAP_8BIT); // SPI RAM
  memset(audioBuffer, 0, AUDIO_BUFF_SIZE);
  audio.MuteMusic();

  xmpContext = xmp_create_context();

  //*** CREATE TIMER ***//
  timer_args.callback = &timerCallback;
  timer_args.name = "audioTimer";

  esp_timer_create(&timer_args, &timer);
  esp_timer_start_periodic(timer, TIMER_LENGTH);
}
#else

//SDL2 audio callback
static void timerCallback() {

}

void BSoundPlayer::Init() {
    //Todo: @Jay :: SDL2
}

#endif


// Called EVERY Time we load a song because we have to destroy the context.
static void loadSamples() {
  xmp_start_smix(xmpContext, 4/* Channels */, 9 /* Samples */);
  // xmp_smix_load_sample_from_memory(xmpContext, 0, (void *)_SFX_player_shoot_wav_start, _SFX_player_shoot_wav_start - _SFX_player_shoot_wav_end);
}


static int loadSong(int temp) {

  xmp_end_player(xmpContext);
  xmp_end_smix(xmpContext);
  xmp_stop_module(xmpContext);
  xmp_free_context(xmpContext);


  xmpContext = xmp_create_context();
  loadSamples();
  printf("Loading Song: %i\n", temp); fflush(stdout);
  int loadResult = 0;
    //Todo: converted to an array.
  switch(temp) {

    case 0:

      // loadResult = xmp_load_module_from_memory(xmpContext, (void *)_00_Intro_xm_start, _00_Intro_xm_start - _00_Intro_xm_end);
    break;
    

    default:
    break;
  }  
  printf("loadResult = %i\n", loadResult); fflush(stdout);
  return loadResult;
}


TBool BSoundPlayer::PlayMusic(TInt aSongId, TBool aLoop) {
  if (aSongId == -1) {
    current_song = -1;
    return EFalse;
  }

  if (current_song != aSongId) {
    int tempSongId = aSongId;
    current_song = -1;
    musicFileLoaded = false;
    audio.MuteMusic();

    xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, 0);
    
    int loadResult = loadSong(tempSongId);  

    if (loadResult < 0) {
      // printf("Could not open song %i! %i\n", tempSongId, loadResult);
      // Sometimes XMP fails for no obvious reason. Try one more time for good measure.
      loadResult = loadSong(tempSongId);  
    }  
    
    if (loadResult == 0) {
      musicFileLoaded = true;
    }

    if (!musicFileLoaded) {
      return EFalse;
    }

    xmp_start_player(xmpContext, SAMPLE_RATE, 0);
    xmp_set_player(xmpContext, XMP_PLAYER_VOLUME, 256);
    xmp_set_player(xmpContext, XMP_PLAYER_MIX, 0);


    audio.MuteMusic(EFalse);
    current_song = tempSongId;

    return ETrue;
  }

  return EFalse;
}

TBool BSoundPlayer::MuteMusic(TBool aMuted) {
    audio.MuteMusic(aMuted);
    return ETrue;
}
