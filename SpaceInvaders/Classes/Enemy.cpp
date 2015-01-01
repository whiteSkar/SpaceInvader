#include "Enemy.h"

USING_NS_CC;

Enemy::Enemy(Sprite *sprite, Sprite *sprite2)   // size of first sprite is the size of this object.
{
    _sprite = sprite;
    _sprite->setVisible(true);
    _sprite2 = sprite2;
    _sprite2->setVisible(false);
    //auto scene = Director::getInstance()->getRunningScene();  // I wanna do this but enemy is initialized in Scene init method. How should I do it..hm
    //scene->addChild(_sprite, 1);   // refactor zorder

    _isAlive = true;

    // Should the Sprite be added as a child to the scene outside of this object or in here?
}

Enemy::~Enemy()
{

}

Point Enemy::getPosition()
{
    return _sprite->getPosition();
}

float Enemy::getPositionX()
{
    return _sprite->getPositionX();
}

float Enemy::getPositionY()
{
    return _sprite->getPositionY();
}

void Enemy::setPosition(cocos2d::Point position)
{
    if (!_isAlive) return;

    _sprite->setPosition(position);
    _sprite2->setPosition(position);
}

void Enemy::setPositionX(float positionX)
{
    if (!_isAlive) return;

    _sprite->setPositionX(positionX);
    _sprite2->setPositionX(positionX);
}

void Enemy::setPositionY(float positionY)
{
    if (!_isAlive) return;

    _sprite->setPositionY(positionY);
    _sprite2->setPositionY(positionY);
}

Rect Enemy::getBoundingBox()
{
    return _sprite->getBoundingBox();
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

    if (!isAlive)
    {
        _sprite->setVisible(false);
        _sprite2->setVisible(false);
    }
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
