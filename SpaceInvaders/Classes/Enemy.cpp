#include "Enemy.h"

USING_NS_CC;

Enemy* Enemy::create(Sprite *sprite, Sprite *sprite2)
{
    Enemy *enemy = new Enemy();
    if (enemy && enemy->initWithSprites(sprite, sprite2))
    {
        enemy->autorelease();
        return enemy;
    }
    CC_SAFE_DELETE(enemy);
    return nullptr;
}

bool Enemy::initWithSprites(Sprite *sprite, Sprite *sprite2)
{
    bool result;
    if (Node::init())
    {
        _sprite = sprite;
        _sprite->setVisible(true);
        _sprite2 = sprite2;
        _sprite2->setVisible(false);

        _sprite->setPosition(Point::ZERO);
        _sprite2->setPosition(Point::ZERO);

        this->addChild(sprite);
        this->addChild(sprite2);

        _isAlive = true;
        this->setVisible(true);

        missileShootElapsedTime = 0.0f;
        nextMissileTimeInterval = ((float) (rand() % ((ENEMY_MISSILE_INTERVAL_MAX - ENEMY_MISSILE_INTERVAL_MIN) * 10))) / 10.0f + ENEMY_MISSILE_INTERVAL_MIN; // duplicate code

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

    missileShootElapsedTime += dt;
    if (missileShootElapsedTime >= nextMissileTimeInterval && !_missile->isVisible())
    {
        missileShootElapsedTime = 0.0f;
        nextMissileTimeInterval = ((float) (rand() % ((ENEMY_MISSILE_INTERVAL_MAX - ENEMY_MISSILE_INTERVAL_MIN) * 10))) / 10.0f + ENEMY_MISSILE_INTERVAL_MIN;

        _missile->setPosition(Point(0, 0 - this->getSize().height/2 - _missile->getBoundingBox().size.height/2));
        _missile->setVisible(true);
    }

    if (_missile->isVisible())
    {
        _missile->setPositionY(_missile->getPositionY() - MISSILE_SPEED * dt);
    }
}

Sprite *Enemy::getMissile()
{
    return _missile;
}

void Enemy::missileHit()
{
    _missile->setVisible(false);
}

void Enemy::missileOutOfBound()
{
    _missile->setVisible(false);
}

Rect Enemy::getBoundingBox()
{
    auto box = _sprite->getBoundingBox();
    box.origin = Point(Node::getPositionX() - box.size.width/2, Node::getPositionY() - box.size.height/2);
    return box;
}

Size Enemy::getSize()
{
    return _sprite->getBoundingBox().size;
}

bool Enemy::isAlive()
{
    return _isAlive;
}

void Enemy::setAlive(bool isAlive)
{
    _isAlive = isAlive;
    Node::setVisible(isAlive);
}

void Enemy::animateToNextFrame()
{
    if (!_isAlive) return;
    
    if (_sprite->isVisible())
    {
        _sprite2->setVisible(true);
        _sprite->setVisible(false);
    }
    else
    {
        _sprite->setVisible(true);
        _sprite2->setVisible(false);
    }
}

//void Enemy::draw (Renderer* renderer, const kmMat4& transform, bool transformUpdated)
//{
//    auto box = this->getBoundingBox();
//    Node::draw(renderer, transform, transformUpdated);
//    ccDrawColor4B(255, 0, 0, 255);
//    ccDrawRect(Point::ZERO, Point(box.size.width,  box.size.height));
//}
