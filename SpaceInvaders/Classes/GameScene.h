#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Enemy.h"
#include "Blockage.h"

static const float PLAYER_SPEED = 300.0f;
static const float PLAYER_MISSILE_SPEED = 1000.0f;

static const float PLAYER_Y_POS_PERCENTAGE_OF_SCREEN = 0.3f;
static const float BLOCKAGE_Y_POS_PERCENTAGE_OF_SCREEN = 0.42f;
static const float ENEMY_ARMY_Y_POS_PERCENTAGE_OF_SCREEN = 0.9f;
static const float BATTLE_FIELD_HEIGHT_PERCENTAGE = ENEMY_ARMY_Y_POS_PERCENTAGE_OF_SCREEN - PLAYER_Y_POS_PERCENTAGE_OF_SCREEN;

static const int ENEMY_ROW_COUNT = 5;
static const int ENEMY_COL_COUNT = 11;
static const int NUMBER_OF_ENEMIES = ENEMY_ROW_COUNT * ENEMY_COL_COUNT;

static const float ENEMY_MOVE_INTERVAL_DEFAULT = 0.5f;
static const float ENEMY_MOVE_INTERVAL_MIN = 0.03f;
static const float ENEMY_MOVE_INTERVAL_DELTA_PER_ENEMY_DEAD = (ENEMY_MOVE_INTERVAL_DEFAULT - ENEMY_MOVE_INTERVAL_MIN) / NUMBER_OF_ENEMIES;

static const float ENEMY_GAP_PROPORTION = 1.0f;
static const float ENEMY_WIDTH_PROPORTIONAL_TO_GAP = 4.0f;
static const float ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN = 2.0f / 3.0f;
static const int ENEMY_NUMBER_OF_MOVEMENTS_FROM_SIDE_TO_SIDE_FOR_ONE_MINUS_ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN = 40;
static const int ENEMY_NUMBER_OF_MOVEMENTS_FOR_ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN = ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN / (1.0f - ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN) *
                                                                                       ENEMY_NUMBER_OF_MOVEMENTS_FROM_SIDE_TO_SIDE_FOR_ONE_MINUS_ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN;
static const float ENEMY_GAP_PROPORTIONAL_TO_DELTA_X = ENEMY_NUMBER_OF_MOVEMENTS_FOR_ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN / 
                                                       (ENEMY_COL_COUNT * (ENEMY_WIDTH_PROPORTIONAL_TO_GAP + ENEMY_GAP_PROPORTION) - ENEMY_GAP_PROPORTION);
static const int ENEMY_NUMBER_OF_MOVEMENTS_FROM_TOP_TO_BOTTOM = 11;

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

    //virtual void draw (cocos2d::Renderer* renderer, const kmMat4& transform, bool transformUpdated);    // debugging purpose

    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
	void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event);

    void update(float dt);
    void updateEnemy(float dt);

    void checkCollision();

    void setEnemyDead(Enemy *enemy, int colIndexOfEnemy);
    void setPlayerDead();

    void setGameState(GameState state);

    GameState gameState;
    
    bool isTouchDown;
	cocos2d::Point currentTouchPos;

    cocos2d::Size visibleSize;
    cocos2d::Point visibleOrigin;

    cocos2d::Sprite *player;
    cocos2d::Sprite *missile;

    Enemy *enemies[ENEMY_ROW_COUNT][ENEMY_COL_COUNT];
    Blockage *blockages[NUMBER_OF_BLOCKAGES];

    bool isEnemyBelowPlayer;
    bool isEnemyMoveDownPending;
    float enemyDeltaX;
    float enemyGap;
    float enemyExpectedWidth;
    float enemyDeltaY;
    float enemyMoveElapsedTime;

    int aliveEnemyCount;

    float enemyMoveInterval;
};

#endif // __GAME_SCENE_H__
