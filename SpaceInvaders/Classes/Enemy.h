#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "cocos2d.h"

static const int ENEMY_MISSILE_INTERVAL_MAX = 7;
static const int ENEMY_MISSILE_INTERVAL_MIN = 3;

static const float MISSILE_SPEED = 600.0f;

class Enemy : public cocos2d::Node
{
public:
    static Enemy* create(cocos2d::Sprite *sprite, cocos2d::Sprite *sprite2); // only need two sprites. This way suffices

    bool initWithSprites(cocos2d::Sprite *sprite, cocos2d::Sprite *sprite2);

    cocos2d::Rect getBoundingBox();
    cocos2d::Size getSize();

    //virtual void draw (cocos2d::Renderer* renderer, const kmMat4& transform, bool transformUpdated);    // debugging purpose

    void update(float dt);

    cocos2d::Sprite* getMissile();  // I don't like this method. What is the best way to check collision between this missile and the player?
    void missileHit();
    void missileOutOfBound();

    bool isAlive();
    void setAlive(bool isAlive);

    void animateToNextFrame();

private:
    cocos2d::Sprite *_sprite;
    cocos2d::Sprite *_sprite2;

    cocos2d::Sprite *_missile;

    float missileShootElapsedTime;
    float nextMissileTimeInterval;
    
    bool _isAlive;
};

#endif