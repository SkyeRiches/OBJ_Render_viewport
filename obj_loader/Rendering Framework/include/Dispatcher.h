#pragma once

#include "Event.h"
#include <map>
#include <list>
#include <functional>
#include <typeinfo>
#include <typeindex>

class Observer
{
public:
	Observer() = default;
	virtual ~Observer() = default;
	// copy move and assignment operators have been disabled for this class
	Observer(const Observer&) = delete;
	Observer(const Observer&&) = delete;
	Observer& operator=(const Observer&) = delete;

	// function to perform a call to the abstract call function to post the event to the observer
	void exec(Event* e)
	{
		call(e);
	}
	
	virtual void* Instance() = 0;

private:
	virtual void call(Event* e) = 0;
};

// Template class member observer an observer that is a class with a desginated observation member function
// typename T is a pointer to the class owning the function to be called
// typename ConcreteEvent is the type of event that is to be observed
template<typename T, typename ConcreteEvent>
class MemberObserver : public Observer
{
public:
	//type define for the pointer to the class member function
	typedef void (T::* MemberFuntion)(ConcreteEvent*);
	
	MemberObserver(T* a_instance, MemberFuntion a_function) : m_instance(a_instance), m_memberFunction(a_function) {};
	~MemberObserver() { m_instance = nullptr; }

	// function to return pointer to the instance of the calss the member function belongs to
	void* Instance() { return (void*)m_instance; }

private:
	void call(Event* e)
	{
		(m_instance->*m_memberFunction)(static_cast<ConcreteEvent*>(e));
	}

private:
	MemberFuntion m_memberFunction;
	T* m_instance;
};

//Template class GlobalObserver, template argument concrete event is deduced by the compiler
template<typename ConcreteEvent>
class GlobalObserver : public Observer
{
public:
	// typedef of function pointer to non member function that takes a concrete event as a parameter
	typedef void (*Function)(ConcreteEvent*);
	GlobalObserver(Function a_function) : m_function(a_function) {}
	~GlobalObserver() {}
	//instance function implementation, global functions have no instance so returnd nullptr
	void* Instance() { return nullptr; }

private:
	void call(Event* e)
	{
		(*m_function)(static_cast<ConcreteEvent*>(e));
	}

private:
	Function m_function;
};

//typedefine for std::list<Observer*> objects to improve code readability
typedef std::list<Observer*> ObserverList;
// Dispatcher class, responsible for handling events and notifying any observers of a particular event
class Dispatcher
{
public:
	static Dispatcher* GetInstance() { return m_instance; }
	static Dispatcher* CreateInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = new Dispatcher();
		}
		return m_instance;
	}
	static void DestroyInstance()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance = nullptr;
		}
	}

	template<typename T, typename ConcreteEvent>
	void subscribe(T* a_instance, void(T::* memberfunction)(ConcreteEvent*))
	{
		// get list of observers from the subscribers map
		ObserverList* observers = m_subscribers[typeid(ConcreteEvent)];
		if (observers == nullptr)
		{
			observers = new ObserverList();
			m_subscribers[typeid(ConcreteEvent)] = observers;
		}
		observers->push_back(new MemberObserver<T, ConcreteEvent>(a_instance, memberfunction));
	}

	// subscribe method for global functions to become event subsribers
	template<typename ConcreteEvent>
	void subscribe(void(*Function)(ConcreteEvent*))
	{
		// get list of observers from the subscribers map
		ObserverList* observers = m_subscribers[typeid(ConcreteEvent)];
		if (observers == nullptr)
		{
			observers = new ObserverList();
			m_subscribers[typeid(ConcreteEvent)] = observers;
		}
		observers->push_back(new GlobalObserver<ConcreteEvent>(Function));
	}

	// FUnction to publish an event that has occured and notrify all subscribers
	template <typename ConcreteEvent>
	void publish(ConcreteEvent* e, bool cleanup = false)
	{
		ObserverList* observers = m_subscribers[typeid(ConcreteEvent)];
		if (observers == nullptr) { return; }
		for (auto& handler : *observers)
		{
			handler->exec(e);
			// if an event has been handled by a subscriber, then we do not need to keep notifying other subscribers
			if (static_cast<Event*>(e)->IsHandled())
			{
				break;
			}
			// as we could pass through "new ConcreteEvent()" we should call delete if needed
			if (cleanup) { delete e; }
		}
	}

protected:
	Dispatcher() {};
	~Dispatcher()
	{
		for (auto it = m_subscribers.begin(); it != m_subscribers.end(); it++)
		{
			ObserverList* obs = it->second;
			for (auto o = obs->begin(); o != obs->end(); o++)
			{
				delete (*o);
				(*o) = nullptr;
			}
			delete obs;
		}
	}

private:
	std::map<std::type_index, ObserverList*> m_subscribers;
	static Dispatcher* m_instance;
};