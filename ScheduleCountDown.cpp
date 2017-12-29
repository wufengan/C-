#include "ScheduleCountDown.h"

USING_NS_CC;
ScheduleCountDown* ScheduleCountDown::create(ScheduleCounterDelegate* target, int perimeter, bool loop)
{
	ScheduleCountDown *countDown=new ScheduleCountDown;
	if(countDown && countDown->init(target,perimeter,loop))
	{
		countDown->autorelease();
		return countDown;
	}
	CC_SAFE_DELETE(countDown);
	return NULL;
}
//将参数保存到相应的数据成员中。
bool ScheduleCountDown::init(ScheduleCounterDelegate* target, int perimeter, bool loop)
{
	if(!CCNode::init())
	{
		return false;
	}
	_target=target;
	_maxTime =_currentTime = perimeter;
	_loop=loop;

	schedule(schedule_selector(ScheduleCountDown::schedulePerSecond),1.0f);
	return true;
}
//倒计时主要函数，每间隔1秒被调用到的函数
void ScheduleCountDown::schedulePerSecond(float delta)
{
	_currentTime--;
	if(_currentTime <0)
	{
		_target->scheduleTimeUp();
		if(_loop==true)
		{
			_currentTime =_maxTime;
		}
		else
		{
			this->unschedule(schedule_selector(ScheduleCountDown::schedulePerSecond));
		}
		return ;
	}
	else
	_target->setScheduleNumber(_currentTime);
}
