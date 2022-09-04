#pragma once
#include "generative/factory_method.h"

namespace builder
{
	using namespace factory;
	class Army
	{
	public:
		void Info() const
		{
			std::cout << "Infantrymans: " << infantrymen_.size() << " count " << std::endl;
			for (const auto& warrior : infantrymen_)
			{
				warrior->Info();
			}
			std::cout << "Archers: " << archers_.size() << " count " << std::endl;
			for (const auto& warrior : archers_)
			{
				warrior->Info();
			}
			std::cout << "Horsemen: " << horsemen_.size() << " count " << std::endl;
			for (const auto& warrior : horsemen_)
			{
				warrior->Info();
			}
			std::cout << "Catapults: " << catapults_.size() << " count " << std::endl;
			for (const auto& warrior : catapults_)
			{
				warrior->Info();
			}
			std::cout << "Elephants: " << elephants_.size() << " count" << std::endl;
			for (const auto& warrior : elephants_)
			{
				warrior->Info();
			}
		}

		std::vector<std::shared_ptr<Infantryman>>& GetInfantrymans()
		{
			return infantrymen_;
		}

		std::vector<std::shared_ptr<Archer>>& GetArchers()
		{
			return archers_;
		}

		std::vector<std::shared_ptr<Horseman>>& GetHorsemen()
		{
			return horsemen_;
		}

		std::vector<std::shared_ptr<Catapult>>& GetCatapults()
		{
			return catapults_;
		}

		std::vector<std::shared_ptr<Elephant>>& GetElephants()
		{
			return elephants_;
		}

	private:
		std::vector<std::shared_ptr<Infantryman>> infantrymen_;
		std::vector<std::shared_ptr<Archer>> archers_;
		std::vector<std::shared_ptr<Horseman>> horsemen_;
		std::vector<std::shared_ptr<Catapult>> catapults_;
		std::vector<std::shared_ptr<Elephant>> elephants_;
	};

	class ArmyBuilder
	{
	protected:
		std::shared_ptr<Army> army_ptr_ = nullptr;

	public:
		ArmyBuilder() = default;
		virtual ~ArmyBuilder() {}
		virtual void CreateArmy() {}
		virtual void CreateInfantryman() {}
		virtual void CreateArcher() {}
		virtual void CreateHorseman() {}
		virtual void CreateCatapult() {}
		virtual void CreateElephant() {}

		virtual std::shared_ptr<Army> GetArmy()
		{
			return army_ptr_;
		}
	};

	class RomanArmyBuilder : public ArmyBuilder
	{
	public:
		void CreateArmy() override
		{
			army_ptr_ = std::make_shared<Army>();
		}

		void CreateInfantryman() override
		{
			army_ptr_->GetInfantrymans() = { 10, std::make_shared <Infantryman>() };
		}

		void CreateArcher() override
		{
			army_ptr_->GetArchers() = { 10, std::make_shared <Archer>() };
		}

		void CreateHorseman() override
		{
			army_ptr_->GetHorsemen() = { 10, std::make_shared <Horseman>() };
		}

		void CreateCatapult() override
		{
			army_ptr_->GetCatapults() = { 5, std::make_shared<Catapult>() };
		}
	};

	class CarthagenArmyBuilder : public ArmyBuilder
	{
	public:
		void CreateArmy() override
		{
			army_ptr_ = std::make_shared<Army>();
		}

		void CreateInfantryman() override
		{
			army_ptr_->GetInfantrymans() = { 10, std::make_shared <Infantryman>() };
		}

		void CreateArcher() override
		{
			army_ptr_->GetArchers() = { 10, std::make_shared <Archer>() };
		}

		void CreateHorseman() override
		{
			army_ptr_->GetHorsemen() = { 5, std::make_shared <Horseman>() };
		}

		void CreateElephant() override
		{
			army_ptr_->GetElephants() = { 5, std::make_shared <Elephant>() };
		}
	};

	class Director
	{
	public:
		Director() {}

		std::shared_ptr<Army> CreateArmy(ArmyBuilder& builder)
		{
			builder.CreateArmy();
			builder.CreateInfantryman();
			builder.CreateArcher();
			builder.CreateHorseman();
			builder.CreateCatapult();
			builder.CreateElephant();
			return builder.GetArmy();
		}
	};
}

void TestBuilder()
{
	using namespace builder;
	Director dir;
	RomanArmyBuilder roman;
	CarthagenArmyBuilder carthagen;
	std::shared_ptr<builder::Army> roman_army = dir.CreateArmy(roman);
	std::shared_ptr<builder::Army> carthagen_army = dir.CreateArmy(carthagen);
	std::cout << "Roman army: ";
	roman_army->Info();
	std::cout << "Carthagen army: ";
	carthagen_army->Info();
	std::cout << std::endl;		

	std::cout << "Test Builder is Ok" << std::endl;
}