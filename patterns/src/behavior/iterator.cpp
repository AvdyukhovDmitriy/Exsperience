#include "iterator.h"

namespace iterator
{
	iterator::GoodsIterator::GoodsIterator(const MarcetStore& store)
		: store_(std::make_shared<MarcetStore>(store)) 
	{}

	bool iterator::GoodsIterator::HasNext()
	{
		if (index_ < store_->goods_.size())
		{
			return true;
		}
		return false;
	}

	std::optional<Product> iterator::GoodsIterator::Next() 
	{
		if (HasNext())
		{
			return store_->goods_[index_++];
		}
		return {};
	}

	int GoodsIterator::GetIteratorPosition()
	{
		return index_;
	}

	std::optional<Product> GoodsIterator::GetItem(int id)
	{
		if (id < store_->goods_.size())
		{
			return store_->goods_[id];
		}
		return {};
	}

	bool GoodsIterator::RemoveItem(int id)
	{
		if (id < store_->goods_.size())
		{
			store_->goods_.erase(store_->goods_.begin() + id);
			return true;
		}
		return false;
	}

	void GoodsIterator::Reset()
	{
		index_ = 0;
	}

	MarcetStore::MarcetStore(const std::string& name)
		: name_store_(name)
	{}

	MarcetStore& MarcetStore::AddProduct(Product&& product)
	{
		goods_.push_back(std::move(product));
		return *this;
	}

	std::vector<Product> MarcetStore::GetGoods() const
	{
		return goods_;
	}

	std::shared_ptr<MyIterator<Product>> MarcetStore::GetIterator() 
	{
		return std::make_shared<GoodsIterator>(GoodsIterator(*this));
	}

} //end namespace

void TestIterator()
{
	using namespace iterator;
	MarcetStore store("Story");
	store.AddProduct({ "qwe" }).AddProduct({ "asd" }).AddProduct({ "zxc" });

	auto it = store.GetIterator();
	auto it1 = store.GetIterator();

	while (it->HasNext())
	{
		std::cout << it->Next().value().name << std::endl;
	}
	std::cout << it->GetItem(2).value().name << std::endl;
	it1->RemoveItem(2);
	it->Reset();
	while (it->HasNext())
	{
		std::cout << it->Next().value().name << std::endl;
	}
}