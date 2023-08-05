#include "SpriteRenderer.h"
#include "GameObject.h"
#include "GlobalConst.h"
#include "AssetManager.h"
#include "Utils.h"


#include <cassert>
#include <SFML/System/Time.hpp>

using namespace Assets;

SpriteRenderer::SpriteRenderer(GameObject * parent)
: _parentObject(parent)
, _objectType(parent->type())
{
    _sprite.setTexture(AssetManager::instance().mainSpriteSheetTexture());
    setCurrentAnimation("default");
}

void SpriteRenderer::setCurrentAnimation(std::string id)
{
    assert(id.empty() == false);

    if (id == _currentAnimation)
        return;

    _currentAnimation = id;
    _currentAnimationFrames = AssetManager::instance().getAnimationFrames(_objectType, _currentAnimation);

    showAnimationFrame(0);
    playAnimation(true);
    _clock.restart();
}

void SpriteRenderer::showAnimationFrame(int frameNum)
{
    assert(_currentAnimation.empty() == false);

    _currentFrame = frameNum;

    auto& frame = _currentAnimationFrames[_currentFrame];

    _sprite.setTextureRect(frame.rect);
    _sprite.setScale(globalConst::spriteScaleX, globalConst::spriteScaleY);
    _sprite.setOrigin(frame.rect.width/2, frame.rect.height/2);
}

void SpriteRenderer::draw()
{
    int framesCount = _currentAnimationFrames.size();
    // play set next frame if duration of current frame passed
    if (framesCount > 1 && _animate) {
        if (_clock.getElapsedTime() > sf::milliseconds(_currentAnimationFrames[_currentFrame].duration)) {
            int nextFrame = _currentFrame+1 < framesCount ? _currentFrame+1 : 0;
            showAnimationFrame(nextFrame);
            _clock.restart();
        }
    }
    Utils::window.draw(_sprite);
}

void SpriteRenderer::playAnimation(bool play)
{
    _animate = play;
}

OneShotAnimationRenderer::OneShotAnimationRenderer(GameObject * parent) : SpriteRenderer(parent) {}

void OneShotAnimationRenderer::draw()
{
    int framesCount = _currentAnimationFrames.size();
    // play set next frame if duration of current frame passed
    if (framesCount > 1 && _animate) {
        if (_clock.getElapsedTime() > sf::milliseconds(_currentAnimationFrames[_currentFrame].duration)) {
            int nextFrame = _currentFrame+1;

            if (nextFrame == framesCount) {
                _parentObject->_deleteme = true;
                return;
            }
            showAnimationFrame(nextFrame);
            _clock.restart();
        }
    }
    Utils::window.draw(_sprite);
}