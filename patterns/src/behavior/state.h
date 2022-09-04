#pragma once
#include <iostream>
#include <memory>

class Phone;
class WaitingState;
class DialingState;
class TalkingState;

class State
{
public:
	virtual ~State() {}
	virtual void UpdateState(Phone& phone) = 0;
};

class Phone
{
public:
	Phone();	
	void SetState(std::shared_ptr<State> state);	
	void SwitchState();
	
private:
	std::shared_ptr<State> state_ ;
};

class WaitingState : public State
{
public:
	WaitingState();	
	void UpdateState(Phone& phone) override;	
};

class DialingState : public State
{
public:
	DialingState();	
	void UpdateState(Phone& phone) override;	
};

class TalkingState : public State
{
public:
	TalkingState();	
	void UpdateState(Phone& phone) override;	
};

void TestState();