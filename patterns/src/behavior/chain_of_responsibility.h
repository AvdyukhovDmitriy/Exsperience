#pragma once
#include <iostream>
#include <memory>

namespace ch_of_respon
{
	enum class FromTo
	{
		B2B,
		B2P,
		P2B,
		P2P
	};

	class MoneyTransfer
	{
	public:
		MoneyTransfer() = delete;
		MoneyTransfer(double money, std::string from, std::string to, std::string currency, FromTo from_to)
			: currency_(currency), money_(money), from_account_(from), to_account_(to), from_to_(from_to)
		{}

		FromTo GetFromTo() const
		{
			return from_to_;
		}
	private:
		std::string currency_{};
		double money_ = 0;
		std::string from_account_{};
		std::string to_account_{};
		FromTo from_to_;
	};

	class TransferMiddleware
	{
	public:
		virtual ~TransferMiddleware() {}
		TransferMiddleware() = default;
		TransferMiddleware(std::shared_ptr<TransferMiddleware> next)
			: next_(next)
		{}

		void Process(std::shared_ptr<MoneyTransfer> transfer)
		{
			if (CanDo(transfer))
			{
				bool next = Do(transfer);
				if (next)
				{
					return;
				}
			}
			if (!next_)
			{
				return;
			}
			next_->Process(transfer);
		}

		TransferMiddleware& Add(std::shared_ptr<TransferMiddleware> next)
		{
			if (next_)
			{
				next_->Add(next);
			}
			else
			{
				next_ = next;
			}
			return *this;
		}
		virtual bool CanDo(std::shared_ptr<MoneyTransfer>&) = 0;
		virtual bool Do(std::shared_ptr<MoneyTransfer>&) = 0;

	private:
		std::shared_ptr<TransferMiddleware> next_ = nullptr;
	};

	class B2BTransferMiddelware : public TransferMiddleware
	{
	public:
		B2BTransferMiddelware() : TransferMiddleware(nullptr) {}

		B2BTransferMiddelware(std::shared_ptr<TransferMiddleware> next) : TransferMiddleware(next) {}

		bool CanDo(std::shared_ptr<MoneyTransfer>& transfer) override
		{
			if (!transfer)
			{
				return false;
			}
			if (transfer->GetFromTo() == FromTo::B2B)
			{
				return true;
			}
			return false;
		}

		bool Do(std::shared_ptr<MoneyTransfer>& transfer) override
		{
			if (transfer)
			{
				std::cout << "B2B logic" << std::endl;
				return true;
			}
			return false;
		}
	};


	class B2PTransferMiddelware : public TransferMiddleware
	{
	public:
		B2PTransferMiddelware() : TransferMiddleware(nullptr) {}

		B2PTransferMiddelware(std::shared_ptr<TransferMiddleware> next) : TransferMiddleware(next) {}

		bool CanDo(std::shared_ptr<MoneyTransfer>& transfer) override
		{
			if (!transfer)
			{
				return false;
			}
			if (transfer->GetFromTo() == FromTo::B2P)
			{
				return true;
			}
			return false;
		}

		bool Do(std::shared_ptr<MoneyTransfer>& transfer) override
		{
			if (transfer)
			{
				std::cout << "B2P logic" << std::endl;
				return true;
			}
			return false;
		}
	};

	class P2BTransferMiddelware : public TransferMiddleware
	{
	public:
		P2BTransferMiddelware() : TransferMiddleware(nullptr) {}

		P2BTransferMiddelware(std::shared_ptr<TransferMiddleware> next) : TransferMiddleware(next) {}

		bool CanDo(std::shared_ptr<MoneyTransfer>& transfer) override
		{
			if (!transfer)
			{
				return false;
			}
			if (transfer->GetFromTo() == FromTo::P2B)
			{
				return true;
			}
			return false;
		}

		bool Do(std::shared_ptr<MoneyTransfer>& transfer) override
		{
			if (transfer)
			{
				std::cout << "P2B logic" << std::endl;
				return true;
			}
			return false;
		}
	};

	class P2PTransferMiddelware : public TransferMiddleware
	{
	public:
		P2PTransferMiddelware() : TransferMiddleware(nullptr) {}

		P2PTransferMiddelware(std::shared_ptr<TransferMiddleware> next) : TransferMiddleware(next) {}

		bool CanDo(std::shared_ptr<MoneyTransfer>& transfer) override
		{
			if (!transfer)
			{
				return false;
			}
			if (transfer->GetFromTo() == FromTo::P2P)
			{
				return true;
			}
			return false;
		}

		bool Do(std::shared_ptr<MoneyTransfer>& transfer) override
		{
			if (transfer)
			{
				std::cout << "P2P logic" << std::endl;
				return true;
			}
			return false;
		}
	};

}// end namespace

void TestChainOfResponsibility()
{
	using namespace ch_of_respon;

	std::shared_ptr<MoneyTransfer> transfer = std::make_shared<MoneyTransfer>(20., "SPB 0123245646544", "ALF 456456432131", "RUB", FromTo::B2P);
	
	std::shared_ptr<B2BTransferMiddelware> b2b = std::make_shared<B2BTransferMiddelware>();
	std::shared_ptr<P2PTransferMiddelware> p2p = std::make_shared<P2PTransferMiddelware>();;
	std::shared_ptr<P2BTransferMiddelware> p2b = std::make_shared<P2BTransferMiddelware>();
	std::shared_ptr<B2PTransferMiddelware> b2p = std::make_shared<B2PTransferMiddelware>();
	b2b->Add(p2p).Add(p2b).Add(b2p).Process(transfer);
	std::cout << "Test Chane of responsibility is ok" << std::endl;
}