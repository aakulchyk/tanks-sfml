#pragma once

#include <string>

class Game
{
private:
    enum GameState
    {
        TitleScreen = 0,
        LoadNextLevel = 1,
        PlayingLevel = 2,
        GameOver = 3
    };

    GameState gameState = TitleScreen;
    int currentLevel;

    int framesToDie, framesToWin;

    bool _paused = false;


public:
    Game();

    bool loadAssets();
    bool initializeWindow();
    void initializeVariables();
    bool update();

    bool isWindowOpen() const;

private:
    void updateFrameClock();
    void processWindowEvents();
    int processStateChange();
    bool buildLevelMap(std::string);
    void updateAllObjectControllers();
    void processDeletedObjects();
    void drawGameScreen();
    void recalculateViewPort();
    void drawObjects();
    void updateDisplay();
    void checkStatePostFrame();
    void drawTitleScreen();
};