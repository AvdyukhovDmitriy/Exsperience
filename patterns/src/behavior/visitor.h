#pragma once
#include <iostream>
#include <memory>
#include <vector>

namespace visitor
{
	class Visitor;

	class Element
	{
	public:
		Element() = default;
		virtual ~Element() {}
		virtual void ShowElement() = 0;
		virtual void Accept(std::shared_ptr<Visitor>) = 0;
	};


	class ElementA : public Element
	{
	public:
		void ShowElement() override
		{
			std::cout << "ElementA" << std::endl;
		}
		void Accept(std::shared_ptr<Visitor>) override;
	};

	class ElementB : public Element
	{
	public:
		void ShowElement() override
		{
			std::cout << "ElementB" << std::endl;
		}
		void Accept(std::shared_ptr<Visitor>) override;
	};

	class Visitor
	{
	public:
		virtual ~Visitor() {}
		virtual void Visit(std::shared_ptr<Element> elem) = 0;
	};

	class VisitorText : public Visitor
	{
	public:
		void Visit(std::shared_ptr<Element> elem) override
		{
			std::cout << "Text: ";
			elem->ShowElement();
		}
	};

	class VisitorJson : public Visitor
	{
	public:
		void Visit(std::shared_ptr<Element> elem) override
		{
			std::cout << "Json: ";
			elem->ShowElement();
		}
	};

	void ElementA::Accept(std::shared_ptr<Visitor> vis)
	{
		vis->Visit(std::make_shared<ElementA>(*this));
	}

	void ElementB::Accept(std::shared_ptr<Visitor> vis)
	{
		vis->Visit(std::make_shared<ElementB>(*this));
	}

	class VectorElements
	{
	public:
		VectorElements() = default;

		void AddVisitor(std::shared_ptr<Visitor> visitor)
		{
			visitor_ = std::move(visitor);
		}

		void AddElem(std::shared_ptr<Element>elem)
		{
			elements_.push_back(std::move(elem));
		}

		void Inicialize(std::shared_ptr<Element>elem, std::shared_ptr<Visitor> visitor)
		{
			for (size_t i = 0; i < 5; i++)
			{
				AddElem(elem);
			}
			AddVisitor(visitor);
		}

		void ShowElements() const
		{
			for (const auto& elem : elements_)
			{
				elem->Accept(visitor_);
			}
		}

	private:
		std::vector<std::shared_ptr<Element>> elements_;
		std::shared_ptr<Visitor> visitor_ = nullptr;
	};

}//end namespace


void TestVisitor()
{
	using namespace visitor;
	
	VectorElements list1;	
	list1.Inicialize(std::make_shared<ElementA>(), std::make_shared<VisitorText>());
	list1.ShowElements();

	VectorElements list2;		
	list2.Inicialize(std::make_shared<ElementB>(), std::make_shared<VisitorJson>());
	list2.ShowElements();
}