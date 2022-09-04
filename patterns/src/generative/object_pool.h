#pragma once
#include <iostream>
#include <string>
#include <cassert>
#include <list>

namespace object_pool
{
	template<class Type>
	class ObjectPool;

	template<class Type>
	class ObjectPoolDestroyer
	{
	private:
		ObjectPool<Type>* p_instance_;

	public:
		~ObjectPoolDestroyer()
		{
			delete p_instance_;
		}
		void Initiliaze(ObjectPool<Type>* p_instance)
		{
			p_instance_ = p_instance;
		}
	};

	template<class Type>
	class ObjectPool
	{
	private:
		static ObjectPool<Type>* instance_pool_;
		static ObjectPoolDestroyer<Type> destroyer_;
		std::list<Type> objects_;
		size_t max_count_objects_ = 5;
	protected:
		ObjectPool() = default;
		~ObjectPool() = default;
		friend class ObjectPoolDestroyer<Type>;
	public:

		static ObjectPool<Type>* GetInstance()
		{
			if (!instance_pool_)
			{
				instance_pool_ = new ObjectPool<Type>();
			}
			return instance_pool_;
		}

		void AddObject(const Type& obj)
		{
			if (objects_.size() < max_count_objects_)
			{
				objects_.push_back(obj);
			}
			else
			{
				throw std::logic_error("Object pool is full");
			}
		}

		template<class Iterator>
		Type GetObject(Iterator it)
		{
			if (it != End())
			{
				Type temp = *it;
				objects_.erase(it);
				return temp;
			}
			return {};
		}

		std::list<Type> GetObjectPool() const
		{
			return objects_;
		}

		auto Begin() 
		{
			return objects_.begin();
		}

		auto End()
		{
			return objects_.end();
		}
	};

	template<class Type>
	ObjectPool<Type>* ObjectPool<Type>::instance_pool_ = nullptr;

	template<class Type>
	ObjectPoolDestroyer<Type> ObjectPool<Type>::destroyer_;

} // end namespace

void TestObjectPool()
{
	using namespace object_pool;
	ObjectPool<std::string>* pool_ptr{ ObjectPool<std::string>::GetInstance() };
	pool_ptr->AddObject("qwe");
	pool_ptr->AddObject("asd");
	pool_ptr->AddObject("qwe1");
	pool_ptr->AddObject("asd1");
	pool_ptr->AddObject("qwe2");	
	try
	{
		pool_ptr->AddObject("asd2");		
	}
	catch (std::logic_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	std::string s = pool_ptr->GetObject(pool_ptr->Begin());
	assert("qwe" == s);
	assert(4 == pool_ptr->GetObjectPool().size());
	std::string s1 = pool_ptr->GetObject(std::find(pool_ptr->Begin(), pool_ptr->End(), "asd"));
	assert("asd" == s1);
	assert(3 == pool_ptr->GetObjectPool().size());

	std::cout << "Test ObjectPool is Ok" << std::endl;
}