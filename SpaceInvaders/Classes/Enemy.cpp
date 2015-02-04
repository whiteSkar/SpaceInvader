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
        _scoreValue = DEFAULT_SCORE_VALUE;

        _missile = Sprite::create("missile.png");   // possibly use different missile image for enemy
        _missile->setVisible(false);
        this->addChild(_missile);

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
    if (_missile->isVisible())  // missile should keep going even if the enemy died
    {
        _missile->setPosition(this->convertToNodeSpace(Point(missileXPos, 0.0f)).x, _missile->getPositionY() - MISSILE_SPEED * dt); // hacky way to fix missile x position
    }

    if (!this->isAlive()) return;

    if (_isAtFrontLine)
    {
        _missileShootElapsedTime += dt;
    }

    if (_missileShootElapsedTime >= _nextMissileTimeInterval && !_missile->isVisible() && _isAtFrontLine)
    {
        _missileShootElapsedTime = 0.0f;
        _nextMissileTimeInterval = ((float) (rand() % ((ENEMY_MISSILE_INTERVAL_MAX - ENEMY_MISSILE_INTERVAL_MIN) * 10))) / 10.0f + ENEMY_MISSILE_INTERVAL_MIN;

        _missile->setPosition(Point(0, 0 - this->getSize().height/2 - _missile->getBoundingBox().size.height/2));
        missileXPos = this->convertToWorldSpace(_missile->getPosition()).x;
        _missile->setVisible(true);
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

    if (!this->isAlive())
    {
        AnimatableObject::setAlive(false, true);    // need this to setAlive to the whole node after hacky way of applying setAlive to the frames only
    }
}

void Enemy::setAlive(bool isAlive)
{
    bool applyIsAliveToNode = true;
    if (isMissileVisible())
    {
        applyIsAliveToNode = false;
    }

    AnimatableObject::setAlive(isAlive, applyIsAliveToNode);    // hacky way to apply setAlive to the frames only
    _isAtFrontLine = false;
}

void Enemy::setScoreValue(int scoreValue)
{
    _scoreValue = scoreValue;
}

int Enemy::getScoreValue()
{
    return _scoreValue;
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
//    ccDrawRect(Point(-box.size.width/2, -box.size.height/2), Point(box.size.width/2,  box.size.height/2));
//}
