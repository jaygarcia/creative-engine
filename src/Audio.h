#ifndef AUDIO_H
#define AUDIO_H

#include "BTypes.h"
#include "BBase.h"


#ifdef __XTENSA__
/***** ODROID GO *****/
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "driver/i2s.h"
#include "driver/rtc_io.h"

#else
/***** Mac/Linux *****/
// SDL2 audio

#endif


class Audio : public BBase {
public:
  Audio();
  virtual ~Audio();
  void Init(TUint16 new_sample_rate);

  void SetVolume(TFloat value);
  //  void ChangeVolume();
  TFloat GetVolume();
  void Terminate();
  void Submit(TInt16 *stereoAudioBuffer, TInt frameCount);
  TInt GetSampleRate();
  void MuteMusic(TBool aMuted = ETrue);
};


extern Audio audio;

#endif
