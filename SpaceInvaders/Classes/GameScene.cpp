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
    
    gameState = NOT_INITIALIZED;
    isEnemyMoveDownPending = false;
    enemyMoveElapsedTime = 0;    
    enemyDeltaX = visibleSize.width * 0.7 / 40;   // 40 movments from side to side  // doesn't seem like it's actually 40 times? investigate later
    enemyDeltaY = 50;   //temp
    aliveEnemyCount = ENEMY_ROW_COUNT * ENEMY_COL_COUNT;

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
    player->setPosition(Point(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height * 0.2f));
    this->addChild(player, 1);  // refactor z order

    missile = Sprite::create("missile.png");
    missile->setVisible(false);
    this->addChild(missile, 2);

    for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
    {
        for (int j = 0; j < ENEMY_COL_COUNT; ++j)
        {
            std::vector<Sprite*> frames;
            frames.push_back(Sprite::create("enemy_large.png"));
            frames.push_back(Sprite::create("enemy_large2.png"));

            auto enemy = Enemy::create(frames);
            enemy->setRepeat();

            if (i == ENEMY_ROW_COUNT - 1)
            {
                enemy->setAtFrontLine(true);
            }
            
            // Fix so that the the right most and left most enemy do not go beyond the screen edge
            auto enemySize = enemy->getSize();
            auto widthBetweenEnemies = (visibleSize.width * 0.7f - (enemySize.width * ENEMY_COL_COUNT)) / (ENEMY_COL_COUNT - 1);    // temporarily for height too
            enemy->setPosition(Point(visibleOrigin.x + visibleSize.width * 0.15f + enemySize.width/2 + enemySize.width * j + widthBetweenEnemies * j,  // magic number
                               visibleOrigin.y + visibleSize.height * 0.9f - enemySize.height/2 - enemySize.height * i - widthBetweenEnemies * i));

            this->addChild(enemy, 1);
            enemies[i][j] = enemy;
        }
    }

    auto temporarySpriteForMeasuringSize = Sprite::create("blockage_0_hit.png");

    int numberOfBlockagesInARow = 4;
    auto wholeBlockWidth = temporarySpriteForMeasuringSize->getBoundingBox().size.width * numberOfBlockagesInARow;
    auto widthBetweenWholeBlocks = (visibleSize.width - wholeBlockWidth * NUMBER_OF_WHOLE_BLOCKS) / (NUMBER_OF_WHOLE_BLOCKS + 1);

    auto yPosOfBlock = visibleOrigin.y + visibleSize.height * 0.3;    // temporary

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

    gameState = INITIALIZED;    // better way to change gamestate to playing
    this->scheduleUpdate();
    
    return true;
}

void GameScene::update(float dt)
{
    if (gameState == OVER) return;

    updateEnemy(dt);

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
    if (enemyMoveElapsedTime >= ENEMY_MOVE_INTERVAL)
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

            if (rightMostEnemy->getPositionX() + rightMostEnemy->getSize().width/2 >= visibleOrigin.x + visibleSize.width ||
                leftMostEnemy->getPositionX() - leftMostEnemy->getSize().width/2 <= visibleOrigin.x)
            {
                enemyDeltaX *= -1;
                isEnemyMoveDownPending = true;
            }
        }

        enemyMoveElapsedTime -= ENEMY_MOVE_INTERVAL;
    }
}

void GameScene::checkCollision()
{
    for (int i = 0; i < ENEMY_ROW_COUNT; ++i)
    {
        for (int j = 0; j < ENEMY_COL_COUNT; ++j)
        {
            auto enemy = enemies[i][j];
            if (!enemy->isVisible()) continue;

            if (missile->isVisible() && missile->getBoundingBox().intersectsRect(enemy->getBoundingBox()))
            {
                missile->setVisible(false);
                enemy->setAlive(false);

                aliveEnemyCount--;
                if (aliveEnemyCount <= 0)
                {
                    gameState = OVER;
                }


                for (int k = ENEMY_ROW_COUNT - 1; k >= 0; --k)
                {
                    auto frontLineEnemy = enemies[k][j];
                    if (frontLineEnemy->isAlive())
                    {
                        frontLineEnemy->setAtFrontLine(true);
                        break;
                    }
                }
            }

            auto enemyMissile = enemy->getMissile();
            if (enemyMissile->isVisible() && enemyMissile->getBoundingBox().intersectsRect(player->getBoundingBox()))
            {
                enemy->missileHit();
            }

            for (int k = 0; k < NUMBER_OF_BLOCKAGES; ++k)
            {
                auto blockage = blockages[k];
                auto wholeNode = blockage->getParent();

                auto blockageBox = blockage->getBoundingBox();
                blockageBox.origin = wholeNode->getPosition();  // what is the best way to get the bounding box of a childe of a node? Do I have to add the parent node's origin?

                if (enemyMissile->isVisible() && enemyMissile->getBoundingBox().intersectsRect(blockageBox))
                {
                    enemy->missileHit();
                    blockages[k]->animateToNextFrame(); // possibly make onHit() in the base class
                }
            }

            if (enemyMissile->getPositionY() + enemy->getPositionY() + enemyMissile->getBoundingBox().size.height/2 < visibleOrigin.y)
            {
                enemy->missileOutOfBound();
            }
        }
    }
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
