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
    enemyDeltaX = visibleSize.width * (1.0f - ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN) / ENEMY_NUMBER_OF_MOVEMENTS_FROM_SIDE_TO_SIDE_FOR_ONE_MINUS_ENEMY_ARMY_WIDTH_PERCENTAGE_OF_SCREEN;
    enemyGap = enemyDeltaX * ENEMY_GAP_PROPORTIONAL_TO_DELTA_X;
    enemyExpectedWidth = enemyGap * ENEMY_WIDTH_PROPORTIONAL_TO_GAP;
    aliveEnemyCount = ENEMY_ROW_COUNT * ENEMY_COL_COUNT;
    enemyMoveInterval = ENEMY_MOVE_INTERVAL_DEFAULT;

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(GameScene::menuCloseCallback, this));
    
	closeItem->setPosition(Point(visibleOrigin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                visibleOrigin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);

    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Point(visibleOrigin.x + visibleSize.width/2,
                            visibleOrigin.y + visibleSize.height - label->getContentSize().height));
    this->addChild(label, 1);

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
            auto sprite1 = Sprite::create("enemy_large.png");
            auto sprite2 = Sprite::create("enemy_large2.png");

            sprite1->setScale(enemyExpectedWidth / sprite1->getBoundingBox().size.width);
            sprite2->setScale(enemyExpectedWidth / sprite2->getBoundingBox().size.width);

            frames.push_back(sprite1);
            frames.push_back(sprite2);

            auto enemy = Enemy::create(frames);
            enemy->setRepeat();

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

    isTouchDown = false;
    currentTouchPos = Point::ZERO;

    auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);

	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	touchListener->onTouchCancelled = CC_CALLBACK_2(GameScene::onTouchCancelled, this);
	
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

    if (isTouchDown)
    {
        // touch HUD for movement and missile shoot => movement on the bottom right and shooting on the bottom left
        auto direction = 1;
        if (currentTouchPos.x <= visibleSize.width / 2)
        {
            direction = -1;
        }

        player->setPositionX(player->getPositionX() + PLAYER_SPEED * dt * direction);

        if (!missile->isVisible())
        {
            missile->setPosition(player->getPositionX(), player->getPositionY() + player->getBoundingBox().size.height/2 + missile->getBoundingBox().size.height/2);
            missile->setVisible(true);
        }
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
}

// Needs a better way to check collision. This is not a good way
void GameScene::checkCollision()
{
    for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
    {
        for (int j = 0; j < ENEMY_COL_COUNT; ++j)
        {
            auto enemy = enemies[i][j];
            if (!enemy->isAlive()) continue;

            if (missile->isVisible() && missile->getBoundingBox().intersectsRect(enemy->getBoundingBox()))
            {
                missile->setVisible(false);
                setEnemyDead(enemy, j);
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
    setGameState(OVER);
}

bool GameScene::onTouchBegan(Touch* touch, Event* event)
{
	isTouchDown = true;
    currentTouchPos = touch->getLocation();

	return true;
}

void GameScene::onTouchMoved(Touch* touch, Event* event)
{
	currentTouchPos = touch->getLocation();
}

void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	isTouchDown = false;
}

void GameScene::onTouchCancelled(Touch* touch, Event* event)
{
	onTouchEnded(touch, event);
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

void GameScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
