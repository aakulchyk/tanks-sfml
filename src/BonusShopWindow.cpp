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



BonusShopWindow::BonusShopWindow()
{
    if (!_texture.loadFromFile("assets/shop-window.png")) {
        Logger::instance() << " failed to load texture\n";
        return;
    }

    _sprite.setTexture(_texture);
    _sprite.setScale(8, 8);

    const int perksNum = PlayerUpgrade::availablePerkObjects.size();
    const int lastRow = (perksNum-1)/4;
    buttonExitCursorPos = (lastRow+1)*4 + 1;
    buttonStartCursorPos = (lastRow+1)*4 + 2;

    _currentUpgradeCursor = buttonStartCursorPos;
}

BonusShopWindow& BonusShopWindow::instance()
{
    static BonusShopWindow _instance;
    return _instance;
}

void BonusShopWindow::open()
{
    _isOpen = true;
    SoundPlayer::instance().stopAllSounds();
    SoundPlayer::instance().playSound(SoundPlayer::SoundType::bonusCollect);

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


    Utils::window.draw(_sprite);

    const int centerX = screenCenterX;//greyRect.getPosition().x;
    const int menuWidth = _sprite.getLocalBounds().width * 8;
    const int menuHeight = _sprite.getLocalBounds().height * 8;

    int currentStringY = 64;


    // draw title
    const int titleFontSize = 48;
    std::string title = afterGameOver ? "Once more?" : afterWin ? "Another challenge?" : "Welcome!";

    currentStringY += titleFontSize * 2;
    UiUtils::instance().drawText( title, titleFontSize, centerX, currentStringY);

    const int subtitleSize = 24;
    std::string subtitle = "Wanna buy a perk for your next run?";
    currentStringY += subtitleSize * 2;
    UiUtils::instance().drawText( subtitle, subtitleSize, centerX, currentStringY);

    const int promptSize = 20;

    currentStringY += 96;

    std::string amount = "Deposit: $" + std::to_string(PersistentGameData::instance().xpDeposit());
    const int amountSize = 20;
    UiUtils::instance().drawText( amount, amountSize, menuWidth/5-64, currentStringY, true) ;

    currentStringY += amountSize;
    UiUtils::instance().drawHorizontalLine(centerX, currentStringY, (1920-256) - 64, sf::Color::White);

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
    const int buttonY = menuHeight - 190;

    if (_currentUpgradeCursor == buttonExitCursorPos)
        drawCursorOnButton(bottomOffsetX, buttonY);
    else if (_currentUpgradeCursor == buttonStartCursorPos)
        drawCursorOnButton(bottomOffsetX*3, buttonY);

    auto rectDefault = AssetManager::instance().getAnimationFrame("button", "default", 0).rect;
    auto rectSelected = AssetManager::instance().getAnimationFrame("button", "selected", 0).rect;

    UiUtils::instance().drawIcon(_currentUpgradeCursor == buttonExitCursorPos ? rectSelected : rectDefault, bottomOffsetX, buttonY);
    UiUtils::instance().drawIcon(_currentUpgradeCursor == buttonStartCursorPos ? rectSelected : rectDefault, bottomOffsetX*3, buttonY);

    UiUtils::instance().drawText("exit", promptSize+4, bottomOffsetX, buttonY, false, sf::Color::Red);
    UiUtils::instance().drawText("start", promptSize+4, bottomOffsetX*3, buttonY, false, sf::Color::Green);

    UiUtils::instance().drawText("press on the item to buy it", promptSize, bottomOffsetX*2, bottomY, false, sf::Color(31, 81, 43));
    Utils::window.display();
}

void BonusShopWindow::drawCursor(int x,  int y)
{
    sf::RectangleShape whiteRect(sf::Vector2f(globalConst::spriteOriginalSizeX+2, globalConst::spriteOriginalSizeY+2));
    whiteRect.setScale(globalConst::spriteScaleX+1, globalConst::spriteScaleY+1);
    whiteRect.setOrigin(whiteRect.getSize().x/2, whiteRect.getSize().y/2);
    whiteRect.setPosition(x, y);
    whiteRect.setFillColor(sf::Color(200, 200, 200));

    Utils::window.draw(whiteRect);
}

void BonusShopWindow::drawCursorOnButton(int x,  int y)
{
    const auto rect = AssetManager::instance().getAnimationFrame("button", "default", 0).rect;

    sf::RectangleShape whiteRect(sf::Vector2f(rect.getSize().x+2, rect.getSize().y+2));
    whiteRect.setScale(globalConst::spriteScaleX+1, globalConst::spriteScaleY+1);
    whiteRect.setOrigin(whiteRect.getSize().x/2, whiteRect.getSize().y/2);
    whiteRect.setPosition(x, y);
    whiteRect.setFillColor(sf::Color(200, 200, 200));

    Utils::window.draw(whiteRect);
}

void BonusShopWindow::drawEdging(int x,  int y)
{
    sf::RectangleShape whiteRect(sf::Vector2f(globalConst::spriteOriginalSizeX+2, globalConst::spriteOriginalSizeY+2));
    whiteRect.setScale(globalConst::spriteScaleX+2, globalConst::spriteScaleY+2);
    whiteRect.setOrigin(whiteRect.getSize().x/2, whiteRect.getSize().y/2);
    whiteRect.setPosition(x, y);
    whiteRect.setFillColor(sf::Color(31, 81, 43));

    Utils::window.draw(whiteRect);
}

void BonusShopWindow::drawUpgrade(int index, int x, int y)
{
    assert(index < PlayerUpgrade::availablePerkObjects.size());
    auto upgrade = PlayerUpgrade::availablePerkObjects.at(index);

    // draw edging of bought
    if (PlayerUpgrade::playerOwnedPerks.contains(upgrade->type())) {
        drawEdging(x, y);
    }

    if (index == _currentUpgradeCursor)
        drawCursor(x, y);

    sf::IntRect iconRect = upgrade->iconRect();
    UiUtils::instance().drawIcon(iconRect, x, y);

    std::string caption = upgrade->name();
    const int captionFontSize = 18;
    int offsetY = y + 40 + captionFontSize*2;
    UiUtils::instance().drawText(caption, captionFontSize, x, offsetY);
    offsetY += captionFontSize + 5;
    int priceSize = 14;


    std::string price = "price: " + std::to_string(upgrade->price());
    sf::Color priceColor = (PersistentGameData::instance().xpDeposit() > upgrade->price()) ? sf::Color::Green : sf::Color::Red;
    UiUtils::instance().drawText(price, priceSize, x, offsetY, false, priceColor);

    std::string description = upgrade->currentEffectDescription(); // "start with lvl.1 armor upgrade\npreserve it upon death";
    const int descriptionFontSize = 14;
    offsetY += captionFontSize + descriptionFontSize;
    UiUtils::instance().drawText(description, descriptionFontSize, x, offsetY);

}

void BonusShopWindow::moveCursorLeft()
{
    SoundPlayer::instance().playSound(SoundPlayer::SoundType::tick);
    const int perksNum = PlayerUpgrade::availablePerkObjects.size();
    const int lastRow = (perksNum-1)/4;
    if (_currentUpgradeCursor == buttonExitCursorPos)
        _currentUpgradeCursor++;
    else if (_currentUpgradeCursor == buttonStartCursorPos)
        _currentUpgradeCursor--;
    else {
        if (_currentUpgradeCursor%4 == 0)
            _currentUpgradeCursor += 3;
        else
            --_currentUpgradeCursor;
    }
}

void BonusShopWindow::moveCursorRight()
{
    SoundPlayer::instance().playSound(SoundPlayer::SoundType::tick);
    const int perksNum = PlayerUpgrade::availablePerkObjects.size();
    const int lastRow = (perksNum-1)/4;

    if (_currentUpgradeCursor == buttonExitCursorPos)
        _currentUpgradeCursor++;
    else if (_currentUpgradeCursor == buttonStartCursorPos)
        _currentUpgradeCursor--;
    else {
        _currentUpgradeCursor++;
        if (_currentUpgradeCursor%4 == 0)
            _currentUpgradeCursor -= 4;
    }
}

void BonusShopWindow::moveCursorUp()
{
    const int perksNum = PlayerUpgrade::availablePerkObjects.size();
    const int lastRow = (perksNum-1)/4;
    SoundPlayer::instance().playSound(SoundPlayer::SoundType::tick);
    if (_currentUpgradeCursor < 4) {
        if (_currentUpgradeCursor > 1)
            _currentUpgradeCursor = buttonStartCursorPos;
        else
            _currentUpgradeCursor = buttonExitCursorPos;
        //_currentUpgradeCursor += lastRow*4;
    }
    else {
        _currentUpgradeCursor -= 4;
        if (_currentUpgradeCursor >= perksNum)
            _currentUpgradeCursor = perksNum-1;
    }
}

void BonusShopWindow::moveCursorDown()
{
    const int perksNum = PlayerUpgrade::availablePerkObjects.size();
    const int lastRow = (perksNum-1)/4;
    const int col = _currentUpgradeCursor % 4;
    SoundPlayer::instance().playSound(SoundPlayer::SoundType::tick);
    if (_currentUpgradeCursor >= lastRow*4) {
        if (_currentUpgradeCursor >= buttonExitCursorPos)
            _currentUpgradeCursor -= lastRow*4;
        else {
            if (col > 1)
                _currentUpgradeCursor = buttonStartCursorPos;
            else
                _currentUpgradeCursor = buttonExitCursorPos;
        }
    }
    else {
        _currentUpgradeCursor += 4;
        if (_currentUpgradeCursor >= perksNum)
            _currentUpgradeCursor = perksNum-1;
    }



    if (_currentUpgradeCursor > buttonStartCursorPos)
        _currentUpgradeCursor = buttonStartCursorPos;
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
            SoundPlayer::instance().playSound(SoundPlayer::SoundType::Shoot);
            upgrade->reset();
        } else {
            // if cannot afford
            SoundPlayer::instance().playSound(SoundPlayer::SoundType::iceSkid);
        }
    } else {
        PersistentGameData::instance().addToXpDeposit(upgrade->price());
        PlayerUpgrade::playerOwnedPerks.erase(perkType);
        SoundPlayer::instance().playSound(SoundPlayer::SoundType::bulletHitWall);
    }
}

void BonusShopWindow::processKeyboardPress(sf::Keyboard::Scancode scancode, globalTypes::GameState& gameState)
{
    if (scancode == sf::Keyboard::Scan::Left)
        moveCursorLeft();
    else if (scancode == sf::Keyboard::Scan::Right)
        moveCursorRight();
    else if (scancode == sf::Keyboard::Scan::Down)
        moveCursorDown();
    else if (scancode == sf::Keyboard::Scan::Up)
        moveCursorUp();
    else if (scancode == sf::Keyboard::Scan::Escape) {
        SoundPlayer::instance().playSound(SoundPlayer::SoundType::bulletHitWall);
        SoundPlayer::instance().stopSound(SoundPlayer::ShopTheme);
        SoundPlayer::instance().playSound(SoundPlayer::TitleTheme);
        gameState = globalTypes::GameState::TitleScreen;
    } else if (scancode == sf::Keyboard::Scan::Enter || scancode == sf::Keyboard::Scan::Space) {
        if (_currentUpgradeCursor < buttonExitCursorPos) {
            getSelectedUpgrade();
        } else if (_currentUpgradeCursor == buttonExitCursorPos) {
            SoundPlayer::instance().playSound(SoundPlayer::SoundType::bulletHitWall);
            SoundPlayer::instance().stopSound(SoundPlayer::ShopTheme);
            SoundPlayer::instance().playSound(SoundPlayer::TitleTheme);
            gameState = globalTypes::GameState::TitleScreen;
        } else if (_currentUpgradeCursor == buttonStartCursorPos) {
            SoundPlayer::instance().playSound(SoundPlayer::SoundType::bulletHitWall);
            gameState = globalTypes::GameState::SelectLevel;
        }
    }
}