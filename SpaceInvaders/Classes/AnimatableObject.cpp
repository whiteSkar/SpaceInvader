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
    box.origin = Point(Node::getPositionX() - box.size.width/2, Node::getPositionY() - box.size.height/2);
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

void AnimatableObject::animateToNextFrame()
{
    if (_frames.empty()) return;

    auto curFrame = *_frameIterator;
    Sprite *nextFrame = nullptr;

    if (_frameIterator != _frames.end())
    {
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
        nextFrame->setVisible(true);
    }
}