#include "PlayerControllerClient.h"
#include "GameClient.h"
#include "GlobalTypes.h"




PlayerControllerClient::PlayerControllerClient()
{

}

PlayerControllerClient::~PlayerControllerClient()
{

}

PlayerControllerClient& PlayerControllerClient::instance()
{
    static PlayerControllerClient _instance;
    return _instance;
}

void PlayerControllerClient::setPressedFlag(KeysPressed flag, bool state)
{
    if (state)
        _pressedStates = _pressedStates | flag;
    else
        _pressedStates &= ~flag;
}

bool PlayerControllerClient::wasPressed(KeysPressed flag)
{
    return (_pressedStates & flag) != 0;
}

void PlayerControllerClient::update(net::PlayerInput *input)
{
    //checkForGamePause();
    //if (_pause) return;

    bool action = false;

    input->shoot_request = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if (!wasPressed(LeftPressed))
            _pressedKeys[sf::Keyboard::Left] = _clock.getElapsedTime();
        setPressedFlag(LeftPressed, true);
    } else {
        setPressedFlag(LeftPressed, false);
        _pressedKeys.erase(sf::Keyboard::Left);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        if (!wasPressed(UpPressed))
            _pressedKeys[sf::Keyboard::Up] = _clock.getElapsedTime();
        setPressedFlag(UpPressed, true);
    } else {
        setPressedFlag(UpPressed, false);
        _pressedKeys.erase(sf::Keyboard::Up);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        if (!wasPressed(RightPressed))
            _pressedKeys[sf::Keyboard::Right] = _clock.getElapsedTime();
        setPressedFlag(RightPressed, true);
    } else {
        setPressedFlag(RightPressed, false);
        _pressedKeys.erase(sf::Keyboard::Right);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        if (!wasPressed(DownPressed))
            _pressedKeys[sf::Keyboard::Down] = _clock.getElapsedTime();
        setPressedFlag(DownPressed, true);
    } else {
        setPressedFlag(DownPressed, false);
        _pressedKeys.erase(sf::Keyboard::Down);
    }

    auto recentKey = sf::Keyboard::Unknown;
    sf::Time latestTime = sf::milliseconds(0);

    for (auto pk : _pressedKeys) {
        if (pk.second > latestTime) {
            recentKey = pk.first;
            latestTime = pk.second;
        }
    }

    //Logger::instance() << "speed: " << fSpeed << "/" << speed << "\n";
    globalTypes::Direction direction = globalTypes::Direction::Unknown;//_gameObject->direction();
    switch (recentKey) {
        case sf::Keyboard::Left:
            direction = globalTypes::Left;
            break;
        case sf::Keyboard::Up:
            direction = globalTypes::Up;
            break;
        case sf::Keyboard::Right:
            direction = globalTypes::Right;
            break;
        case sf::Keyboard::Down:
            direction = globalTypes::Down;
            break;
        default:
            break;
    }

    input->direction_request = (uint8_t)direction;

    // TODO play sounds
}