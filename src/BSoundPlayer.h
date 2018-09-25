//
// Created by Michael Schwartz on 9/13/18.
//

#ifndef GAME_ENGINE_BSOUNDPLAYER_H
#define GAME_ENGINE_BSOUNDPLAYER_H

#include "BTypes.h"
#include "BBase.h"

/**
 * Abstract sound player class.
 *
 * Each game will inherit to make GSoundPlayer and implement
 * the pure virtual methods.
 */
class BSoundPlayer : public BBase {
public:
  BSoundPlayer();
  ~BSoundPlayer();
public:
  void Init();

//   // set system volume
//   virtual TBool SetVolume(TFloat aPercent) = 0;
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
  TBool PlayMusic(TInt aSongId, TBool aLoop = ETrue);
//   //  stop playing music
//   virtual TBool StopMusic() = 0;
//   // toggle music paused/playing
//   virtual TBool PauseMusic(TBool aPaused = ETrue) = 0;
//   // toggle music muted (will not mute sound effects)
  TBool MuteMusic(TBool aMuted = ETrue);
// public:
//   // reset music player, stop all sounds and music
//   virtual TBool Reset() = 0;
};

extern BSoundPlayer soundPlayer;

#endif //GAME_ENGINE_BSOUNDPLAYER_H
