#include "Damageable.h"
#include "EagleController.h"
#include "GameObject.h"
#include "GlobalConst.h"
#include "ObjectsPool.h"
#include "MapCreator.h"
#include "SoundPlayer.h"
#include "SpriteRenderer.h"

using namespace globalTypes;
using namespace sf;

static const std::map<EagleWallDirection, sf::Vector2i> dirOffsets = {
    {DownLeft, Vector2i(-12, 12)},
    {LeftLeftDown, Vector2i(-12, 4)},
    {LeftLeftUp, Vector2i(-12, -4)},
    {UpLeft, Vector2i(-12, -12)},
    {UpUpLeft, Vector2i(-4, -12)},
    {UpUpRight, Vector2i(4, -12)},
    {UpRight, Vector2i(12, -12)},
    {RightRightUp, Vector2i(12, -4)},
    {RightRightDown, Vector2i(12, 4)},
    {DownRight, Vector2i(12, 12)},
    {DownDownRight, Vector2i(4, 12)},
    {DownDownLeft, Vector2i(-4, 12)}
};

EagleController::EagleController(GameObject *obj)
: Controller(obj, 0), _state(Starting)
{}

EagleController::~EagleController()
{
    for (auto it = _collectedUpgrades.begin(); it != _collectedUpgrades.end(); ) {
        PlayerUpgrade *obj = (*it).second;
        it = _collectedUpgrades.erase(it);
        delete obj;
    }
}

constexpr int initialWallsBuildTimeout = 100;

void EagleController::update()
{
    checkForGamePause();
    if (_pause) return;

    if (_invincible) {
        if (_invincibilityTimer.getElapsedTime() < sf::seconds(_invincibilityAfterDamageTimeout)) {
            _gameObject->visualEffect->copyParentPosition(_gameObject);
        } else {
            _invincible = false;
            delete _gameObject->visualEffect;
            _gameObject->visualEffect = nullptr;
            Damageable *d = _gameObject->getComponent<Damageable>();
            d->makeInvincible(false);
        }
    }


    switch (_state) {
        case Starting:
            _currentBuildDirection = globalTypes::EagleWallDirection::DownLeft;
            _state = BuildingWalls;
            _clock.reset(true);
            _rebuildTimeouts.push(initialWallsBuildTimeout);
            break;
        case BuildingWalls:
            assert(!_rebuildTimeouts.empty());
            if (_clock.getElapsedTime() > sf::milliseconds(_rebuildTimeouts.top())) {
                _clock.reset(true);
                if (false == ObjectsPool::getEagleWalls().contains(_currentBuildDirection)) {
                    SoundPlayer::instance().playDebuffSound();
                    const int thisX = _gameObject->position().x;
                    const int thisY = _gameObject->position().y;
                    const auto offset = dirOffsets.at(_currentBuildDirection);
                    Logger::instance() << "build dir " << (int)_currentBuildDirection << " coord " << thisX + offset.x << " " << thisY + offset.y << "\n";
                    GameObject *obj = MapCreator::buildObject("brickWall1x1");
                    assert(obj != nullptr);
                    obj->setSize(8, 8);
                    obj->setPosition(
                    thisX + offset.x, thisY + offset.y);
                    ObjectsPool::addEagleWall(_currentBuildDirection, obj);
                }

                _currentBuildDirection = static_cast<EagleWallDirection>((int)_currentBuildDirection + 1);
                if (_currentBuildDirection == EagleWallDirection::MaxDirection) {
                    if (_rebuildTimeouts.size() > 1 || !_isSlowRepairMode)
                        _rebuildTimeouts.pop();
                    _state = Waiting;
                }
            }
            break;
        case Waiting:
            break;
    }

}

void EagleController::fastRepairWalls(int timeout)
{
    _currentBuildDirection = globalTypes::EagleWallDirection::DownLeft;
    _rebuildTimeouts.push(timeout);
    _state = BuildingWalls;
}

void EagleController::setSlowRepairMode(int timeout)
{

    _currentBuildDirection = globalTypes::EagleWallDirection::DownLeft;
    _rebuildTimeouts.push(timeout);
    _state = BuildingWalls;
    _isSlowRepairMode = true;
}

void EagleController::upgrade(PlayerUpgrade *upgrade)
{
    assert(upgrade != nullptr);

    auto tp = upgrade->type();
    int lvl = hasLevelOfUpgrade(tp);
    // TODO magic names
    if (lvl > -1 && lvl < 3) {
        _collectedUpgrades[tp]->increaseLevel();
    } else {
        _collectedUpgrades[tp] = upgrade;
    }
}

void EagleController::applyUpgrades()
{
    // re-new all bonuses (like, armor protection etc)
    for (auto it : _collectedUpgrades) {
        it.second->onCollect(_gameObject);
    }

    // restore basic defence if needed
    Damageable *damageable = _gameObject->getComponent<Damageable>();
    assert(damageable != nullptr);
    if (damageable->defence() < globalConst::DefaultBaseProtection)
        damageable->setDefence(globalConst::DefaultBaseProtection);

    updateAppearance();
}

void EagleController::updateAppearance()
{
    SpriteRenderer *renderer = _gameObject->getComponent<SpriteRenderer>();
    assert(renderer != nullptr);
    Damageable *damageable = _gameObject->getComponent<Damageable>();
    assert(damageable != nullptr);

    switch (damageable->defence()) {
        case 0:
            renderer->setSpriteSheetOffset(-16, 16);
            break;
        case 1:
            renderer->setSpriteSheetOffset(0, 0);
            break;
        case 2:
            renderer->setSpriteSheetOffset(0, 16);
            break;
        case 3:
            renderer->setSpriteSheetOffset(0, 32);
            break;
        case 4:
            renderer->setSpriteSheetOffset(0, 32);
            break;
        case 5:
            renderer->setSpriteSheetOffset(0, 32);
            break;
    }

    renderer->showAnimationFrame(0);
}





int EagleController::hasLevelOfUpgrade(PlayerUpgrade::UpgradeType type) const
{
    if (_collectedUpgrades.find(type) == _collectedUpgrades.end())
        return -1;
    else
        return _collectedUpgrades.at(type)->currentLevel();
}

int EagleController::numberOfUpgrades() const
{
    return _collectedUpgrades.size();
}

PlayerUpgrade *EagleController::getUpgrade(int index) const
{
    assert(index < _collectedUpgrades.size());

    int i = 0;
    for (auto u : _collectedUpgrades) {
        if (i == index)
            return u.second;
        i++;
    }

    return nullptr;
}

void EagleController::setTempInvincibilityAfterDamage(int timeout)
{
    _invincibilityAfterDamageHit = true;
    _invincibilityAfterDamageTimeout = timeout;
}

void EagleController::onDamaged()
{
    updateAppearance();
    SoundPlayer::instance().playDebuffSound();

    if (_invincibilityAfterDamageHit) {
        _invincible = true;
        _invincibilityTimer.reset(true);
        Damageable *dmg = _gameObject->getComponent<Damageable>();
        dmg->makeInvincible(true);

        if (_gameObject->visualEffect == nullptr) {
            GameObject *cloud = new GameObject(_gameObject, "cloud");
            cloud->setFlags(GameObject::TankPassable | GameObject::BulletPassable);
            cloud->setRenderer(new LoopAnimationSpriteRenderer(cloud, "cloud"));
            _gameObject->visualEffect = cloud;
        }
    }
}