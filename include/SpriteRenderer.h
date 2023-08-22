#pragma once

#include <unordered_map>
#include <cassert>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "AssetManager.h"
#include "NetGameTypes.h"

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

    int _oneFrameSpriteSheetOffsetX = 0;
    int _oneFrameSpriteSheetOffsetY = 0;
    bool _oneFrameOffset = false;

    void setAnimationFrame(int frameNum, net::ThinGameObject& obj);

public:
    SpriteRenderer(GameObject * parent, std::string type = "");
    virtual ~SpriteRenderer() {}
    void setCurrentAnimation(std::string id);
    void hide(bool);
    bool isHidden();
    virtual void draw();
    virtual bool networkDraw(net::ThinGameObject&);
    void playAnimation(bool);
    void setSpriteSheetOffset(int x, int y);
    void setOneFrameSpriteSheetOffset(int x, int y);
    void showAnimationFrame(int i);
    void setNewObjectType(std::string type);
};

class OneShotAnimationRenderer : public SpriteRenderer
{
public:
    OneShotAnimationRenderer(GameObject * parent, std::string type = "");
    void draw() override;
    bool networkDraw(net::ThinGameObject&) override;
};

class LoopAnimationSpriteRenderer : public SpriteRenderer
{
public:
    LoopAnimationSpriteRenderer(GameObject * parent, std::string type = "");
    void draw() override;
    bool networkDraw(net::ThinGameObject&) override;
};

