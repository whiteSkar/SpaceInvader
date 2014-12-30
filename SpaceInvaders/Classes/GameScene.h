#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

static const float PLAYER_SPEED = 300.0f;
static const float MISSILE_SPEED = 600.0f;

static const float ENEMY_MOVE_INTERVAL = 0.5f;

class GameScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

    void update(float dt);
    void updateEnemy(float dt);

    void checkCollision();
    
    bool isTouchDown;
	cocos2d::Point currentTouchPos;

    cocos2d::Size visibleSize;
    cocos2d::Point visibleOrigin;

    cocos2d::Sprite *player;
    cocos2d::Sprite *missile;
    cocos2d::Sprite *enemy;

    float enemyDeltaX;
    float enemyMoveElapsedTime;
};

#endif // __GAME_SCENE_H__
