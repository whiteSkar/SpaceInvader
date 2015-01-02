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

        result = true;
    }
    else
    {
        result = false;
    }

    return result;
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
