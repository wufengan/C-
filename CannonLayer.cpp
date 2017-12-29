#include "CannonLayer.h"

CannonLayer::CannonLayer(void)
{
}

CannonLayer::~CannonLayer(void)
{
}

bool CannonLayer::init()
{
	if(!CCLayer::init())
	{
		return false;
	}
	weapon = Weapon::create((CannonType)0);
	addChild(weapon,1);
	CCSize winSize=CCDirector::sharedDirector()->getWinSize();
	weapon->setPosition(ccp(winSize.width/2 - 18, 0));

	addMenuItem = CCMenuItemImage::create(
		"ui_button_65-ipadhd.png",
		"ui_button_65-ipadhd.png",
		this,menu_selector(CannonLayer::switchCannonCallback));

	subMenuItem = CCMenuItemImage::create(
		"ui_button_63-ipadhd.png",
		"ui_button_63-ipadhd.png",
		this,menu_selector(CannonLayer::switchCannonCallback));

	CCMenu* menu = CCMenu::create(subMenuItem, addMenuItem, NULL);
	menu->alignItemsHorizontallyWithPadding(120);
	addChild(menu,0);
	menu->setPosition(ccp(winSize.width/2-20, addMenuItem->getContentSize().height/2));
	return true;
}

void CannonLayer::switchCannonCallback(cocos2d::CCObject* sender)
{
	CannonOperate operate = k_Cannon_Operate_Up;
	if((CCMenuItemImage*)sender == subMenuItem)
	{
		operate = k_Cannon_Operate_Down;
	}
	weapon->changeCannon(operate);

}

void CannonLayer::aimAt(CCPoint target)
{
	weapon->aimAt(target);
}

void CannonLayer::shootTo(CCPoint target)
{
	weapon->shootTo(target);
}
