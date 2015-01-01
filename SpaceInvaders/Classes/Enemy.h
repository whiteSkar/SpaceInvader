#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "cocos2d.h"

class Enemy
{
public:
    Enemy(cocos2d::Sprite *sprite, cocos2d::Sprite *sprite2);   // only need two sprites. This way suffices
    ~Enemy();

    cocos2d::Point getPosition();
    float getPositionX();
    float getPositionY();
    void setPosition(cocos2d::Point position);
    void setPositionX(float positionX);
    void setPositionY(float positionY);

    cocos2d::Rect getBoundingBox();
    cocos2d::Size getSize();

    bool isAlive();
    void setAlive(bool isAlive);

    void animateToNextFrame();

private:
    cocos2d::Sprite *_sprite;
    cocos2d::Sprite *_sprite2;
    
    bool _isAlive;
};

#endif