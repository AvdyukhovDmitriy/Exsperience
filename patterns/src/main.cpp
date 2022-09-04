#include "generative/singleton.h"
#include "generative/factory_method.h"
#include "generative/object_pool.h"
#include "generative/prototype.h"
#include "generative/abstract_factory.h"
#include "generative/builder.h"

#include "structural/adapter_wrapper.h"
#include "structural/decorator.h"
#include "structural/fasade.h"

#include "behavior/visitor.h"
#include "behavior/obsever.h"
#include "behavior/chain_of_responsibility.h"
#include "behavior/mediator.h"
#include "behavior/strategy.h"
#include "behavior/state.h"
#include "behavior/command.h"
#include "behavior/iterator.h"

int main()
{
	//////-------generative-----------
	//TestAbstractFactory();
	//TestSingleton();
	//TestFactoryMethod();
	//TestPrototype();
	//TestObjectPool();

	//////----------structurial----------
	//TestAdapter();	
	//TestBuilder();	
	//TestDecorator();
	//TestFasade();

	//////-----------behavior------------
	//TestVisitor();
	//TestObserver();
	//TestChainOfResponsibility();
	//TestMediator();
	//TestDiscountable();
	//TestState();
	//TestCommand();
	TestIterator();
	return 0;
}