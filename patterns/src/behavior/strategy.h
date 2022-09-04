#pragma once
#include <iostream>
#include <vector>

class Product
{
public:
	Product() = delete;
	Product(int id, std::string name, double cost)
		: id_(id), name_(name), cost_(cost)
	{}

	double GetCost() const
	{
		return cost_;
	}

private:
	int id_ = 0;
	std::string name_{};
	double cost_ = 0.;
};

class Discountable
{
public:
	virtual ~Discountable() {}
	Discountable(double discount) : discount_(discount) {}
	virtual double ApplyDiscountable(double) = 0;
	double GetDiscountable() const
	{
		return discount_;
	}

private:
	double discount_ = 0.;
};

class DiscountablePercent : public Discountable
{
public:
	DiscountablePercent(double value) : Discountable(value) {}
	double ApplyDiscountable(double value) override
	{
		return value - value * GetDiscountable() / 100;
	}
};

class DiscountablePermanent : public Discountable
{
public:
	DiscountablePermanent(double value) : Discountable(value) {}
	double ApplyDiscountable(double value) override
	{
		return value - GetDiscountable();
	}
};

class ShopingCart
{
public:
	ShopingCart() = default;

	ShopingCart& AddCart(std::shared_ptr<Product> product)
	{
		if (product)
		{
			shoping_cart_.push_back(product);
		}
		return *this;
	}

	void RemoveProduct(std::shared_ptr<Product> product)
	{
		auto it_remove = std::find(shoping_cart_.begin(), shoping_cart_.end(), product);
		if (it_remove != shoping_cart_.end())
		{
			shoping_cart_.erase(it_remove);
		}
	}

	void AddDiscountable(std::shared_ptr<Discountable> discount)
	{
		discount_ = discount;
	}

	std::shared_ptr<Discountable> GetDiscount() const
	{
		return discount_;
	}

	double CalculetTotal() const
	{
		double result = 0.;
		for (const auto& product : shoping_cart_)
		{
			result += product->GetCost();
		}
		if (discount_ && result > 50)
		{
			result = discount_->ApplyDiscountable(result);
		}
		return result;
	}

private:
	std::vector<std::shared_ptr<Product>> shoping_cart_;
	std::shared_ptr<Discountable> discount_ = nullptr;
};

void TestDiscountable()
{
	Product apple(1, "apple", 20.);
	Product cherry(2, "cherry", 25.);
	Product peach(1, "peach", 30.);
	DiscountablePercent discount1(20);
	DiscountablePermanent discount2(30);
	ShopingCart my_cart;
	my_cart.AddCart(std::make_shared<Product>(apple)).AddCart(std::make_shared<Product>(cherry))
		.AddCart(std::make_shared<Product>(peach)).AddCart(std::make_shared<Product>(cherry));
	std::cout << "Cost my shopping cart without discount = " << my_cart.CalculetTotal() << std::endl;

	ShopingCart my_cart1;
	my_cart1.AddCart(std::make_shared<Product>(apple)).AddCart(std::make_shared<Product>(cherry))
		.AddCart(std::make_shared<Product>(peach)).AddCart(std::make_shared<Product>(cherry))
		.AddDiscountable(std::make_shared<DiscountablePercent>(discount1));
	std::cout << "Cost my shopping cart with precent discount equal: "
		<< my_cart1.GetDiscount()->GetDiscountable() << "  = "
		<< my_cart1.CalculetTotal() << std::endl;

	ShopingCart my_cart2;
	my_cart2.AddCart(std::make_shared<Product>(apple)).AddCart(std::make_shared<Product>(cherry))
		.AddCart(std::make_shared<Product>(peach)).AddCart(std::make_shared<Product>(cherry))
		.AddDiscountable(std::make_shared<DiscountablePermanent>(discount2));
	std::cout << "Cost my shopping cart with permanent discount equal: "
		<< my_cart2.GetDiscount()->GetDiscountable() << "  = "
		<< my_cart2.CalculetTotal() << std::endl;

}