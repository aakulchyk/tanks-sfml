#include "GameObject.h"
#include "Logger.h"
#include "Controller.h"
#include "GlobalConst.h"
#include "ObjectsPool.h"
#include "Utils.h"
#include "Shootable.h"

#include <iostream>


GameObject::GameObject(std::string type)
: _type(type)
{
    assignUniqueId();
}

GameObject::GameObject(GameObject *parent, std::string type)
: _parentObject(parent), _type(type)
{
    assignUniqueId();
}

void GameObject::assignUniqueId()
{
    static int count = 0;
    _id = count++;
}

GameObject::~GameObject()
{
    if (spriteRenderer)
        delete spriteRenderer;

    if (_controller)
        delete _controller;

    if (_shootable)
        delete _shootable;
}

void GameObject::setFlags(GameObject::ObjectFlags flags)
{
    _flags = flags;
}

bool GameObject::isFlagSet(GameObject::ObjectFlags f)
{
    return (_flags & f) != 0;
}

void GameObject::createSpriteRenderer()
{
    Logger::instance() << "createSpriteRenderer";
    spriteRenderer = new SpriteRenderer(this);
}

void GameObject::draw()
{
    if (_deleteme)
        return;

    if (spriteRenderer)
        spriteRenderer->draw();
    else
        Logger::instance() << "[ERROR] GameObject - no spriteRenderer found";
}

void GameObject::setPos(int x, int y)
{
    if (spriteRenderer) {
        int mappedX = x + globalVars::gameViewPort.left;
        int mappedY = y + globalVars::gameViewPort.top;
        spriteRenderer->_sprite.setPosition(mappedX, mappedY);
    }
    else
        Logger::instance() << "[ERROR] GameObject - no spriteRenderer found";
}


void GameObject::move(int x, int y)
{
    if (_deleteme)
        return;

    if (spriteRenderer) {
        spriteRenderer->_sprite.move(x, y);

        GameObject *collider = nullptr;
        bool cancelMovement = false;
        sf::IntRect thisBoundingBox = sf::IntRect(spriteRenderer->_sprite.getGlobalBounds());

        if (Utils::isOutOfBounds(thisBoundingBox)) {
            updateOnCollision(collider, cancelMovement);
        }
        else {
            for (GameObject *o : ObjectsPool::getAllObjects()) {
                if (this != o && collides(*o)) {
                    collider = o;
                    updateOnCollision(collider, cancelMovement);
                    if (collider)
                        collider->updateOnCollision(this);
                }
            }
        }

        if (cancelMovement)
            spriteRenderer->_sprite.move(-x, -y);
    }
    else
        Logger::instance() << "[ERROR] GameObject - no spriteRenderer found";
}


void GameObject::updateOnCollision(GameObject *other, bool& cancelMovement)
{
    bool isBullet = isFlagSet(Bullet);

    if (other == nullptr) {
        // bullet just fled out of bounds
        if (isBullet)
            _deleteme = true;
        cancelMovement = true;
        return;
    }

    assert(other != nullptr);

    if (isBullet) {
        // just hit non-transparent target (and it's not its own creator)
        if (!other->isFlagSet(BulletPassable) && !isFlagSet(PiercingBullet) && _parentId != other->id()) {
            _deleteme = true;
        }

        if (other->isFlagSet(Bullet))
            _deleteme = true;

        return;
    }

    assert(isBullet == false);

    // is Hit by bullet
    if (other->isFlagSet(Bullet)) {
        if (isFlagSet(BulletKillable)) {
            bool friendlyFire = false;
            // check if its my own bullet
            if (id() == other->_parentId)
                friendlyFire = true;
            // check if I'm NPC and bullet is from another NPC (friendly fire)
            if (isFlagSet(NPC)) {
                GameObject *bulletAuthor = ObjectsPool::findNpcById(other->_parentId);
                if (bulletAuthor && bulletAuthor->isFlagSet(NPC))
                    friendlyFire = true;
            }

            if (!friendlyFire) {
                _deleteme = true;
                cancelMovement = true;
            }
        }
        return;
    }

    // just run into wall or another tank
    if (!other->isFlagSet(TankPassable)) {
        cancelMovement = true;
    }
}

void GameObject::updateOnCollision(GameObject *other)
{
    bool _;
    updateOnCollision(other, _);
}

sf::Vector2i GameObject::position() const
{
    return sf::Vector2i(spriteRenderer->_sprite.getPosition());
}

void GameObject::copyParentPosition(const GameObject * parent)
{
    auto pos = parent->position();
    spriteRenderer->_sprite.setPosition(pos.x, pos.y);
}

sf::IntRect GameObject::boundingBox() const
{
    sf::IntRect thisBoundingBox = sf::IntRect(spriteRenderer->_sprite.getGlobalBounds());
    // reduct rect
    thisBoundingBox.left += 2;
    thisBoundingBox.top += 2;
    thisBoundingBox.width -= 4;
    thisBoundingBox.height -= 4;

    return thisBoundingBox;
}

bool GameObject::collides(const GameObject& go) const
{
    auto thisBB = boundingBox();
    auto otherBB = go.boundingBox();

    using v2f = sf::Vector2i;

    bool intersects
            =  thisBB.contains(v2f(otherBB.left, otherBB.top))
            || thisBB.contains(v2f(otherBB.left + otherBB.width, otherBB.top))
            || thisBB.contains(v2f(otherBB.left + otherBB.width, otherBB.top + otherBB.height))
            || thisBB.contains(v2f(otherBB.left, otherBB.top + otherBB.height))
            || otherBB.contains(v2f(thisBB.left, thisBB.top))
            || otherBB.contains(v2f(thisBB.left + thisBB.width, thisBB.top))
            || otherBB.contains(v2f(thisBB.left + thisBB.width, thisBB.top + thisBB.height))
            || otherBB.contains(v2f(thisBB.left, thisBB.top + thisBB.height));

    return intersects;
}


void GameObject::setCurrentAnimation(std::string animName)
{
    if (spriteRenderer)
        spriteRenderer->setCurrentAnimation(animName);
    else
        Logger::instance() << "[ERROR] GameObject - no spriteRenderer found";
}

void GameObject::stopAnimation()
{
    if (spriteRenderer)
        spriteRenderer->playAnimation(false);
}

void GameObject::restartAnimation()
{
    if (spriteRenderer)
        spriteRenderer->playAnimation(true);
}

void GameObject::setController(Controller * ctrl)
{
    _controller = ctrl;
}


void GameObject::setShootable(Shootable * shtbl)
{
    _shootable = shtbl;
}

void GameObject::setCurrentDirection(globalTypes::Direction dir)
{
    _direction = dir;
}

bool GameObject::shoot()
{
    if (_shootable)
        return _shootable->shoot(_direction);
    else
        return false;
}


void GameObject::update()
{
    if (_controller)
        _controller->update();
    else
        Logger::instance() << "[ERROR] GameObject::update - no controller found";
}

