#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "cocos2d.h"

class Enemy : public cocos2d::Node
{
public:
    static Enemy* create(cocos2d::Sprite *sprite, cocos2d::Sprite *sprite2); // only need two sprites. This way suffices

    bool initWithSprites(cocos2d::Sprite *sprite, cocos2d::Sprite *sprite2);

    cocos2d::Rect getBoundingBox();
    cocos2d::Size getSize();

    //virtual void draw (cocos2d::Renderer* renderer, const kmMat4& transform, bool transformUpdated);    // debugging purpose

    bool isAlive();
    void setAlive(bool isAlive);

    void animateToNextFrame();

private:
    cocos2d::Sprite *_sprite;
    cocos2d::Sprite *_sprite2;
    
    bool _isAlive;
};

#endif