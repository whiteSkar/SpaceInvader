#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "AnimatableObject.h"

static const int ENEMY_MISSILE_INTERVAL_MAX = 7;
static const int ENEMY_MISSILE_INTERVAL_MIN = 3;

static const float MISSILE_SPEED = 600.0f;

class Enemy : public AnimatableObject
{
public:
    static Enemy* create(std::vector<cocos2d::Sprite*> frames);
    
    bool initWithFrames(std::vector<cocos2d::Sprite*> frames);

    //virtual void draw (cocos2d::Renderer* renderer, const kmMat4& transform, bool transformUpdated);    // debugging purpose

    void update(float dt);

    cocos2d::Sprite* getMissile();  // I don't like this method. What is the best way to check collision between this missile and the player?
    void missileHit();
    void missileOutOfBound();

    bool isAlive();
    void setAlive(bool isAlive);

    void setAtFrontLine(bool isAtFrontLine);

private:
    cocos2d::Sprite *_missile;

    float _missileShootElapsedTime;
    float _nextMissileTimeInterval;
    
    bool _isAlive;
    bool _isAtFrontLine;
};

#endif