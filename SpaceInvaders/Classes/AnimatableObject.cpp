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
            auto frame = *it;
            frame->setVisible(false);
            frame->setPosition(Point::ZERO);
            this->addChild(frame);
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

void AnimatableObject::reinitialize()
{
    _frameIterator = _frames.begin();
    _isRepeat = false;
    _isAlive = true;
        
    for (auto it = _frames.begin(); it != _frames.end(); ++it)
    {
        auto frame = *it;
        frame->setVisible(false);
        frame->setPosition(Point::ZERO);
    }

    auto firstFrame = _frames.empty() ? nullptr : _frames.front();
    if (firstFrame)
    {
        firstFrame->setVisible(true);
    }

    this->setVisible(true);
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

void AnimatableObject::setAlive(bool isAlive, bool isApplyToNode)   // hacky way to only set visible to the frames not the node itself
{
    _isAlive = isAlive;
    
    if (isApplyToNode)
    {
        this->setVisible(isAlive);
    }
    else
    {
        for (int i = 0; i < _frames.size(); ++i)
        {
            auto frame = _frames[i];
            frame->setVisible(isAlive);
        }
    }
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