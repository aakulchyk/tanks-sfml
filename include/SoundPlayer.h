#pragma once

#include "Logger.h"

#include <SFML/Audio.hpp>
#include <queue>
#include <unordered_map>

class SoundPlayer
{
public:
    enum SoundType
    {
        NoSound,
        TankStand,
        TankMove,
        Shoot,
        RocketShoot,
        bulletHitWall,
        smallExplosion,
        bigExplosion,
        bonusAppear,
        bonusCollect,
        iceSkid,
        pause,
        xpCollect,
        tick,
        win,
        fail,
        BuildWall,
        DestroyWall,
        GetDamage,
        DamageEnemy,
        moveCursor,
        startGame,
        briefingTheme,
        TitleTheme,
        BossTheme,
        ShopTheme,
        PartialReload,
        FullReload,
        SetLandmine,
        RejectLandmine,
        Alarm
    };

private:
    std::unordered_map<SoundType, std::pair<sf::SoundBuffer, sf::Sound>> loadedSounds;

    std::queue<SoundType> _sounds_to_stop;
    std::queue<SoundType> _sounds_to_play;

public:
    bool gameOver = false;
    bool bossAlive = false;
private:
    SoundPlayer();


    SoundPlayer(SoundPlayer &) = delete;
    void operator=(const SoundPlayer&) = delete;


public:
    static SoundPlayer& instance();

    void playSound(SoundType);
    void playSoundWithOffset(SoundType, int offset);
    void stopSound(SoundType);
    void pauseSound(SoundType);
    void pauseAllSounds();

    int loadSounds();
    void stopAllSounds();
    void playTankStandSound();
    void playTankMoveSound();
    void playTitleTheme();
    void playBossTheme();
    void playShopTheme();

    void playIceSkidSound();

    void enqueueSound(SoundType, bool);

    std::queue<SoundType>& getSoundsQueue(bool);

    void processQueuedSounds();
};