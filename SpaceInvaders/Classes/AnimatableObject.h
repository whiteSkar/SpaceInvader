#ifndef __ANIMATABLE_OBJECT_H__
#define __ANIMATABLE_OBJECT_H__

#include "cocos2d.h"

class AnimatableObject : public cocos2d::Node
{
public:
    static AnimatableObject* create(std::vector<cocos2d::Sprite*> frames);

    bool initWithFrames(std::vector<cocos2d::Sprite*> frames);

    void reinitialize();

    cocos2d::Rect getBoundingBox();
    cocos2d::Size getSize();

    void setRepeat();

    virtual bool isAlive();
    virtual void setAlive(bool isAlive, bool isApplyToNode);

    void animateToNextFrame();

protected:
    std::vector<cocos2d::Sprite*> _frames;
    std::vector<cocos2d::Sprite*>::iterator _frameIterator;

    bool _isRepeat;
    bool _isAlive;
};

#endif