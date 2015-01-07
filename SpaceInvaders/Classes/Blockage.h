#ifndef __BLOCKAGE_H__
#define __BLOCKAGE_H__

#include "AnimatableObject.h"

class Blockage : public AnimatableObject
{
public:
    static Blockage* create(std::vector<cocos2d::Sprite*> frames);
    
    bool initWithFrames(std::vector<cocos2d::Sprite*> frames);
};

#endif
