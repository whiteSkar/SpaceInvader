#include "Blockage.h"

USING_NS_CC;

Blockage* Blockage::create(std::vector<Sprite*> frames)
{
    Blockage *object = new Blockage();
    if (object && object->initWithFrames(frames))
    {
        object->autorelease();
        return object;
    }
    CC_SAFE_DELETE(object);
    return nullptr;
}

bool Blockage::initWithFrames(std::vector<Sprite*> frames)
{
    bool result;
    if (AnimatableObject::initWithFrames(frames))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}