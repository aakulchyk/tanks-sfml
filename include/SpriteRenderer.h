#pragma once

#include <unordered_map>
#include <cassert>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "AssetManager.h"

class GameObject;

class SpriteRenderer
{
    friend class GameObject;

private:
    bool _hide = false;
protected:
    GameObject *_parentObject;

    std::string _objectType = "";
    std::string _currentAnimation = "";

    std::vector<Assets::AnimationFrame> _currentAnimationFrames;
    int _currentFrame = 0;

    sf::Sprite _sprite;

    sf::Clock _clock;

    bool _animate = false;

    int _spriteSheetOffsetX = 0;
    int _spriteSheetOffsetY = 0;
public:
    SpriteRenderer(GameObject * parent, std::string type = "");
    virtual ~SpriteRenderer() {}
    void setCurrentAnimation(std::string id);
    void hide(bool);
    bool isHidden();
    virtual void draw(bool paused = false);
    void playAnimation(bool);
    void setSpriteSheetOffset(int x, int y);


protected:
    void showAnimationFrame(int i);
};

class OneShotAnimationRenderer : public SpriteRenderer
{
public:
    OneShotAnimationRenderer(GameObject * parent, std::string type = "");
    void draw(bool paused = false) override;
};

class LoopAnimationSpriteRenderer : public SpriteRenderer
{
public:
    LoopAnimationSpriteRenderer(GameObject * parent, std::string type = "");
    void draw(bool paused = false) override;
};

