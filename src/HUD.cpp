#include "EagleController.h"
#include "GameObject.h"
#include "GlobalConst.h"
#include "HUD.h"
#include "ObjectsPool.h"
#include "PlayerUpgrade.h"
#include "PlayerController.h"
#include "Utils.h"
#include "UiUtils.h"



void drawCursor(sf::RectangleShape& parentRect, int pos)
{
    int cursorX = 0;
    int cursorY = parentRect.getPosition().y - parentRect.getSize().y/8;
    switch (pos) {
        case 0:
            cursorX = parentRect.getPosition().x - parentRect.getSize().x/4 - 32;
            break;
        case 1:
            cursorX = parentRect.getPosition().x;
            break;
        case 2:
            cursorX = parentRect.getPosition().x + parentRect.getSize().x/4 + 32;
            break;
    }

    sf::RectangleShape whiteRect(sf::Vector2f(18, 18));
    whiteRect.setScale(globalConst::spriteScaleX+1, globalConst::spriteScaleY+1);
    whiteRect.setOrigin(whiteRect.getSize().x/2, whiteRect.getSize().y/2);
    whiteRect.setPosition(cursorX, cursorY);
    whiteRect.setFillColor(sf::Color(200, 200, 200));
    Utils::window.draw(whiteRect);
}

////////////////////////////////////////

HUD::HUD()
{}

HUD& HUD::instance()
{
    static HUD _instance;
    return _instance;
}

void HUD::draw()
{
    drawPlayerXP(28);
    drawTankLives(64);
    drawTankUpgrades(112);
    drawBaseLives(112+64);
    drawBaseUpgrades(112+64+48);

    drawGlobalTimer();

    if (_showWin)
        drawWinScreen();

    if (_showFail)
        drawFailScreen();
}

void HUD::drawGlobalTimer()
{
    sf::Time time = globalVars::globalChronometer.getElapsedTime();
    int minutes = (int)time.asSeconds() / 60;
    int seconds = (int)time.asSeconds() % 60;

    std::string timeStr = std::to_string(minutes) + ":" + std::to_string(seconds);
    UiUtils::instance().drawText(timeStr, 30, globalConst::screen_w/2, 32);
}

void HUD::drawPlayerXP(int baseY)
{
    const int fontSize = 28;
    std::string levelAndXp = std::to_string(globalVars::player1Level) + "/" + std::to_string(globalVars::player1XP);
    UiUtils::instance().drawText(levelAndXp, fontSize, 8, baseY, true);
}

void HUD::drawTankLives(int baseY)
{
    constexpr int iconWidth = 7;
    constexpr int iconHeight = 8;
    sf::IntRect rect = sf::IntRect(377, 144, iconWidth, iconHeight);

    int baseX = 17;

    for (int i=0; i < globalVars::player1Lives; i++) {
        UiUtils::instance().drawMiniIcon( rect, baseX + i * (iconWidth + 18), baseY );
    }
}

void HUD::drawTankUpgrades(int baseY)
{
    if (ObjectsPool::playerObject == nullptr)
        return;

    auto pController = ObjectsPool::playerObject->getComponent<PlayerController>();

    const int upgradesNumber = pController->numberOfUpgrades();

    int baseX = 32;

    for (int i=0; i<upgradesNumber; i++) {
        UiUtils::instance().drawMiniIcon(
            pController->getUpgrade(i)->iconRect(),
            baseX + i * (32 + 16),
            baseY
        );

        UiUtils::instance().drawText(
            std::to_string(pController->getUpgrade(i)->currentLevel()+1),
            24,
            baseX + i * (32 + 16),
            baseY + 24
        );
    }
}

void HUD::drawBaseLives(int baseY)
{
    sf::IntRect eagleRect = sf::IntRect(376, 125, 9, 8);
    UiUtils::instance().drawMiniIcon(eagleRect, 20, baseY);
}

void HUD::drawBaseUpgrades(int baseY)
{

    if (ObjectsPool::eagleObject == nullptr)
        return;

    auto bController = ObjectsPool::eagleObject->getComponent<EagleController>();

    const int upgradesNumber = bController->numberOfUpgrades();

    int baseX = 32;

    for (int i=0; i<upgradesNumber; i++) {
        UiUtils::instance().drawMiniIcon(
            bController->getUpgrade(i)->iconRect(),
            baseX + i * (32 + 16),
            baseY
        );

        UiUtils::instance().drawText(
            std::to_string(bController->getUpgrade(i)->currentLevel()+1),
            24,
            baseX + i * (32 + 16),
            baseY + 24
        );
    }
}

void HUD::drawWinScreen()
{
    using namespace globalConst;
    UiUtils::instance().drawText(
        "CONGRATULATIONS!" , 90,
        screen_w/2, screen_h/2 - 90, false,
        sf::Color::Green);

    UiUtils::instance().drawText(
        "Good job, commander!" , 50,
        screen_w/2, screen_h/2 + 50, false,
        sf::Color::White);
}

void HUD::drawFailScreen()
{
    using namespace globalConst;
    UiUtils::instance().drawText(
        "GAME OVER" , 90,
        screen_w/2, screen_h/2 - 90, false,
        sf::Color::Red);

    UiUtils::instance().drawText(
        "You survived for " + _surviveTimeStr , 50,
        screen_w/2, screen_h/2 + 50, false,
        sf::Color::White);
}

void HUD::showWin(bool val)
{
    _showWin = val;
}
void HUD::showFail(bool val)
{
    _showFail = val;

    if (_showFail) {
        sf::Time time = globalVars::globalChronometer.getElapsedTime();
        int minutes = (int)time.asSeconds() / 60;
        int seconds = (int)time.asSeconds() % 60;

        _surviveTimeStr = std::to_string(minutes) + " minutes " + std::to_string(seconds) + " seconds";
    }
}