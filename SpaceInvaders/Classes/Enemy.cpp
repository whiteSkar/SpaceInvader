#include "Enemy.h"

USING_NS_CC;

Enemy* Enemy::create(std::vector<Sprite*> frames)
{
    Enemy *object = new Enemy();
    if (object && object->initWithFrames(frames))
    {
        object->autorelease();
        return object;
    }
    CC_SAFE_DELETE(object);
    return nullptr;
}

bool Enemy::initWithFrames(std::vector<Sprite*> frames)
{
    bool result;
    if (AnimatableObject::initWithFrames(frames))
    {
        _missileShootElapsedTime = 0.0f;
        _nextMissileTimeInterval = ((float) (rand() % ((ENEMY_MISSILE_INTERVAL_MAX - ENEMY_MISSILE_INTERVAL_MIN) * 10))) / 10.0f + ENEMY_MISSILE_INTERVAL_MIN; // duplicate code
        _isAtFrontLine = false;

        _missile = Sprite::create("missile.png");   // possibly use different missile image for enemy
        _missile->setVisible(false);
        this->addChild(_missile);

        this->scheduleUpdate();

        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

void Enemy::update(float dt)
{
    if (!this->isAlive()) return;

    if (_isAtFrontLine)
    {
        _missileShootElapsedTime += dt;
    }

    if (_missileShootElapsedTime >= _nextMissileTimeInterval && !_missile->isVisible() && _isAtFrontLine)
    {
        _missileShootElapsedTime = 0.0f;
        _nextMissileTimeInterval = ((float) (rand() % ((ENEMY_MISSILE_INTERVAL_MAX - ENEMY_MISSILE_INTERVAL_MIN) * 10))) / 10.0f + ENEMY_MISSILE_INTERVAL_MIN;

        _missile->setPosition(Point(0, 0 - this->getSize().height/2 - _missile->getBoundingBox().size.height/2));   // missile moves sideways along with the enemy... Fix this
        _missile->setVisible(true);
    }

    if (_missile->isVisible())
    {
        _missile->setPositionY(_missile->getPositionY() - MISSILE_SPEED * dt);
    }
}

Rect Enemy::getMissileBoundingBox()
{
    auto box = _missile->getBoundingBox();
    box.origin = this->convertToWorldSpace(box.origin);
    return box;
}

bool Enemy::isMissileVisible()
{
    return _missile->isVisible();
}

void Enemy::missileHit()
{
    _missile->setVisible(false);
}

void Enemy::missileOutOfBound()
{
    _missile->setVisible(false);
}

void Enemy::setAlive(bool isAlive)
{
    AnimatableObject::setAlive(isAlive);
    _isAtFrontLine = false;
}

void Enemy::setAtFrontLine(bool isAtFrontLine)
{
    _isAtFrontLine = isAtFrontLine;
}

//void Enemy::draw (Renderer* renderer, const kmMat4& transform, bool transformUpdated)
//{
//    auto box = this->getBoundingBox();
//    Node::draw(renderer, transform, transformUpdated);
//    ccDrawColor4B(255, 0, 0, 255);
//    ccDrawRect(Point::ZERO, Point(box.size.width,  box.size.height));
//}
