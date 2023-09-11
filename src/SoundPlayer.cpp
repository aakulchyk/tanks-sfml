#include "SoundPlayer.h"


using SoundType = SoundPlayer::SoundType;
using namespace std;

static std::unordered_map<SoundType, std::string> soundPaths = {
    {SoundType::TankStand, string("assets/new_sounds/newest/car_idle_lp_01.wav")},
    {SoundType::TankMove, string("assets/new_sounds/newest/car_move_lp_01.wav")},
    {SoundType::Shoot, string("assets/new_sounds/newest/raygun_04.wav")},
    {SoundType::bulletHitWall, string("assets/new_sounds/newest/hit_normal_09.wav")},
    {SoundType::smallExplosion, string("assets/new_sounds/newest/explosion_short_punchy_rnd_01.wav")},
    {SoundType::bigExplosion, string("assets/new_sounds/newest/explosion_medium_09.wav")},
    {SoundType::bonusAppear, string("assets/new_sounds/newest/powerup_12.wav")},
    {SoundType::bonusCollect, string("assets/new_sounds/newest/powerup_02.wav")},
    {SoundType::iceSkid, string("assets/new_sounds/newest/tyre_skid_lp_01.wav")},
    {SoundType::pause, string("assets/new_sounds/newest/notification_1.wav")},
    {SoundType::xpCollect, string("assets/new_sounds/newest/pickup_15.wav")},
    {SoundType::tick, string("assets/new_sounds/newest/UI_click_15.wav")},
    {SoundType::win, string("assets/audio/8_Bit_Dendy_Battle_City.wav")},
    {SoundType::fail, string("assets/new_sounds/newest/jingles/player_death_01.wav")},
    {SoundType::BuildWall, string("assets/new_sounds/newest/rifle_shot_rnd_01.wav")},
    {SoundType::DestroyWall, string("assets/new_sounds/newest/block_smash_rnd_01.wav")},
    {SoundType::GetDamage, string("assets/new_sounds/newest/hit_normal_04.wav")},
    {SoundType::DamageEnemy, string("assets/new_sounds/newest/hit_normal_11.wav")},
    {SoundType::moveCursor, string("assets/new_sounds/Coin Pickup 48.wav")},
    {SoundType::startGame, string("assets/new_sounds/Coin Pickup 42.wav")},
    {SoundType::briefingTheme, string("assets/audio/march-massacre.wav")},
    {SoundType::BossTheme, string("assets/audio/bossTankLoop.wav")},
    {SoundType::ShopTheme, string("assets/audio/shop-theme.wav")},
    {SoundType::PartialReload, string("assets/new_sounds/Gun Reload 6.wav")},
    {SoundType::FullReload, string("assets/new_sounds/Gun Reload 1.wav")}
};


SoundPlayer::SoundPlayer()
{
    if (-1 == loadSounds())
        Logger::instance() << "[ERROR] Cannot load sounds";
}


SoundPlayer& SoundPlayer::instance()
{
    static SoundPlayer _instance;
    return _instance;
}


int SoundPlayer::loadSounds()
{
    for (auto item : soundPaths) {
        auto soundType = item.first;
        auto soundPath = item.second;

        loadedSounds[soundType] = {};
        auto& thisBuffer = loadedSounds.at(soundType);
        if (!thisBuffer.first.loadFromFile(soundPath))
            return -1;
        thisBuffer.second.setBuffer(thisBuffer.first);
    }

    return 0;
}

void SoundPlayer::stopAllSounds()
{
    stopSound(TankStand);
    stopSound(TankMove);
    stopSound(BossTheme);
    stopSound(ShopTheme);
}

void SoundPlayer::pauseAllSounds()
{
    pauseSound(TankStand);
    pauseSound(TankMove);
    pauseSound(BossTheme);
    pauseSound(ShopTheme);
}

void SoundPlayer::playSound(SoundPlayer::SoundType type)
{
    switch (type)
    {
        case NoSound:
            stopAllSounds();
            break;
        case TankStand:
            playTankStandSound();
            break;
        case TankMove:
            playTankMoveSound();
            break;
        case iceSkid:
            playIceSkidSound();
            break;
        case BossTheme:
            playBossTheme();
            break;
        case ShopTheme:
            playShopTheme();
            break;
        default:
            loadedSounds.at(type).second.play();
            break;
    }
}

void SoundPlayer::stopSound(SoundPlayer::SoundType type)
{
    loadedSounds.at(type).second.stop();
}

void SoundPlayer::pauseSound(SoundPlayer::SoundType type)
{
    loadedSounds.at(type).second.pause();
}


void SoundPlayer::playTankStandSound()
{
    auto& tankStandSound = loadedSounds.at(TankStand).second;
    auto& tankMoveSound = loadedSounds.at(TankMove).second;

    sf::SoundSource::Status standStatus = tankStandSound.getStatus();
    sf::SoundSource::Status moveStatus = tankMoveSound.getStatus();

    if (moveStatus == sf::SoundSource::Status::Playing)
        tankMoveSound.pause();

    if (gameOver || bossAlive) {
        tankStandSound.stop();
        return;
    }

    if (standStatus != sf::SoundSource::Status::Playing) {
        tankStandSound.setLoop(true);
        tankStandSound.play();
    }
}

void SoundPlayer::playTankMoveSound()
{
    auto& tankStandSound = loadedSounds.at(TankStand).second;
    auto& tankMoveSound = loadedSounds.at(TankMove).second;

    sf::SoundSource::Status standStatus = tankStandSound.getStatus();
    sf::SoundSource::Status moveStatus = tankMoveSound.getStatus();

    if (standStatus == sf::SoundSource::Status::Playing)
        tankStandSound.pause();

    if (bossAlive) {
        tankMoveSound.stop();
        return;
    }


    if (moveStatus != sf::SoundSource::Status::Playing) {
        tankMoveSound.setLoop(true);
        tankMoveSound.play();
    }
}

void SoundPlayer::playBossTheme()
{
    bossAlive = true;
    auto& bossMusic = loadedSounds.at(BossTheme).second;
    sf::SoundSource::Status moveStatus = bossMusic.getStatus();
    if (moveStatus != sf::SoundSource::Status::Playing) {
        bossMusic.setLoop(true);
        bossMusic.play();
    }
}

void SoundPlayer::playShopTheme()
{
    auto& shopMusic = loadedSounds.at(ShopTheme).second;
    sf::SoundSource::Status moveStatus = shopMusic.getStatus();
    if (moveStatus != sf::SoundSource::Status::Playing) {
        shopMusic.setLoop(true);
        shopMusic.play();
    }
}

void SoundPlayer::playIceSkidSound()
{
    auto iceSkidSound = loadedSounds.at(iceSkid).second;
    sf::SoundSource::Status skidStatus = iceSkidSound.getStatus();
    if (skidStatus != sf::SoundSource::Status::Playing)
        iceSkidSound.play();
}

void SoundPlayer::enqueueSound(SoundType type, bool play)
{
    if (play)
        _sounds_to_play.push(type);
    else
        _sounds_to_stop.push(type);
}

std::queue<SoundPlayer::SoundType>& SoundPlayer::getSoundsQueue(bool play)
{
    if (play)
        return _sounds_to_play;
    else
        return _sounds_to_stop;
}

void SoundPlayer::processQueuedSounds()
{
    while (!_sounds_to_stop.empty()) {
        stopSound(_sounds_to_stop.front());
        _sounds_to_stop.pop();
    }

    while (!_sounds_to_play.empty()) {
        playSound(_sounds_to_play.front());
        _sounds_to_play.pop();
    }
}