#include "state.h"

Phone::Phone()
{
	state_ = std::make_shared<WaitingState>();
}

void Phone::SetState(std::shared_ptr<State> state)
{
	state_ = state;
}

void Phone::SwitchState()
{
	state_->UpdateState(*this);
}

WaitingState::WaitingState()
{
	std::cout << "Waiting state" << std::endl;
}

void WaitingState::UpdateState(Phone& phone)
{	
	phone.SetState(std::make_shared<DialingState>());
}

DialingState::DialingState()
{
	std::cout << "Dialing state" << std::endl;
}

void DialingState::UpdateState(Phone& phone)
{	
	phone.SetState(std::make_shared<TalkingState>());
}

TalkingState::TalkingState()
{
	std::cout << "Talking state" << std::endl;
}

void TalkingState::UpdateState(Phone& phone)
{
	phone.SetState(std::make_shared<WaitingState>());
}

void TestState()
{
	Phone phone;
	for (size_t i = 0; i < 6; i++)
	{
		phone.SwitchState();
	}
}
