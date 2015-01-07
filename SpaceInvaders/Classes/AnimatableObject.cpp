#include "AnimatableObject.h"

USING_NS_CC;

AnimatableObject* AnimatableObject::create(std::vector<Sprite*> frames)
{
    AnimatableObject *object = new AnimatableObject();
    if (object && object->initWithFrames(frames))
    {
        object->autorelease();
        return object;
    }
    CC_SAFE_DELETE(object);
    return nullptr;
}

bool AnimatableObject::initWithFrames(std::vector<Sprite*> frames)
{
    bool result;
    if (Node::init())
    {
        _frames = frames;
        _frameIterator = _frames.begin();
        _isRepeat = false;
        _isAlive = true;
        
        for (auto it = _frames.begin(); it != _frames.end(); ++it)
        {
            auto sprite = *it;
            sprite->setVisible(false);
            sprite->setPosition(Point::ZERO);
            this->addChild(sprite);
        }

        auto firstFrame = _frames.empty() ? nullptr : _frames.front();
        if (firstFrame)
        {
            firstFrame->setVisible(true);
        }

        this->setVisible(true);

        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

Rect AnimatableObject::getBoundingBox()
{
    if (_frames.empty())
    {
        return Rect::ZERO;
    }

    auto box = _frames.front()->getBoundingBox();
    box.origin = this->convertToWorldSpace(box.origin);
    return box;
}

Size AnimatableObject::getSize()
{
    if (_frames.empty())
    {
        return Size::ZERO;
    }

    return _frames.front()->getBoundingBox().size;
}

void AnimatableObject::setRepeat()
{
    _isRepeat = true;
}

bool AnimatableObject::isAlive()
{
    return _isAlive;
}

void AnimatableObject::setAlive(bool isAlive)
{
    _isAlive = isAlive;
    Node::setVisible(isAlive);  // missile also gets invisible. Find a way to fix this
}

void AnimatableObject::animateToNextFrame()
{
    if (_frames.empty()) return;

    if (_frameIterator != _frames.end())
    {
        auto curFrame = *_frameIterator;
        Sprite *nextFrame = nullptr;

        _frameIterator++;
        if (_frameIterator != _frames.end())
        {
            nextFrame = *_frameIterator;
        }
        else
        {
            if (_isRepeat)
            {
                _frameIterator = _frames.begin();
                nextFrame = *_frameIterator;
            }
        }

        curFrame->setVisible(false);
        if (nextFrame)
            nextFrame->setVisible(true);
    }
}