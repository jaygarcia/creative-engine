//
// Created by Michael Schwartz on 9/13/18.
//

#ifndef GAME_ENGINE_BSOUNDPLAYER_H
#define GAME_ENGINE_BSOUNDPLAYER_H

#include "BTypes.h"
#include "BBase.h"
#include "BResourceManager.h"

/**
 * Abstract sound player class.
 *
 * Each game will inherit to make GSoundPlayer and implement
 * the pure virtual methods.
 */
class BSoundPlayer : public BBase {
public:
  TUint8 mMusicVolume;
  TUint8 mEffectsVolume;
  BSoundPlayer();
  ~BSoundPlayer();
public:
  void Init();

//   // set system volume
  TBool SetVolume(TFloat aPercent);
  TBool SetMusicVolume(TFloat aPercent);
  TBool SetEffectsVolume(TFloat aPercent);
//   // mute or unute sounds & music (MASTER mute)
//   virtual TBool Mute(TBool aMuted = ETrue) = 0;
// public:
//   // play a sound, overriding an existing sound with lower priority (if necessary)
//   // sound will loop back to start if flag is set
  TBool PlaySound(TInt aSoundNumber, TInt aPriority, TBool aLoop = EFalse);
//   // stop a currently playing sound
//   virtual TBool StopSound(TInt aSoundNumber) = 0;
//   // mute only sound effects (music will continue to be heard)
//   virtual TBool MuteSounds(TBool aMuted = ETrue) = 0;
//   // stop all sounds (but not music)
//   virtual TBool StopSounds() = 0;
// public:
//   // play a song, track will loop back to start if flag is set
//   // if a score is already playing, it will be stopped first
  TBool PlayMusic(BRaw *aSong, TBool aLoop = ETrue);
//   //  stop playing music
  TBool StopMusic();
//   // toggle music paused/playing
  TBool PauseMusic(TBool aPaused = ETrue);
//   // toggle music muted (will not mute sound effects)
  TBool MuteMusic(TBool aMuted = ETrue);
// public:
//   // reset music player, stop all sounds and music
  TBool Reset();
};

extern BSoundPlayer soundPlayer;

#endif //GAME_ENGINE_BSOUNDPLAYER_H
