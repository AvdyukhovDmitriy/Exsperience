#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <map>

namespace singleton
{
	class Singleton;

	class SingletonDestroyer
	{
	private:
		Singleton* p_instance_;

	public:
		~SingletonDestroyer();
		void Initialize(Singleton* singleton);
	};


	class Singleton
	{
	public:
		static Singleton* Instance();
		std::string GetName() const;

	protected:
		Singleton();
		Singleton(const Singleton&) = delete;
		Singleton operator=(const Singleton&) = delete;
		~Singleton() {};
		friend class SingletonDestroyer;
	private:
		static Singleton* instance_;
		static SingletonDestroyer destroyer_;
		std::string name_;
	};

	class WaterSpring;

	class WaterSpringDestroyer
	{
	private:
		WaterSpring* w_;
	public:
		~WaterSpringDestroyer();
		void Initialize(WaterSpring* water_spring);
	};

	class WaterSpring
	{
	public:
		static WaterSpring* Instance();
		double GetMaxWater() const;
		double GetWater(double v);

	protected:
		WaterSpring();
		~WaterSpring() {}
		friend class WaterSpringDestroyer;

	private:
		WaterSpring(const WaterSpring&) = delete;
		WaterSpring operator=(const WaterSpring&) = delete;
		static WaterSpringDestroyer w_destroy_;
		static WaterSpring* w_instance_;
		double max_water_;
	};

	class Teapot
	{
	public:
		Teapot(double v);
		void MakeTea(double v);

	private:
		double volume_ = 0.;
		double AddWater(double v);
	};

	class Multiton
	{
	public:
		static void DestroyAll()
		{
			for (auto it = instances_.begin(); it != instances_.end(); ++it)
			{
				delete it->second;
			}
			instances_.clear();
		}

		static void Destroy(const std::string& key)
		{
			auto it = instances_.find(key);
			if (it != instances_.end())
			{
				delete it->second;
				instances_.erase(it);
			}
		}

		static Multiton* GetPtr(const std::string& key)
		{
			if (instances_.find(key) == instances_.end())
			{
				instances_[key] = new Multiton(key);
			}
			return instances_[key];
		}

		std::string GetName()
		{
			return name_;
		}

	protected:
		explicit Multiton(std::string name) : name_(name)
		{}
		~Multiton() {}


	private:
		static std::map<std::string, Multiton*> instances_;
		std::string name_;
	};

}// end namespace

void TestSingleton();
