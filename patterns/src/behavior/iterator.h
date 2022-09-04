#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <optional>


namespace iterator
{
	template<class Object>
	class MyIterator
	{
	public:
		virtual ~MyIterator() {}
		virtual bool HasNext() = 0;
		virtual std::optional<Object> Next() = 0;
		virtual int GetIteratorPosition() = 0;
		virtual std::optional<Object> GetItem(int id) = 0;
		virtual bool RemoveItem(int id) = 0;
		virtual void Reset() = 0;
	};

	template<class Object>
	class Colection
	{
	public:
		virtual ~Colection() {}
		virtual std::shared_ptr<MyIterator<Object>> GetIterator() = 0;
	};

	struct Product
	{
		std::string name{};
	};

	class MarcetStore;

	class GoodsIterator : public MyIterator<Product>
	{
	public:
		explicit GoodsIterator(const MarcetStore& store);

		bool HasNext() override;		

		std::optional<Product> Next() override;

		int GetIteratorPosition() override;

		std::optional<Product> GetItem(int id) override;

		bool RemoveItem(int id) override;

		void Reset() override;
		
	private:
		int index_ = 0;
		std::shared_ptr<MarcetStore> store_ = nullptr;
	};

	class MarcetStore : public Colection<Product>
	{
	public:
		friend class GoodsIterator;

		MarcetStore() = default;

		explicit MarcetStore(const std::string& name);

		MarcetStore& AddProduct(Product&& product);

		std::vector<Product> GetGoods() const;

		std::shared_ptr<MyIterator<Product>> GetIterator() override;

	private:
		std::string name_store_;
		std::vector<Product> goods_;
	};
	
} //end namespace

void TestIterator();