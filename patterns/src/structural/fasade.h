#pragma once
#include <iostream>
#include <sstream>


namespace department
{
	class MisDepartment
	{
	public:
		MisDepartment() = default;

		void SubmitNetworckRequest()
		{
			state_ = 0;
		}

		bool CheckOnStatus()
		{
			++state_;
			if (state_ == static_cast<int>(States::Complete))
			{
				return true;
			}
			return false;
		}

	private:
		enum class States
		{
			Received, DenyAllKnowledge, ReferClientToFacilities,
			FacilitiesHasNotSentPaperwork, ElectricianIsNotDone,
			ElectricianDidItWrong, DispatchTechnician, SignedOff,
			DoesNotWork, FixElectriciansWiring, Complete
		};
		int state_ = 0;
	};


	class ElectricianUnion
	{
	public:
		ElectricianUnion() = default;

		void SubmitNetworckRequest()
		{
			state_ = 0;
		}

		bool CheckOnStatus()
		{
			++state_;
			if (state_ == static_cast<int>(States::Complete))
			{
				return true;
			}
			return false;
		}
	private:
		enum class States
		{
			Received, RejectTheForm, SizeTheJob, SmokeAndJokeBreak,
			WaitForAuthorization, DoTheWrongJob, BlameTheEngineer,
			WaitToPunchOut, DoHalfAJob, ComplainToEngineer,
			GetClarification, CompleteTheJob, TurnInThePaperwork,
			Complete
		};
		int state_ = 0;
	};

	class FacilitiesDepartment
	{
	public:
		void SubmitNetworckRequest()
		{
			state_ = 0;
		}

		bool CheckOnStatus()
		{
			++state_;
			if (state_ == static_cast<int>(States::Complete))
			{
				return true;
			}
			return false;
		}

	private:
		enum class States
		{
			Received, AssignToEngineer, EngineerResearches,
			RequestIsNotPossible, EngineerLeavesCompany,
			AssignToNewEngineer, NewEngineerResearches,
			ReassignEngineer, EngineerReturns,
			EngineerResearchesAgain, EngineerFillsOutPaperWork,
			Complete
		};
		int state_ = 0;
	};

	class FacilitiesFasade
	{
	public:
		FacilitiesFasade() = default;
		void SubmitNetworckRequest()
		{
			state_ = 0;
		}
		bool CheckOnStatus()
		{
			++count_;
			if (state_ == static_cast<int>(States::Received))
			{
				++state_;
				engneer_.SubmitNetworckRequest();
				std::cout << "Submitted to Facilitiaes - " << count_
					<< " phone calls so far" << std::endl;
			}
			else if (state_ == static_cast<int>(States::SubmitToEngineer))
			{
				if (engneer_.CheckOnStatus())
				{
					++state_;
					electrician_.SubmitNetworckRequest();
					std::cout << "Submitted to Electrician - " << count_
						<< " phone calls so far" << std::endl;
				}
			}
			else if (state_ == static_cast<int>(States::SubmitToElectrician))
			{
				if (electrician_.CheckOnStatus())
				{
					++state_;
					technician_.SubmitNetworckRequest();
					std::cout << "Submitted to Mis - " << count_
						<< " phone calls so far" << std::endl;
				}
			}
			else if (state_ == static_cast<int>(States::SubmitToTechnician))
			{
				if (technician_.CheckOnStatus())
				{
					return true;
				}
			}
			return false;
		}

		int GetNumberOfCalls() const
		{
			return count_;
		}

	private:
		enum class States
		{
			Received, SubmitToEngineer, SubmitToElectrician,
			SubmitToTechnician
		};
		int state_ = 0;
		int count_ = 0;
		MisDepartment technician_;
		ElectricianUnion electrician_;
		FacilitiesDepartment engneer_;
	};
}

void TestFasade()
{
	{ // namespace department
		department::FacilitiesFasade facilities;
		while (!facilities.CheckOnStatus())
		{
		}
		std::cout << "Job completed after only "
			<< facilities.GetNumberOfCalls()
			<< " phone calls" << std::endl;
	}	
}