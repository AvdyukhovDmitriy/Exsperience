#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <memory>

namespace proto
{
	enum class WarriorId
	{
		INFANTRYMAN,
		ARCHER,
		HORSEMAN
	};

	class Warrior;

	using Registry = std::map<WarriorId, std::shared_ptr<Warrior>>;

	Registry& GetRegistry()
	{
		static Registry instance;
		return instance;
	}

	class Dummy {};

	class Warrior
	{
	public:
		virtual std::shared_ptr<Warrior> Clone() = 0;
		virtual void Info() = 0;
		virtual ~Warrior() {}
		static std::shared_ptr<Warrior> CreateWarrior(WarriorId war_id)
		{
			Registry& reg = GetRegistry();
			if (reg.find(war_id) != reg.end())
			{
				return reg[war_id];
			}
			return nullptr;
		}

	protected:
		static void AddPrototype(WarriorId war_id, std::shared_ptr<Warrior> proto)
		{
			Registry& reg = GetRegistry();
			reg.insert({ war_id, proto });
		}

		static void RemovePrototype(WarriorId war_id)
		{
			Registry& reg = GetRegistry();
			reg.erase(reg.find(war_id));
		}

	};

	class Infantryman : public Warrior
	{
	public:
		std::shared_ptr<Warrior> Clone() override
		{
			return std::make_shared<Infantryman>(*this);
		}

		void Info() override
		{
			std::cout << "Infantryman" << std::endl;
		}

	private:
		Infantryman(Dummy)
		{			
			Warrior::AddPrototype(WarriorId::INFANTRYMAN, std::make_shared<Infantryman>(*this));
		}

		Infantryman() {}
		static Infantryman prototype_;
	};

	class Archer : public Warrior
	{
	public:
		std::shared_ptr<Warrior> Clone() override
		{
			return std::make_shared<Archer>(*this);
		}

		void Info() override
		{
			std::cout << "Archer" << std::endl;
		}

	private:
		Archer(Dummy)
		{
			Warrior::AddPrototype(WarriorId::ARCHER, std::make_shared<Archer>(*this));
		}

		Archer() {}
		static Archer prototype_;
	};

	class Horseman : public Warrior
	{
	public:
		std::shared_ptr<Warrior> Clone() override
		{
			return std::make_shared<Horseman>(*this);
		}

		void Info() override
		{
			std::cout << "Horseman" << std::endl;
		}

	private:
		Horseman(Dummy)
		{
			Warrior::AddPrototype(WarriorId::HORSEMAN, std::make_shared<Horseman>(*this));
		}

		Horseman() {}
		static Horseman prototype_;
	};

	Infantryman Infantryman::prototype_ = Infantryman(Dummy());
	Archer Archer::prototype_ = Archer(Dummy());
	Horseman Horseman::prototype_ = Horseman(Dummy());
}

namespace proto_with_factory
{
	class Warrior 
	{
	public:
		virtual ~Warrior() {}
		virtual std::shared_ptr<Warrior> Clone() = 0;
		virtual void Info() = 0;
	};

	class Infantryman : public Warrior
	{
	public:
		friend class PrototypeFactory;
		std::shared_ptr<Warrior> Clone() override
		{
			return std::make_shared<Infantryman>(*this);
		}

		void Info() override
		{
			std::cout << "Infantryman" << std::endl;
		}

	private:
		Infantryman() {}
	};

	class Archer : public Warrior
	{
	public:
		friend class PrototypeFactory;
		std::shared_ptr<Warrior> Clone() override
		{
			return std::make_shared<Archer>(*this);
		}

		void Info() override
		{
			std::cout << "Archer" << std::endl;
		}

	private:
		Archer() {}
	};

	class Horseman : public Warrior
	{
	public:
		friend class PrototypeFactory;
		std::shared_ptr<Warrior> Clone() override
		{
			return std::make_shared<Horseman>(*this);
		}

		void Info() override
		{
			std::cout << "Horseman" << std::endl;
		}

	private:
		Horseman() {}
	};

	class PrototypeFactory
	{
	public:
		std::shared_ptr<Warrior> CreateInfantryman()
		{
			static Infantryman proto;
			return proto.Clone();
		}

		std::shared_ptr<Warrior> CreateArcher()
		{
			static Archer proto;
			return proto.Clone();
		}

		std::shared_ptr<Warrior> CreateHorseman()
		{
			static Horseman proto;
			return proto.Clone();
		}
	};
}

void TestPrototype()
{
	{
		using namespace proto;
		std::vector<std::shared_ptr<Warrior>> army;
		army.push_back(Warrior::CreateWarrior(WarriorId::INFANTRYMAN));
		army.push_back(Warrior::CreateWarrior(WarriorId::ARCHER));
		army.push_back(Warrior::CreateWarrior(WarriorId::HORSEMAN));
		for (const auto& warrior : army)
		{
			warrior->Info();
		}		
	}
	{
		using namespace proto_with_factory;
		PrototypeFactory factory;
		std::vector<std::shared_ptr<Warrior>> army;
		army.push_back(factory.CreateArcher());
		army.push_back(factory.CreateHorseman());
		army.push_back(factory.CreateInfantryman());
		for (const auto& warrior : army)
		{
			warrior->Info();
		}				
	}
}