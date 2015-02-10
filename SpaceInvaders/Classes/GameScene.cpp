#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    visibleSize = Director::getInstance()->getVisibleSize();
    visibleOrigin = Director::getInstance()->getVisibleOrigin();

    srand(time(NULL));
    
    setGameState(NOT_INITIALIZED);
    isEnemyMoveDownPending = false;
    isEnemyBelowPlayer = false;
    enemyMoveElapsedTime = 0;   
    currentScore = 0;
    playerLife = DEFAULT_PLAYER_LIVES;
    enemyDeltaX = visibleSize.width * (1.0f - ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN) / ENEMY_NUMBER_OF_MOVEMENTS_FROM_SIDE_TO_SIDE_FOR_ONE_MINUS_ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN;
    enemyGap = enemyDeltaX * ENEMY_GAP_PROPORTIONAL_TO_DELTA_X;
    enemyExpectedWidth = enemyGap * ENEMY_WIDTH_PROPORTIONAL_TO_GAP;
    aliveEnemyCount = ENEMY_ROW_COUNT * ENEMY_COL_COUNT;
    enemyMoveInterval = ENEMY_MOVE_INTERVAL_DEFAULT;

    auto fireButton = MenuItemImage::create(
                                           "button_fire.png",   // PLACEHOLDER IMAGE
                                           "button_fire.png",
                                           CC_CALLBACK_1(GameScene::fireMissile, this));
    
	fireButton->setPosition(Point(visibleOrigin.x + visibleSize.width * UI_FIRE_BUTTON_X_POS,
                                  visibleOrigin.y + visibleSize.height * UI_HEIGHT_PERCENTAGE / 2.0f));

    auto ui = Menu::create(fireButton, NULL);
    ui->setPosition(Point::ZERO);
    this->addChild(ui, 99);

    joystickSlider = Sprite::create("joystick_slider.png");
    joystickSlider->setPosition(visibleOrigin.x + visibleSize.width * 0.25f, 
                                visibleOrigin.y + visibleSize.height * UI_HEIGHT_PERCENTAGE / 2.0f);
    this->addChild(joystickSlider);

    joystickStick = Sprite::create("joystick_stick.png");
    joystickStick->setPosition(visibleOrigin.x + visibleSize.width * 0.25f, 
                                visibleOrigin.y + visibleSize.height * UI_HEIGHT_PERCENTAGE / 2.0f);
    this->addChild(joystickStick);

    /* HUD */
    auto hudYPosForAnchorZero = visibleOrigin.y + visibleSize.height * HUD_TOP_OFFSET_FROM_BOTTOM;

    // Fix Font and size. Font size need to be also dynamic
    scoreLabel = LabelTTF::create(SCORE_LABEL + std::to_string(currentScore), "Arial", 48); // to_string is not supported in ndk. copy to_string method from my previous game
    scoreLabel->setPosition(visibleOrigin.x + visibleSize.width * HUD_LEFT_OFFSET,
                            hudYPosForAnchorZero);
    scoreLabel->setAnchorPoint(Point::ZERO);
    this->addChild(scoreLabel, 99);

    // Need auto resize image
    playerLifeImage3 = Sprite::create("player.png");
    playerLifeImage3->setAnchorPoint(Point::ZERO);
    playerLifeImage3->setPosition(visibleOrigin.x + visibleSize.width * HUD_RIGHT_OFFSET - playerLifeImage3->getBoundingBox().size.width,
                                  hudYPosForAnchorZero);
    this->addChild(playerLifeImage3);

    playerLifeImage2 = Sprite::create("player.png");
    playerLifeImage2->setAnchorPoint(Point::ZERO);
    playerLifeImage2->setPosition(playerLifeImage3->getPositionX() - visibleSize.width * HUD_PLAYER_LIFE_IMAGE_GAP - playerLifeImage2->getBoundingBox().size.width,
                                  hudYPosForAnchorZero);
    this->addChild(playerLifeImage2);

    playerLifeImage1 = Sprite::create("player.png");
    playerLifeImage1->setAnchorPoint(Point::ZERO);
    playerLifeImage1->setPosition(playerLifeImage2->getPositionX() - visibleSize.width * HUD_PLAYER_LIFE_IMAGE_GAP - playerLifeImage1->getBoundingBox().size.width,
                                  hudYPosForAnchorZero);
    this->addChild(playerLifeImage1);

    // Fix font and size
    auto playerLivesLabel = LabelTTF::create("LIVES", "Arial", 48);
    playerLivesLabel->setPosition(playerLifeImage1->getPositionX() - visibleSize.width * HUD_LABEL_VALUE_GAP - playerLivesLabel->getContentSize().width,
                                  hudYPosForAnchorZero);
    playerLivesLabel->setAnchorPoint(Point::ZERO);
    this->addChild(playerLivesLabel, 99);

    /* Objects */
    player = Sprite::create("player.png");
    player->setPosition(Point(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height * PLAYER_Y_POS_PERCENTAGE_OF_SCREEN));
    this->addChild(player, 1);  // refactor z order

    missile = Sprite::create("missile.png");
    missile->setVisible(false);
    this->addChild(missile, 2);

    enemyDeltaY = 0;
    for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
    {
        for (int j = 0; j < ENEMY_COL_COUNT; ++j)
        {
            std::vector<Sprite*> frames;

            std::string enemyType = "small";
            if (i >= 1 && i <= 2)
                enemyType = "med";
            if (i >= 3)
                enemyType = "large";

            // smaller enemies sprites need to be horizontally shorter. Otherwise, collision doesn't work properly
            auto sprite1 = Sprite::create("enemy_" + enemyType + ".png");
            auto sprite2 = Sprite::create("enemy_" + enemyType + "2.png");

            sprite1->setScale(enemyExpectedWidth / sprite1->getBoundingBox().size.width);
            sprite2->setScale(enemyExpectedWidth / sprite2->getBoundingBox().size.width);

            frames.push_back(sprite1);
            frames.push_back(sprite2);

            auto enemy = Enemy::create(frames);
            enemy->setRepeat();

            if (enemyType == "small")   // use constants
                enemy->setScoreValue(SCORE_VALUE_SMALL_ENEMY);
            else if (enemyType == "med")
                enemy->setScoreValue(SCORE_VALUE_MED_ENEMY);
            else
                enemy->setScoreValue(SCORE_VALUE_LARGE_ENEMY);


            if (i == ENEMY_ROW_COUNT - 1)
            {
                enemy->setAtFrontLine(true);
            }
            
            auto enemySize = enemy->getSize();
            enemy->setPosition(Point(visibleOrigin.x + visibleSize.width * ((1.0f - ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN) / 2) + enemySize.width/2 + enemySize.width * j + enemyGap * j,
                               visibleOrigin.y + visibleSize.height * ENEMY_ARMY_Y_POS_PERCENTAGE_OF_SCREEN - enemySize.height/2 - enemySize.height * i - enemyGap * i));

            this->addChild(enemy, 1);
            enemies[i][j] = enemy;

            if (enemyDeltaY == 0)
            {
                enemyDeltaY = (visibleSize.height * BATTLE_FIELD_HEIGHT_PERCENTAGE - (enemySize.height * ENEMY_ROW_COUNT + enemyGap * (ENEMY_ROW_COUNT - 1))) / ENEMY_NUMBER_OF_MOVEMENTS_FROM_TOP_TO_BOTTOM;
            }
        }
    }

    auto temporarySpriteForMeasuringSize = Sprite::create("blockage_0_hit.png");

    int numberOfBlockagesInARow = 4;
    auto wholeBlockWidth = temporarySpriteForMeasuringSize->getBoundingBox().size.width * numberOfBlockagesInARow;
    auto widthBetweenWholeBlocks = (visibleSize.width - wholeBlockWidth * NUMBER_OF_WHOLE_BLOCKS) / (NUMBER_OF_WHOLE_BLOCKS + 1);

    auto yPosOfBlock = visibleOrigin.y + visibleSize.height * BLOCKAGE_Y_POS_PERCENTAGE_OF_SCREEN;

    Position blockagePositions[NUMBER_OF_BLOCKAGES_IN_A_WHOLE_BLOCK];
    blockagePositions[0] = Position(-1.5f, -1.5f);
    blockagePositions[1] = Position(-1.5f, -0.5f);
    blockagePositions[2] = Position(-1.5f, 0.5f);
    blockagePositions[3] = Position(-0.5f, -0.5f);
    blockagePositions[4] = Position(-0.5f, 0.5f);
    blockagePositions[5] = Position(0.5f, -0.5f);
    blockagePositions[6] = Position(0.5f, 0.5f);
    blockagePositions[7] = Position(1.5f, -1.5f);
    blockagePositions[8] = Position(1.5f, -0.5f);
    blockagePositions[9] = Position(1.5f, 0.5f);

    for (int i = 0 ; i < NUMBER_OF_WHOLE_BLOCKS; ++i)
    {
        auto wholeBlockNode = Node::create();
        wholeBlockNode->setVisible(true);
        wholeBlockNode->setPosition(Point(visibleOrigin.x + widthBetweenWholeBlocks * (1 + 2 * i), yPosOfBlock));

        for (int j = 0; j < NUMBER_OF_BLOCKAGES_IN_A_WHOLE_BLOCK; ++j)
        {
            std::vector<Sprite*> frames;
            frames.push_back(Sprite::create("blockage_0_hit.png"));
            frames.push_back(Sprite::create("blockage_1_hit.png"));
            frames.push_back(Sprite::create("blockage_2_hit.png"));
            frames.push_back(Sprite::create("blockage_3_hit.png"));

            auto position = blockagePositions[j];

            auto blockage = Blockage::create(frames);
            wholeBlockNode->addChild(blockage);
            blockage->setPosition(blockage->getBoundingBox().size.width * position.first, blockage->getBoundingBox().size.height * position.second);
            blockages[i * NUMBER_OF_BLOCKAGES_IN_A_WHOLE_BLOCK + j] = blockage;
        }

        this->addChild(wholeBlockNode, 999);
    }

    touchPointInJoystickArea = Point::ZERO;

    auto touchListener = EventListenerTouchAllAtOnce::create();
	//touchListener->setSwallowTouches(true);

	touchListener->onTouchesBegan = CC_CALLBACK_2(GameScene::onTouchesBegan, this);
	touchListener->onTouchesMoved = CC_CALLBACK_2(GameScene::onTouchesMoved, this);
	touchListener->onTouchesEnded = CC_CALLBACK_2(GameScene::onTouchesEnded, this);
	touchListener->onTouchesCancelled = CC_CALLBACK_2(GameScene::onTouchesCancelled, this);
	
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

    setGameState(INITIALIZED);   // better way to change gamestate to playing
    this->scheduleUpdate();
    
    return true;
}

void GameScene::update(float dt)
{
    if (gameState == OVER) return;

    updateEnemy(dt);

    if (isEnemyBelowPlayer)
    {
        setGameState(OVER);
    }

    if (missile->isVisible())
    {
        if (missile->getPositionY() - missile->getBoundingBox().size.height/2 > visibleOrigin.y + visibleSize.height)
        {
            missile->setVisible(false);
        }
        else
        {
            missile->setPositionY(missile->getPositionY() + PLAYER_MISSILE_SPEED * dt);
        }
    }

    if (touchPointInJoystickArea != Point::ZERO)
    {
        auto newStickXPos = touchPointInJoystickArea.x;
        auto sliderLeftX = joystickSlider->getPositionX() - joystickSlider->getBoundingBox().size.width / 2;
        auto sliderRightX = joystickSlider->getPositionX() + joystickSlider->getBoundingBox().size.width / 2;
        auto stickWidthHalf = joystickStick->getBoundingBox().size.width / 2;

        if (newStickXPos - stickWidthHalf < sliderLeftX)
        {
            newStickXPos = sliderLeftX + stickWidthHalf;
        }
        else if (newStickXPos + stickWidthHalf > sliderRightX)
        {
            newStickXPos = sliderRightX - stickWidthHalf;
        }

        joystickStick->setPositionX(newStickXPos);

        //auto direction = 1;
        //if (currentTouchPos.x <= visibleSize.width / 2)
        //{
        //    direction = -1;
        //}

        //player->setPositionX(player->getPositionX() + PLAYER_SPEED * dt * direction);
    }
    else
    {
        joystickStick->setPositionX(joystickSlider->getPositionX());
    }

    this->checkCollision();
}

void GameScene::updateEnemy(float dt)
{
    enemyMoveElapsedTime += dt;
    if (enemyMoveElapsedTime >= enemyMoveInterval)
    {
        Enemy *rightMostEnemy = nullptr;
        Enemy *leftMostEnemy = nullptr;

        if (isEnemyMoveDownPending)
        {
            for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
            {
                for (int j = 0; j < ENEMY_COL_COUNT; ++j)
                {
                    auto enemy = enemies[i][j];
                    if (!enemy->isAlive()) continue;

                    enemy->setPositionY(enemy->getPositionY() - enemyDeltaY);
                    enemy->animateToNextFrame();

                    if (enemy->getPositionY() < player->getPositionY())
                    {
                        isEnemyBelowPlayer = true;
                    }
                }
            }

            isEnemyMoveDownPending = false;
        }
        else
        {
            for (int j = 0; j < ENEMY_COL_COUNT; ++j)
            {
                for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
                {
                    auto enemy = enemies[i][j];
                    if (!enemy->isAlive()) continue;

                    enemy->setPositionX(enemy->getPositionX() + enemyDeltaX);
                    enemy->animateToNextFrame();

                    rightMostEnemy = enemy;
                    if (!leftMostEnemy)
                        leftMostEnemy = enemy;
                }
            }

            if (std::ceil(rightMostEnemy->getPositionX() + rightMostEnemy->getSize().width/2) >= visibleOrigin.x + visibleSize.width ||
                std::floor(leftMostEnemy->getPositionX() - leftMostEnemy->getSize().width/2) <= visibleOrigin.x)
            {
                enemyDeltaX *= -1;
                isEnemyMoveDownPending = true;
            }
        }

        enemyMoveElapsedTime -= enemyMoveInterval;
    }

    for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
    {
        for (int j = 0; j < ENEMY_COL_COUNT; ++j)
        {
            auto enemy = enemies[i][j];
            enemy->update(dt);  // has to call this after moving the enemies for missile to fix position
        }
    }
}

// Needs a better way to check collision. This is not a good way
void GameScene::checkCollision()
{
    for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
    {
        for (int j = 0; j < ENEMY_COL_COUNT; ++j)
        {
            auto enemy = enemies[i][j];

            if (enemy->isAlive() && missile->isVisible() && missile->getBoundingBox().intersectsRect(enemy->getBoundingBox()))
            {
                missile->setVisible(false);
                setEnemyDead(enemy, j);

                addScore(enemy->getScoreValue());   // score only goes up when it is hit by your missile
            }

            auto enemyMissileBox = enemy->getMissileBoundingBox();
            if (enemy->isMissileVisible() && enemyMissileBox.intersectsRect(player->getBoundingBox()))
            {
                enemy->missileHit();
                setPlayerDead();
            }

            if (enemy->isAlive() && player->isVisible() && enemy->getBoundingBox().intersectsRect(player->getBoundingBox()))
            {
                setEnemyDead(enemy, j);
                setPlayerDead();
            }

            for (int k = 0; k < NUMBER_OF_BLOCKAGES; ++k)
            {
                auto blockage = blockages[k];
                if (!blockage->isAlive()) continue;

                auto blockageBox = blockage->getBoundingBox();
                if (enemy->isMissileVisible() && enemyMissileBox.intersectsRect(blockageBox))
                {
                    enemy->missileHit();
                    blockages[k]->onHit();
                }

                if (enemy->isAlive() && enemy->getBoundingBox().intersectsRect(blockageBox))
                {
                    setEnemyDead(enemy, j);
                    blockages[k]->onHit();
                }
            }

            if (enemyMissileBox.origin.y + enemyMissileBox.size.height/2 < visibleOrigin.y)
            {
                enemy->missileOutOfBound();
            }
        }
    }

    for (int k = 0; k < NUMBER_OF_BLOCKAGES; ++k)
    {
        auto blockage = blockages[k];
        if (!blockage->isAlive()) continue;

        auto blockageBox = blockage->getBoundingBox();

        if (missile->isVisible() && missile->getBoundingBox().intersectsRect(blockageBox))
        {
            missile->setVisible(false);
            blockages[k]->onHit();
        }
    }
}

void GameScene::setGameState(GameState state)
{
    gameState = state;

    if (gameState == OVER)
    {
        for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
        {
            for (int j = 0; j < ENEMY_COL_COUNT; ++j)
            {
                auto enemy = enemies[i][j];
                enemy->unscheduleUpdate();
            }
        }
    }
}

void GameScene::addScore(int deltaScore)
{
    currentScore += deltaScore;
    scoreLabel->setString(SCORE_LABEL + std::to_string(currentScore));
}


void GameScene::setEnemyDead(Enemy *enemy, int colIndexOfEnemy)
{
    enemy->setAlive(false);
    enemyMoveInterval -= ENEMY_MOVE_INTERVAL_DELTA_PER_ENEMY_DEAD;

    aliveEnemyCount--;
    if (aliveEnemyCount <= 0)
    {
        setGameState(OVER);
    }

    for (int k = ENEMY_ROW_COUNT - 1; k >= 0; --k)
    {
        auto frontLineEnemy = enemies[k][colIndexOfEnemy];
        if (frontLineEnemy->isAlive())
        {
            frontLineEnemy->setAtFrontLine(true);
            break;
        }
    }
}

void GameScene::setPlayerDead()
{
    player->setVisible(false);

    playerLife--;
    if (playerLife < 0)
    {
        playerLife = 0;
    }

    playerLifeImage3->setVisible(playerLife >= 3);
    playerLifeImage2->setVisible(playerLife >= 2);
    playerLifeImage1->setVisible(playerLife >= 1);

    setGameState(OVER);
}

void GameScene::onTouchesBegan(const std::vector<cocos2d::Touch*> &touches, Event* event)
{
    onTouchesMoved(touches, event);
}

void GameScene::onTouchesMoved(const std::vector<cocos2d::Touch*> &touches, Event* event)
{
    touchPointInJoystickArea = Point::ZERO;

    for (auto it = touches.begin(); it != touches.end(); ++it)
    {
        auto touch = *it;
        if (isPositionWithinJoystickArea(touch->getLocation()))
        {
            touchPointInJoystickArea = touch->getLocation();
        }
    }
}

void GameScene::onTouchesEnded(const std::vector<cocos2d::Touch*> &touches, Event* event)
{
    for (auto it = touches.begin(); it != touches.end(); ++it)
    {
        auto touch = *it;
        if (isPositionWithinJoystickArea(touch->getLocation()))
        {
            touchPointInJoystickArea = Point::ZERO;
        }
    }
}

void GameScene::onTouchesCancelled(const std::vector<cocos2d::Touch*> &touches, Event* event)
{
	onTouchesEnded(touches, event);
}

bool GameScene::isPositionWithinJoystickArea(Point position)
{
    float areaHeightHalf = joystickSlider->getBoundingBox().size.height * UI_JOYSTICK_AREA_HEIGHT_MULTIPLIER / 2;
    float areaWidthHalf = joystickSlider->getBoundingBox().size.width * UI_JOYSTICK_AREA_WIDTH_MULTIPLIER / 2;
    float areaMinX = joystickSlider->getPositionX() - areaWidthHalf;
    float areaMaxX = joystickSlider->getPositionX() + areaWidthHalf;
    float areaMinY = joystickSlider->getPositionY() - areaHeightHalf;
    float areaMaxY = joystickSlider->getPositionY() + areaHeightHalf;
    
    if (position.x >= areaMinX && position.x <= areaMaxX && position.y >= areaMinY && position.y <= areaMaxY)
    {
        return true;
    }

    return false;
}

//void GameScene::draw(Renderer* renderer, const kmMat4& transform, bool transformUpdated)
//{
//    Node::draw(renderer, transform, transformUpdated);
//    ccDrawColor4B(255, 0, 0, 255);
//    ccDrawLine(Point(visibleOrigin.x + visibleSize.width * (1.0f - ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN) / 2, 
//                    visibleOrigin.y), Point(visibleOrigin.x + visibleSize.width * (1.0f - ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN) / 2, visibleOrigin.y + visibleSize.height));
//    ccDrawLine(Point((visibleOrigin.x + visibleSize.width * (1.0f - ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN) / 2) + visibleSize.width * ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN, 
//                    visibleOrigin.y), Point((visibleOrigin.x + visibleSize.width * (1.0f - ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN) / 2) + visibleSize.width * ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN, visibleOrigin.y + visibleSize.height));
//
//    ccDrawLine(Point(visibleOrigin.x + 5, visibleOrigin.y), Point(visibleOrigin.x + 5, visibleOrigin.y + visibleSize.height));
//    ccDrawLine(Point(visibleOrigin.x + visibleSize.width - 5, visibleOrigin.y), Point(visibleOrigin.x + visibleSize.width - 5, visibleOrigin.y + visibleSize.height));
//}

void GameScene::fireMissile(Ref* pSender)
{
    if (!missile->isVisible())
    {
        missile->setPosition(player->getPositionX(), player->getPositionY() + player->getBoundingBox().size.height/2 + missile->getBoundingBox().size.height/2);
        missile->setVisible(true);
    }
}
