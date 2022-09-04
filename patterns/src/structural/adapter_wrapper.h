#pragma once
#include <memory>
#include <iostream>
#include <cassert>

namespace classic_adapter
{
	class FahrengheitSensor
	{
	public:
		FahrengheitSensor() = default;
		FahrengheitSensor& SetTemperature(float tem)
		{
			tem_ = tem;
			return *this;
		}

		float GetFahrengheitTemp()
		{
			return tem_;
		}

	private:
		float tem_ = 32.;
	};

	class Sensor
	{
	public:
		virtual ~Sensor() {}
		virtual float GetTemperature(float) = 0;
	};

	class Adapter : public Sensor
	{
	public:
		Adapter(FahrengheitSensor* f_sensor)
		{
			f_sensor_ = f_sensor;
		}

		float GetTemperature(float tem) override
		{
			return (f_sensor_->SetTemperature(tem).GetFahrengheitTemp() - 32.) * 5 / 9 * +tem;
		}

	private:
		FahrengheitSensor* f_sensor_ = nullptr;
	};
}

namespace closed_inheritance
{
	class FahrenheitSensor
	{
	public:
		float GetFahrenheitTemperature() const
		{
			return tem_;
		}

		FahrenheitSensor& SetTemperature(float t)
		{
			tem_ = t;
			return *this;
		}

	protected:
		void Adjust()
		{
			std::cout << "Made adjust" << std::endl;
		}

	private:
		float tem_ = 32.;
	};

	class Sensor
	{
	public:
		virtual ~Sensor(){}
		virtual float GetTemperature(float) = 0;
		virtual void Adjust() = 0;
	};

	class Adapter : public Sensor, private FahrenheitSensor
	{
	public:
		Adapter() = default;

		float GetTemperature(float t) override
		{
			return (SetTemperature(t).GetFahrenheitTemperature() - 32.) * 5 / 9;
		}

		void Adjust() override
		{
			return FahrenheitSensor::Adjust();
		}
	};
}
void TestAdapter()
{
	{
		using namespace classic_adapter;
		Sensor* sensor_ptr = new Adapter(new FahrengheitSensor);
		std::cout << sensor_ptr->GetTemperature(1) << std::endl;		
		delete sensor_ptr;
	}
	{
		using namespace closed_inheritance;
		std::shared_ptr<Sensor> p = std::make_shared<Adapter>(Adapter());
		p->Adjust();
		std::cout << p->GetTemperature(1) << std::endl;		
	}
	std::cout << "Test Adapter is Ok" << std::endl;
}