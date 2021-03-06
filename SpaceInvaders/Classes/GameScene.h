#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Enemy.h"
#include "Blockage.h"

static const float PLAYER_SPEED = 300.0f;
static const float PLAYER_MISSILE_SPEED = 1000.0f;
static const int DEFAULT_PLAYER_LIVES = 3;

static const float PLAYER_Y_POS_PERCENTAGE_OF_SCREEN = 0.3f;
static const float BLOCKAGE_Y_POS_PERCENTAGE_OF_SCREEN = 0.42f;
static const float ENEMY_ARMY_Y_POS_PERCENTAGE_OF_SCREEN = 0.9f;
static const float BATTLE_FIELD_HEIGHT_PERCENTAGE = ENEMY_ARMY_Y_POS_PERCENTAGE_OF_SCREEN - PLAYER_Y_POS_PERCENTAGE_OF_SCREEN;

static const float UI_HEIGHT_PERCENTAGE = PLAYER_Y_POS_PERCENTAGE_OF_SCREEN;
static const float UI_FIRE_BUTTON_X_POS = 0.75f;
static const float UI_JOYSTICK_AREA_HEIGHT_MULTIPLIER = 5.0f;
static const float UI_JOYSTICK_AREA_WIDTH_MULTIPLIER = 1.5f;

static const int ENEMY_ROW_COUNT = 5;   // 5
static const int ENEMY_COL_COUNT = 11;  // 11
static const int NUMBER_OF_ENEMIES = ENEMY_ROW_COUNT * ENEMY_COL_COUNT;

static const int SCORE_VALUE_LARGE_ENEMY = 10;
static const int SCORE_VALUE_MED_ENEMY = 20;
static const int SCORE_VALUE_SMALL_ENEMY = 40;

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

static const float HUD_TOP_OFFSET_FROM_BOTTOM = 0.94f;
static const float HUD_LEFT_OFFSET = 0.03f;
static const float HUD_RIGHT_OFFSET = 1.0f - HUD_LEFT_OFFSET;
static const float HUD_PLAYER_LIFE_IMAGE_GAP = 0.02f;
static const float HUD_LABEL_VALUE_GAP = 0.04f;

static const std::string SCORE_LABEL = "SCORE: ";

typedef std::pair<float, float> Position;

enum GameState
{
    NOT_INITIALIZED,
    INITIALIZED,
    PLAYING,
    WIN,
    DEAD,
    OVER
};

class GameScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

    //virtual void draw (cocos2d::Renderer* renderer, const kmMat4& transform, bool transformUpdated);    // debugging purpose

    void initializeForNextRound();

    void fireMissile(cocos2d::Ref* pSender);

    void onTouchesBegan(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event);
	void onTouchesCancelled(const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event);

    bool GameScene::isPositionWithinJoystickArea(cocos2d::Point position);

    void update(float dt);
    void updateEnemy(float dt);

    void checkCollision();

    void setEnemyDead(Enemy *enemy, int colIndexOfEnemy);
    void setPlayerDead();
    void revivePlayerAfterDelay();
    void setPlayerAlive();

    void setGameState(GameState state);
    void addScore(int score);

    void setPauseForEnemies(bool isPause);

    GameState gameState;

    int currentScore;
    int playerLife;
    
	cocos2d::Point touchPointInJoystickArea;

    cocos2d::Size visibleSize;
    cocos2d::Point visibleOrigin;

    cocos2d::Sprite *player;
    cocos2d::Sprite *playerLifeImage3;
    cocos2d::Sprite *playerLifeImage2;
    cocos2d::Sprite *playerLifeImage1;
    cocos2d::Sprite *missile;

    cocos2d::Sprite *joystickStick;
    cocos2d::Sprite *joystickSlider;

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

    cocos2d::LabelTTF *scoreLabel;
};

#endif // __GAME_SCENE_H__
