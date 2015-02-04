#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "AnimatableObject.h"

static const int ENEMY_MISSILE_INTERVAL_MAX = 7;
static const int ENEMY_MISSILE_INTERVAL_MIN = 3;

static const int DEFAULT_SCORE_VALUE = 10;

static const float MISSILE_SPEED = 600.0f;

class Enemy : public AnimatableObject
{
public:
    static Enemy* create(std::vector<cocos2d::Sprite*> frames);
    
    bool initWithFrames(std::vector<cocos2d::Sprite*> frames);

    //virtual void draw (cocos2d::Renderer* renderer, const kmMat4& transform, bool transformUpdated);    // debugging purpose

    void update(float dt);

    cocos2d::Rect getMissileBoundingBox();  // I don't like this method. What is the best way to check collision between this missile and the player?
    bool isMissileVisible();
    void missileHit();
    void missileOutOfBound();

    void setAlive(bool isAlive);

    void setScoreValue(int scoreValue);
    int getScoreValue();

    void setAtFrontLine(bool isAtFrontLine);

private:
    cocos2d::Sprite *_missile;
    float missileXPos;

    float _missileShootElapsedTime;
    float _nextMissileTimeInterval;

    int _scoreValue;

    bool _isAtFrontLine;
};

#endif