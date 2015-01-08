#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Enemy.h"
#include "Blockage.h"

static const float PLAYER_SPEED = 300.0f;
static const float PLAYER_MISSILE_SPEED = 1000.0f;

static const int ENEMY_ROW_COUNT = 5;
static const int ENEMY_COL_COUNT = 9;   // should be 11.
static const int NUMBER_OF_ENEMIES = ENEMY_ROW_COUNT * ENEMY_COL_COUNT;

static const float ENEMY_MOVE_INTERVAL_DEFAULT = 0.5f;
static const float ENEMY_MOVE_INTERVAL_MIN = 0.03f;
static const float ENEMY_MOVE_INTERVAL_DELTA_PER_ENEMY_DEAD = (ENEMY_MOVE_INTERVAL_DEFAULT - ENEMY_MOVE_INTERVAL_MIN) / NUMBER_OF_ENEMIES;

static const int NUMBER_OF_WHOLE_BLOCKS = 4;
static const int NUMBER_OF_BLOCKAGES_IN_A_WHOLE_BLOCK = 10;
static const int NUMBER_OF_BLOCKAGES = NUMBER_OF_WHOLE_BLOCKS * NUMBER_OF_BLOCKAGES_IN_A_WHOLE_BLOCK;

typedef std::pair<float, float> Position;

enum GameState
{
    NOT_INITIALIZED,
    INITIALIZED,
    PLAYING,
    OVER
};

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

    GameState gameState;
    
    bool isTouchDown;
	cocos2d::Point currentTouchPos;

    cocos2d::Size visibleSize;
    cocos2d::Point visibleOrigin;

    cocos2d::Sprite *player;
    cocos2d::Sprite *missile;

    Enemy *enemies[ENEMY_ROW_COUNT][ENEMY_COL_COUNT];
    Blockage *blockages[NUMBER_OF_BLOCKAGES];

    bool isEnemyMoveDownPending;
    float enemyDeltaX;
    float enemyDeltaY;
    float enemyMoveElapsedTime;

    int aliveEnemyCount;

    float enemyMoveInterval;
};

#endif // __GAME_SCENE_H__
