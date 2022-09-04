#pragma once

#include <memory>
#include <iostream>
#include <vector>


namespace document
{
	class Document;

	class Subscriber
	{
	public:
		virtual ~Subscriber() {}
		virtual void Update(const std::shared_ptr<Document>&) = 0;
	};

	class Document
	{
	public:
		explicit Document(int id_doc) : id_doc_(id_doc) {}

		void AddText(const std::string& str)
		{
			document_ = str;
			Notify();
		}

		void ChangeDocument(const std::string& str)
		{
			document_ += str;
			Notify();
		}

		std::string GetDocument() const
		{
			return document_;
		}

		int GetId() const
		{
			return id_doc_;
		}
		void AddSubscriber(std::shared_ptr<Subscriber> subscriber)
		{
			views_.push_back(subscriber);
		}

	private:
		std::vector<std::shared_ptr<Subscriber>>views_;
		std::string document_{};
		int id_doc_;

		void Notify()
		{
			for (auto& view : views_)
			{
				view->Update(std::make_shared<Document>(*this));
			}
		}
	};

	class Person
	{
	public:
		Person(const std::string& name)
		{
			name_ = name;
		}

		std::string GetName() const
		{
			return name_;
		}

		virtual void PrintMessage() const = 0;
	private:
		std::string name_{};
	};

	class Author : public Person, public Subscriber
	{
	public:
		Author(const std::string& name)
			: Person(name)
		{}
		void Update(const std::shared_ptr<Document>& doc) override
		{
			description_ = doc->GetDocument();
			PrintMessage();
		}

	private:
		std::string description_{};

		void PrintMessage() const override
		{
			std::cout << "Author: " << GetName() << " received changed document: \n"
				<< description_ << std::endl;
		}
	};

	class Analist : public Person, public Subscriber
	{
	public:
		Analist(const std::string& name)
			: Person(name)
		{}

		void Update(const std::shared_ptr<Document>& doc)
		{
			id_ = doc->GetId();
			PrintMessage();
		}


	private:
		int id_ = 0;

		void PrintMessage() const override
		{
			std::cout << "Analist: " << GetName() << " received data about document change from id: "
				<< id_ << std::endl;
		}
	};
}

namespace number
{
	class Observer
	{
	public:
		virtual ~Observer() {}
		virtual void Update(int) = 0;
	};

	class Subject
	{
	public:
		Subject() = default;
		void AttachObserver(std::shared_ptr<Observer> observer)
		{
			observers_.push_back(observer);
		}

		void SetValue(int n)
		{
			value_ = n;
			Notify();
		}

		void Notify()
		{
			for (auto& obs : observers_)
			{
				obs->Update(value_);
			}
		}

	private:
		std::vector<std::shared_ptr<Observer>> observers_;
		int value_ = 0;
	};



	class DivObserver : public Observer
	{
	public:
		DivObserver(std::shared_ptr<Subject> model, int div)
		{
			div_ = div;
			model->AttachObserver(std::make_shared<DivObserver>(*this));
		}

		void Update(int v) override
		{
			std::cout << v << " div " << div_ << " is " << v / div_ << std::endl;
		}

	private:
		int div_ = 0;
	};

	class ModObserver : public Observer
	{
	public:
		ModObserver(std::shared_ptr<Subject> model, int mod)
		{
			mod_ = mod;
			model->AttachObserver(std::make_shared<ModObserver>(*this));
		}

		void Update(int v) override
		{
			std::cout << v << " mod " << mod_ << " is " << v % mod_ << std::endl;
		}

	private:
		int mod_ = 0;
	};

}// end namespace



void TestObserver()
{
	{
		using namespace document;
		Document doc(1);
		std::shared_ptr<Author> autor = std::make_shared<Author>("Ivan");
		std::shared_ptr<Analist> analist = std::make_shared<Analist>("Petr");
		doc.AddSubscriber(autor);
		doc.AddSubscriber(analist);
		doc.AddText("Qwe asd xcv");
		doc.ChangeDocument("Hello world");
	}
	{
		using namespace number;
		std::shared_ptr<Subject> sub_ptr = std::make_shared<Subject>();
		DivObserver div_obs1(sub_ptr, 4);
		DivObserver div_obs2(sub_ptr, 3);
		ModObserver mod_obs(sub_ptr, 5);
		sub_ptr->SetValue(12);
	}	
}