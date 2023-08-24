#include "GameObject.h"
#include "GlobalConst.h"
#include "BonusShopWindow.h"
#include "ObjectsPool.h"
#include "PersistentGameData.h"
#include "PlayerController.h"
#include "PlayerUpgrade.h"
#include "SoundPlayer.h"
#include "Utils.h"
#include "UiUtils.h"

#include <cassert>

BonusShopWindow::BonusShopWindow() : _currentUpgradeCursor(0)
{}

BonusShopWindow& BonusShopWindow::instance()
{
    static BonusShopWindow _instance;
    return _instance;
}

void BonusShopWindow::open()
{
    _isOpen = true;
    SoundPlayer::instance().stopAllSounds();
    SoundPlayer::instance().playBonusCollectSound();
    globalVars::gameIsPaused = true;
    globalVars::globalChronometer.pause();
    globalVars::globalFreezeChronometer.pause();
}

void BonusShopWindow::close()
{
    globalVars::gameIsPaused = false;
    globalVars::openLevelUpMenu = false;
    globalVars::globalChronometer.resume();
    globalVars::globalFreezeChronometer.resume();
    _isOpen = false;
}

void BonusShopWindow::draw()
{
    using namespace globalConst;
    using namespace globalVars;

    constexpr int screenCenterX = screen_w / 2;
    constexpr int screenCenterY = screen_h / 2;

    constexpr int screenQuarterY = screen_h / 4;

    // draw frame
    sf::RectangleShape greyRect(sf::Vector2f(screen_w, screen_h));
    greyRect.setOrigin(greyRect.getSize().x/2, greyRect.getSize().y/2);
    greyRect.setPosition(sf::Vector2f(screenCenterX, screenCenterY));
    greyRect.setFillColor(sf::Color(102, 102, 102));
    Utils::window.draw(greyRect);

    const int centerX = greyRect.getPosition().x;
    const int menuWidth = greyRect.getSize().x;
    const int menuHeight = greyRect.getSize().y;

    int currentStringY = greyRect.getPosition().y - greyRect.getSize().y/2 - 64;


    // draw title
    const int titleFontSize = 48;
    std::string title = "Once more?";

    currentStringY += titleFontSize * 2;
    UiUtils::instance().drawText( title, titleFontSize, centerX, currentStringY);

    const int subtitleSize = 24;
    std::string subtitle = "Wanna buy an upgrade for your next run?";
    currentStringY += subtitleSize * 2;
    UiUtils::instance().drawText( subtitle, subtitleSize, centerX, currentStringY);

    const int promptSize = 20;
    /*currentStringY += subtitleSize * 2;
    std::string prompt = "Press [Enter] to buy an item, and [escape] to exit";
    UiUtils::instance().drawText( prompt, promptSize, centerX, currentStringY, false, sf::Color::Yellow); */
    currentStringY += 96;

    std::string amount = "Deposit: $" + std::to_string(PersistentGameData::instance().xpDeposit());
    const int amountSize = 16;
    UiUtils::instance().drawText( amount, amountSize, menuWidth/5-64, currentStringY, true) ;

    currentStringY += amountSize;
    std::string line = "---------------------------------------------------------------------------";
    UiUtils::instance().drawText( line, promptSize, menuWidth/5-64, currentStringY, true) ;


    // draw cursor
    currentStringY += 96;
    //drawCursor(currentStringY, greyRect);

    const int offsetX = menuWidth/5;
    const int offsetY = 240;

    // draw icons
    const int perksNum = PlayerUpgrade::availablePerkObjects.size();
    for (int i = 0; i < perksNum; i++) {
        int x = offsetX + offsetX * (i%4);
        int y = currentStringY + offsetY * (i/4);
        drawUpgrade(i, x, y);
    }

    // draw bottom

    const int bottomOffsetX = menuWidth / 4;
    const int bottomY = menuHeight - 30;

    UiUtils::instance().drawText("[escape] exit to main menu", promptSize, bottomOffsetX, bottomY, false, sf::Color::Red);
    UiUtils::instance().drawText("[enter] buy an item", promptSize, bottomOffsetX*2, bottomY, false, sf::Color::Green);
    UiUtils::instance().drawText("[space] start the game", promptSize, bottomOffsetX*3, bottomY, false, sf::Color::Yellow);

    Utils::window.display();
}


/*void BonusShopWindow::drawCursor(int startY, sf::RectangleShape& parentRect)
{
    const int menuWidth = parentRect.getSize().x;
    const int offsetX = menuWidth/5;
    const int offsetY = 240;

    const int i = _currentUpgradeCursor;

    int cursorX = offsetX + offsetX * (i%4);
    int cursorY = startY + offsetY * (i/4);

    sf::RectangleShape whiteRect(sf::Vector2f(18, 18));
    whiteRect.setScale(globalConst::spriteScaleX+1, globalConst::spriteScaleY+1);
    whiteRect.setOrigin(whiteRect.getSize().x/2, whiteRect.getSize().y/2);
    whiteRect.setPosition(cursorX, cursorY);
    whiteRect.setFillColor(sf::Color(200, 200, 200));

    Utils::window.draw(whiteRect);
}*/

void BonusShopWindow::drawCursor(int x,  int y)
{
    sf::RectangleShape whiteRect(sf::Vector2f(18, 18));
    whiteRect.setScale(globalConst::spriteScaleX+1, globalConst::spriteScaleY+1);
    whiteRect.setOrigin(whiteRect.getSize().x/2, whiteRect.getSize().y/2);
    whiteRect.setPosition(x, y);
    whiteRect.setFillColor(sf::Color(200, 200, 200));

    Utils::window.draw(whiteRect);
}

void BonusShopWindow::drawEdging(int x,  int y)
{
    sf::RectangleShape whiteRect(sf::Vector2f(18, 18));
    whiteRect.setScale(globalConst::spriteScaleX+2, globalConst::spriteScaleY+2);
    whiteRect.setOrigin(whiteRect.getSize().x/2, whiteRect.getSize().y/2);
    whiteRect.setPosition(x, y);
    whiteRect.setFillColor(sf::Color(31, 81, 43));

    Utils::window.draw(whiteRect);
}

void BonusShopWindow::drawUpgrade(int index, int x, int y)
{
    //assert(index >=0 && index < PlayerUpgrade::currentThreeRandomUpgrades.size());
    //auto upgrade = PlayerUpgrade::currentThreeRandomUpgrades[index];


    assert(index < PlayerUpgrade::availablePerkObjects.size());
    auto upgrade = PlayerUpgrade::availablePerkObjects.at(index);

    // draw edging of bought
    if (PlayerUpgrade::playerOwnedPerks.contains(upgrade->type())) {
        drawEdging(x, y);
    }

    if (index == _currentUpgradeCursor)
        drawCursor(x, y);

    sf::IntRect iconRect = upgrade->iconRect();//sf::IntRect(272, 16, 16, 16);
    UiUtils::instance().drawIcon(iconRect, x, y);

    std::string caption = upgrade->name();
    const int captionFontSize = 18;
    int offsetY = y + 40 + captionFontSize;
    UiUtils::instance().drawText(caption, captionFontSize, x, offsetY);
    offsetY += captionFontSize + 5;
    int priceSize = 14;


    std::string price = "price: " + std::to_string(upgrade->price());
    sf::Color priceColor = (PersistentGameData::instance().xpDeposit() > upgrade->price()) ? sf::Color::Green : sf::Color::Red;
    UiUtils::instance().drawText(price, priceSize, x, offsetY, false, priceColor);

    std::string description = upgrade->currentEffectDescription();// "start with lvl.1 armor upgrade\npreserve it upon death";
    const int descriptionFontSize = 14;
    offsetY += captionFontSize + descriptionFontSize;
    UiUtils::instance().drawText(description, descriptionFontSize, x, offsetY);

}

void BonusShopWindow::moveCursorLeft()
{
    SoundPlayer::instance().playTickSound();

    if (_currentUpgradeCursor%4 == 0)
        _currentUpgradeCursor += 3;
    else
        --_currentUpgradeCursor;
}

void BonusShopWindow::moveCursorRight()
{
    SoundPlayer::instance().playTickSound();
    _currentUpgradeCursor++;
    if (_currentUpgradeCursor%4 == 0)
        _currentUpgradeCursor -= 4;
}

void BonusShopWindow::moveCursorUp()
{
    const int perksNum = PlayerUpgrade::availablePerkObjects.size();
    const int lastRow = (perksNum-1)/4;
    SoundPlayer::instance().playTickSound();
    if (_currentUpgradeCursor < 4)
        _currentUpgradeCursor += lastRow*4;
    else
        _currentUpgradeCursor -= 4;
}

void BonusShopWindow::moveCursorDown()
{
    const int perksNum = PlayerUpgrade::availablePerkObjects.size();
    const int lastRow = (perksNum-1)/4;
    SoundPlayer::instance().playTickSound();
    if (_currentUpgradeCursor >= lastRow*4)
        _currentUpgradeCursor -= lastRow*4;
    else
        _currentUpgradeCursor += 4;
}

void BonusShopWindow::getSelectedUpgrade()
{
    assert(_currentUpgradeCursor < PlayerUpgrade::availablePerkObjects.size());

    const auto upgrade = PlayerUpgrade::availablePerkObjects.at(_currentUpgradeCursor);
    const PlayerUpgrade::UpgradeType perkType = upgrade->type();

    if (!PlayerUpgrade::playerOwnedPerks.contains(perkType)) {
        if (PersistentGameData::instance().xpDeposit() > upgrade->price()) {
            // if can afford
            PersistentGameData::instance().subtractFromDeposit(upgrade->price());
            PlayerUpgrade::playerOwnedPerks.insert(perkType);
            SoundPlayer::instance().playShootSound();
        } else {
            // if cannot afford
            SoundPlayer::instance().playIceSkidSound();
        }
    } else {
        PersistentGameData::instance().addToXpDeposit(upgrade->price());
        PlayerUpgrade::playerOwnedPerks.erase(perkType);
        SoundPlayer::instance().playBulletHitWallSound();
    }
}