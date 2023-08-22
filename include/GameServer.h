#pragma once

#include "MapCreator.h"
#include "NetGameTypes.h"

#include <string>

namespace server
{

class GameServer
{
private:
    enum GameState
    {
        WaitingForStartRequest = 0,
        LoadNextLevel,
        StartLevelScreen,
        WaitingForLevelStartRequest,
        StartLevel,
        PlayingLevel,
        GameOver
    };

    GameState gameState = WaitingForStartRequest;
    int currentLevel;
    Level::Properties _currentLevelProperties;
    int _surviveTimeoutSec;

    int framesToDie, framesToWin;

    sf::UdpSocket _sendSocket;
    sf::UdpSocket _recvSocket;
    sf::IpAddress sender;

    net::MapDetails _map;
    net::FrameDetails _lastFrame;

public:
    GameServer();

    bool loadAssets();
    void initializeVariables();
    bool update();
    void processWindowEvents();

    bool isWindowOpen() const;

private:
    void updateFrameClock();
    int processStateChange();
    bool buildLevelMap(std::string);
    void updateAllObjectControllers();
    void processDeletedObjects();
    void checkStatePostFrame();
    void pause(bool);
    void networkDraw();
    void recalculateViewPort();

    bool winConditionsMet() const;
    bool failConditionsMet() const;


    void drawConnectionScreen();
    void drawStartingScreen();
    void drawPlayingScreen();
};


}