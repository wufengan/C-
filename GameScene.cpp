#include "GameScene.h"
//#include "FishJoyData.h"
GameScene::GameScene()
{
}

bool GameScene::init()
{
	do
	{
		CC_BREAK_IF(!CCScene::init());
		preloadResources();
		//因为~GameScene()中需要CC_SAFE_RELEASE(_menuLayer)， 如果其它层创建失败，_menuLayer将不创建，
		//所以_menuLayer要先于其他层创建， 否则将报 "reference count greater than 0" 错误
		menuLayer = MenuLayer::create(); 
		CC_BREAK_IF(!menuLayer);
		CC_SAFE_RETAIN(menuLayer); 
		backgroundLayer = BackgroundLayer::create();
		CC_BREAK_IF(!backgroundLayer);
		this->addChild(backgroundLayer);
		fishLayer = FishLayer::create();
		CC_BREAK_IF(!fishLayer);
		this->addChild(fishLayer);
		cannonLayer = CannonLayer::create();
		CC_BREAK_IF(!cannonLayer);
		this->addChild(cannonLayer);
		touchLayer = TouchLayer::create();
		CC_BREAK_IF(!touchLayer);
		this->addChild(touchLayer);
		paneLayer = PanelLayer::create();
		CC_BREAK_IF(!paneLayer);
		this->addChild(paneLayer);
		paneLayer->getGoldCounter()->setNumber(FishJoyData::sharedFishingJoyData()->getGold());
		this->scheduleUpdate();
		return true;
	} while (0);
	return false;
}

void GameScene::preloadResources(void)
{
	PersonalAudioEngine::sharePersonalAudioEngine();
	CCSpriteFrameCache* spriteFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
	//修改以下plist文件， 删除key中的中文， 否则spriteFrameByName函数无法找到Frame，将返回NULL
	spriteFrameCache->addSpriteFramesWithFile("FishActor-Large-ipadhd.plist");		//修改metadata->realTextureFileName->FishActor-Large-ipadhdhd.png, textureFileName->FishActor-Large-ipadhd.png
	spriteFrameCache->addSpriteFramesWithFile("FishActor-Marlin-ipadhd.plist");		//修改metadata->realTextureFileName->FishActor-Marlin-ipadhdhd.png, textureFileName->FishActor-Marlin-ipadhd.png
	spriteFrameCache->addSpriteFramesWithFile("FishActor-Shark-ipadhd.plist");		//同上
	spriteFrameCache->addSpriteFramesWithFile("FishActor-Small-ipadhd.plist");		//同上
	spriteFrameCache->addSpriteFramesWithFile("FishActor-Mid-ipadhd.plist");			//同上
	spriteFrameCache->addSpriteFramesWithFile("cannon-ipadhd.plist");
	spriteFrameCache->addSpriteFramesWithFile("Item-chaojiwuqi-ipadhd.plist");

	CCTextureCache *textureCache = CCTextureCache::sharedTextureCache();
	textureCache->addImage("ui_button_63-ipadhd.png");
	textureCache->addImage("ui_button_65-ipadhd.png");

	char str[][50] = { "SmallFish", "Croaker", "AngelFish", "Amphiprion", "PufferS", 
		"Bream", "Porgy", "Chelonian", "Lantern", "Ray", "Shark", "GoldenTrout", "GShark", 
		"GMarlinsFish", "GrouperFish", "JadePerch", "MarlinsFish", "PufferB" };
	for (int i = 0; i < 18; i++)
	{
		CCArray* array = CCArray::createWithCapacity(10);
		for (int j = 0; j < 10; j++)
		{
			CCString* spriteFrameName = CCString::createWithFormat("%s_actor_%03d.png", str[i], j + 1);
			CCSpriteFrame* spriteFrame = spriteFrameCache->spriteFrameByName(spriteFrameName->getCString());
			CC_BREAK_IF(!spriteFrame);
			array->addObject(spriteFrame);
		}
		if (array->count() == 0)
		{
			continue;
		}
		CCAnimation* animation = CCAnimation::createWithSpriteFrames(array, 0.15f);
		CCString* animationName = CCString::createWithFormat("fish_animation_%02d", i + 1);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, animationName->getCString());
	}
	
}

GameScene::~GameScene()
{
	CC_SAFE_RELEASE(menuLayer);
}

void GameScene::cannonAimAt(CCPoint target)
{
	cannonLayer->aimAt(target);
}

void GameScene::cannonShootTo(CCPoint target)
{
	int cost = cannonLayer->getWeapon()->getCannonType() + 1;
	if (FishJoyData::sharedFishingJoyData()->getGold() >= cost)
	{
		cannonLayer->shootTo(target);
		alterGold(-cost);
	}

	//_cannonLayer->shootTo(target);
}

bool GameScene::checkOutCollisionBetweenFishesAndBullet(Bullet* bullet)
{
	CCPoint bulletPos = bullet->getCollosionPoint();
	CCArray* fishArray = fishLayer->getFishArray();
	CCObject* obj = NULL;
	CCARRAY_FOREACH(fishArray, obj)
	{
		Fish* fish =(Fish*)obj;
		if(fish->isRunning() && fish->getCollisionArea().containsPoint(bulletPos))
		{
			bullet->end();
			return true;
		}
	}
	return false;
}

void GameScene::checkOutCollision()
{
	CCArray* bullets = cannonLayer->getWeapon()->getBullets();
	CCObject* obj = NULL;
	CCARRAY_FOREACH(bullets, obj)
	{
		Bullet* bullet = (Bullet*)obj;
		if(bullet->isVisible())
		{
			if(checkOutCollisionBetweenFishesAndBullet(bullet))
			{
				checkOutCollisionBetweenFishesAndFishingNet(bullet);
			}
		}
	}	
}

void GameScene::update(float delta)
{
	checkOutCollision();
}

void GameScene::fishWillBeCaught(Fish* fish)
{
	float weaponPercents[k_Cannon_Count] = { 0.3, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1 };
	float fishPercents[	k_Fish_Type_Count] = { 1, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4 };
	int _cannonType = cannonLayer->getWeapon()->getCannonType();
	int _fishType = fish->getType();
	float percentage =(float)_cannonType * _fishType;
	if(CCRANDOM_0_1() < percentage)//1.1
	{
		fish->beCaught();
		int reward = STATIC_DATA_INT(CCString::createWithFormat(STATIC_DATA_STRING("reward_format"),_fishType)->getCString());
		alterGold(reward);
	}
	
}

void GameScene::checkOutCollisionBetweenFishesAndFishingNet(Bullet* bullet)
{
	Weapon* weapon = cannonLayer->getWeapon();
	CCRect rect = weapon->getCollisionArea(bullet);
	CCArray* fishArray = fishLayer->getFishArray();
	CCObject* obj = NULL;
	CCARRAY_FOREACH(fishArray, obj)
	{
		Fish* fish = (Fish*)obj;
		if(fish->isRunning() && rect.intersectsRect(fish->getCollisionArea()))
		{
			fishWillBeCaught(fish);
		}
	}
}
void GameScene::alterGold(int delta)
{
	FishJoyData* fishJoyData = FishJoyData::sharedFishingJoyData();
	fishJoyData->alterGold(delta);
	paneLayer->getGoldCounter()->setNumber(fishJoyData->getGold());
}
void GameScene::onEnter()
{
	CCScene::onEnter();
	PersonalAudioEngine::sharePersonalAudioEngine()->playBackgroundMusic(3);
}