#pragma once
#include <iostream>
#include <vector>
#include <memory>

namespace mediator
{
	struct Address
	{
		Address() = default;
		Address(std::string stre, std::string number)
			: street(stre), number_home(number)
		{}
		std::string street = "qwe";
		std::string number_home = "34";
	};

	std::ostream& operator<<(std::ostream& out, const Address& address)
	{
		out << "Address: street " << address.street << " number home " << address.number_home;
		return out;
	}

	class Trip
	{
	public:
		Trip(bool assigned, Address address)
			: assigned_trip_(assigned), address_(address)
		{}

		Trip() = default;

		bool AssignedTrip()
		{
			return assigned_trip_;
		}

		void SetAssignedTrip()
		{
			assigned_trip_ = true;
		}

		Address GetAddress() const
		{
			return address_;
		}

	private:
		bool assigned_trip_ = false;
		Address address_{};
	};

	class Taxi
	{
	public:
		virtual ~Taxi() {}
		Taxi() = delete;
		Taxi(std::string name) : name_(name) {}
		virtual void SetBooked() = 0;
		virtual void SetFree() = 0;
		virtual void SetRest() = 0;
		virtual bool GetBusy() = 0;
		virtual void AssignTrip(Trip&) = 0;

		std::string GetName() const
		{
			return name_;
		}

		void SetTrip(Trip& trip)
		{
			trip_ = trip;
		}

	private:
		std::string name_{};
		Trip trip_{};
	};

	class Mediator
	{
	public:
		virtual void RequestAll(Trip& trip) = 0;
	};

	class ConcreteTaxi : public Taxi
	{
	public:
		ConcreteTaxi() = delete;

		ConcreteTaxi(const std::string& name, const std::string& status, bool busy, std::shared_ptr<Mediator> mediator)
			: Taxi(name), status_(status), busy_(busy), mediator_(mediator)
		{}

		void SetBooked() override
		{
			status_ = "Booked";
			busy_ = true;
			std::cout << "Taxi: " << GetName() << " is booked" << std::endl;
		}

		void SetFree() override
		{
			status_ = "Free";
			busy_ = false;
			std::cout << "Taxi: " << GetName() << " is free" << std::endl;
		}

		void SetRest() override
		{
			status_ = "Reet";
			busy_ = true;
			std::cout << "Taxi: " << GetName() << " is rest" << std::endl;
		}

		bool GetBusy() override
		{
			return busy_;
		}

		std::string GetStatus() const
		{
			return status_;
		}

		void AssignTrip(Trip& trip) override
		{
			mediator_->RequestAll(trip);
		}

	private:
		std::string status_{};
		bool busy_ = false;
		std::shared_ptr<Mediator> mediator_;
	};

	class ConcreteMediator : public Mediator
	{
	public:
		void RequestAll(Trip& trip) override
		{
			for (const auto& taxi : taxis_)
			{
				if (!taxi->GetBusy() && !trip.AssignedTrip())
				{
					trip.SetAssignedTrip();
					std::cout << "Trip to address " << trip.GetAddress() << " assined taxi: " << taxi->GetName() << std::endl;
					taxi->SetBooked();
					taxi->SetTrip(trip);
					break;
				}
			}
			if (!trip.AssignedTrip())
			{
				std::cout << "All taxis are busy! Please wait." << std::endl;
			}
		}

		ConcreteMediator& Add(std::shared_ptr<Taxi> taxi)
		{
			if (taxi)
			{
				taxis_.push_back(taxi);
			}
			return *this;
		}

	private:
		std::vector<std::shared_ptr<Taxi>> taxis_;
	};

}// end namespace

void TestMediator()
{
	using namespace mediator;

	Trip trip(false, { "asd", "23" });
	Trip trip1(false, { "qwe", "23" });
	Trip trip2(false, { "zxc", "23" });
	Trip trip3(false, { "fgh", "23" });
	std::shared_ptr<ConcreteMediator> mediator = std::make_shared<ConcreteMediator>();
	std::shared_ptr <ConcreteTaxi> taxi_1 = std::make_shared<ConcreteTaxi>("Ivan", "Free", false, mediator);
	std::shared_ptr <ConcreteTaxi> taxi_2 = std::make_shared<ConcreteTaxi>("Petr", "Busy", true, mediator);
	std::shared_ptr <ConcreteTaxi> taxi_3 = std::make_shared<ConcreteTaxi>("Andrey", "Free", false, mediator);
	std::shared_ptr <ConcreteTaxi> taxi_4 = std::make_shared<ConcreteTaxi>("Dmitriy", "Free", false, mediator);
	std::shared_ptr <ConcreteTaxi> taxi_5 = std::make_shared<ConcreteTaxi>("Alex", "Free", true, mediator);
	mediator->Add(taxi_1).Add(taxi_2).Add(taxi_3).Add(taxi_4).Add(taxi_5);	
	taxi_1->AssignTrip(trip);
	taxi_1->AssignTrip(trip1);
	taxi_2->AssignTrip(trip2);
	taxi_1->SetFree();
	taxi_2->AssignTrip(trip3);
}