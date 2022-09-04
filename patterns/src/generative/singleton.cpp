#include "singleton.h"

namespace singleton
{
	//------------Singleton---------------

	Singleton* Singleton::instance_ = nullptr; //реализация static поля
	SingletonDestroyer Singleton::destroyer_;  //реализация static поля

	SingletonDestroyer::~SingletonDestroyer()
	{
		delete p_instance_;
	}

	void SingletonDestroyer::Initialize(Singleton* singleton)
	{
		p_instance_ = singleton;
	}

	Singleton* Singleton::Instance()
	{
		if (instance_ == nullptr)
		{
			instance_ = new Singleton();
		}
		destroyer_.Initialize(instance_);
		return instance_;
	}

	std::string Singleton::GetName() const
	{
		return name_;
	}

	Singleton::Singleton()
	{
		name_ = "User";
	}

	//--------------WaterSpring-----------------

	WaterSpringDestroyer WaterSpring::w_destroy_;  //реализация static поля
	WaterSpring* WaterSpring::w_instance_ = nullptr;  //реализация static поля

	WaterSpring* WaterSpring::Instance()
	{
		if (!w_instance_)
		{
			w_instance_ = new WaterSpring();
		}
		w_destroy_.Initialize(w_instance_);
		return w_instance_;
	}



	WaterSpringDestroyer::~WaterSpringDestroyer()
	{
		delete w_;
	}

	void WaterSpringDestroyer::Initialize(WaterSpring* water_spring)
	{
		w_ = water_spring;
	}

	WaterSpring::WaterSpring()
	{
		max_water_ = 21;
	}

	double WaterSpring::GetMaxWater() const
	{
		return max_water_;
	}

	double WaterSpring::GetWater(double v)
	{
		if (max_water_ - v >= 0)
		{
			max_water_ -= v;
			return v;
		}
		return 0;
	}

	Teapot::Teapot(double v)
		: volume_(v)
	{}

	void Teapot::MakeTea(double v)
	{
		if (double w = AddWater(v); w)
		{
			std::cout << "The tespot with " << w << " water" << std::endl;
		}
		else
		{
			std::cout << "Can't add water" << std::endl;
		}
	}

	double Teapot::AddWater(double v)
	{
		WaterSpring* intance = WaterSpring::Instance();
		return intance->GetWater(v);
	}
	std::map<std::string, Multiton*> Multiton::instances_;

}// end namespace

void TestSingleton()
{
	using namespace singleton;

	Singleton* s = Singleton::Instance();
	{
		Singleton* s1 = Singleton::Instance();
		std::cout << s1->GetName() << std::endl;
	}
	std::cout << s->GetName() << std::endl;
	WaterSpring* w1 = WaterSpring::Instance();
	std::cout << w1->GetMaxWater() << std::endl;
	w1->GetWater(5);
	WaterSpring* w2 = WaterSpring::Instance();
	std::cout << w2->GetMaxWater() << std::endl;
	w2->GetWater(15);
	std::cout << w2->GetMaxWater() << std::endl;
	std::cout << w1->GetMaxWater() << std::endl;

	Teapot teapot(3);
	teapot.MakeTea(1.7);
	std::cout << w1->GetMaxWater() << std::endl;

	auto m = Multiton::GetPtr("qwe");
	auto m1 = Multiton::GetPtr("asd");
	auto m2 = Multiton::GetPtr("zxc");
	std::cout << m->GetName() << std::endl;
	std::cout << m1->GetName() << std::endl;
	std::cout << m2->GetName() << std::endl;
	m1->Destroy("qwe");

	std::cout << "Test Singleton is Ok" << std::endl;
}


