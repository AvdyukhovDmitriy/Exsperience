#pragma once
#include <iostream>
#include <memory>


namespace decorator
{
	class I
	{
	public:
		virtual ~I() {}
		virtual void ToDo() = 0;
	};

	class A : public I
	{
	public:
		~A()
		{
			std::cout << " A destroy \n";
		}
		void ToDo() override
		{
			std::cout << "I ";
		}
	};

	class Core : public I
	{
	public:
		Core(std::shared_ptr<I> inner) : wrapper_(inner)
		{}

		~Core() override {}

		void ToDo()
		{
			wrapper_->ToDo();
		}

	private:
		std::shared_ptr<I> wrapper_ = nullptr;
	};

	class DecorWrapper : public Core
	{
	public:
		DecorWrapper(std::shared_ptr<I> core) : Core(core) {}
		~DecorWrapper() override
		{ 
			std::cout << "DecorWrapper destroy " ; 
		}

		void ToDo() override
		{
			Core::ToDo();
			std::cout << "How beautiful ";
		}
	};

	

	class Function : public Core
	{
	public:
		Function(std::shared_ptr<I> core) : Core(core) {}
		~Function() override
		{
			std::cout << "Function destroy ";
		}

		void ToDo() override
		{
			Core::ToDo();
			std::cout << "How  functional ";
		}
	};

	class Template : public Core
	{
	public:
		Template(std::shared_ptr<I> core) : Core(core) {}

		~Template() override
		{
			std::cout << "Template destroy ";
		}

		void ToDo() override
		{
			Core::ToDo();
			std::cout << "As a template ";
		}
	};

	//-----------Widget--------------

	// 1 - lowest common denominator LCD
	class Widget
	{
	public:
		virtual ~Widget() {}
		virtual void Draw() const = 0;
	};

	// 2 - base class two level
	class Decorator : public Widget
	{
	public:
		Decorator(std::shared_ptr<Widget> widget)
			: widget_(widget)
		{}

		void Draw() const override
		{
			widget_->Draw(); // 6 - delegation
		}

	private:
		std::shared_ptr<Widget> widget_ = nullptr;
	};

	// 3 -  main class using relation "is"
	class TextField : public Widget
	{
	public:
		TextField(int wegth, int height)
			: wegth_(wegth), height_(height)
		{}

		void Draw() const override
		{
			std::cout << "TextField: " << wegth_ << ", " << height_ << std::endl;
		}

	private:
		int wegth_ = 0, height_ = 0;
	};

	struct Point
	{
		Point() = default;
		Point(int _x, int _y) :x(_x), y(_y){}
		int x = 0;
		int y = 0;
	};

	class Cyrcle : public Widget
	{
	public:
		Cyrcle(Point pos, double radius)
			: pos_(pos), radius_(radius)
		{}

		void Draw() const override
		{
			std::cout << "Cyrcle:\npos: x = " << pos_.x 
				<< ", y= " << pos_.y 
				<< " Radius: " << radius_ << std::endl;
		}
	private:
		Point pos_{};
		double radius_ = 0.;
	};
	// 4 - Decorator class
	class BordeDecorator : public Decorator
	{
	public:
		BordeDecorator(std::shared_ptr<Widget> widget) : Decorator(widget) {}

		void Draw() const override
		{
			Decorator::Draw(); // 7 - Delegate to base class and add extra stuff
			std::cout << "  BordeDecorator" << std::endl;
		}
	};

	// 5 -  Decorator class
	class ScrollDecorator : public Decorator
	{
	public:
		ScrollDecorator(std::shared_ptr<Widget> widget) : Decorator(widget) {}

		void Draw() const override
		{
			Decorator::Draw(); // 7 - Delegate to base class and add extra stuff
			std::cout << "  ScrollDecorator" << std::endl;
		}
	};

}// end namespace





void TestDecorator()
{
	using namespace decorator;
	std::shared_ptr<I> w_decor_wapper = std::make_shared<DecorWrapper>(std::make_shared<A>());
	std::shared_ptr<I> w_decor_wapper_w_function = std::make_shared<DecorWrapper>(std::make_shared<Function>(std::make_shared<A>()));
	std::shared_ptr<I> w_decor_wapper_w_function_w_template 
		= std::make_shared<DecorWrapper>(std::make_shared<Function>(std::make_shared<Template>(std::make_shared<A>())));

	w_decor_wapper->ToDo();
	std::cout << std::endl;
	w_decor_wapper_w_function->ToDo();
	std::cout << std::endl;
	w_decor_wapper_w_function_w_template->ToDo();
	std::cout << std::endl;


	//---------Widget-----------
	std::shared_ptr<Widget> widget
		= std::make_shared<BordeDecorator>(
			std::make_shared<BordeDecorator>(
				std::make_shared<ScrollDecorator>(
					std::make_shared<TextField>(80, 24))));
	widget->Draw();

	Point p{ 80, 24 };
	std::shared_ptr<Widget> widget_c
		= std::make_shared<BordeDecorator>(
			std::make_shared<ScrollDecorator>(
				std::make_shared<BordeDecorator>(
					std::make_shared<Cyrcle>(p, 20))));
	widget_c->Draw();
	std::cout << "Test Decorator is Ok" << std::endl;
}