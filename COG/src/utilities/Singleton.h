#pragma once
/*!
	@file Singleton.h
	@brief Implementation for a Singleton design pattern.
	@author PrettyBinary
	@date 20/06/2016

	@class Pretty::Utilities::_Singleton
	@brief A class that implements the Singleton design pattern.

	@warning __NO DIRECT ACCESS__
	- This class should not be accessed directly. To create a singleton object, make this
	class a friend of your desired singleton class.

	A thread-safe singleton class that generates one instance of a template object on the heap.

	We use a Windows defined `CRITICAL_SECTION` for the following reasons:
	- A critical section object provides synchronization similar to that provided by a mutex object,
	except that a critical section can be used only by the threads of a single process.
	- Critical section objects cannot be shared across processes.
	- Event, mutex, and semaphore objects can also be used in a single-process application,
	but critical section objects provide a slightly faster, more efficient mechanism for mutual-exclusion synchronization
	(a processor-specific test and set instruction). Like a mutex object, a critical section object can be owned by only
	one thread at a time, which makes it useful for protecting a shared resource from simultaneous access.

	Using a critical section, we are able to generate a lock and unlock mechanism.
	This means that in the event of using multiple threads, if both threads try to access the `Instance()` function
	at the same time, only one instance will ever be created due to the lock and unlock procedures.

	Effectively, the first thread to reach the lock will create the instance. The other thread will 'wait'
	for the lock to be unlocked, and then as the instance has already been created, will just return that instance.

	We also implement double-checked locking and lazy instantiation. Once the instance has been created
	it makes no sense to generate the lock and unlock procedures every time the user calls the `Instance()` function.
	This is slightly costly and so by implementing double-checked locking we can just return the instance
	without any additional checks or function calls.

	We also use `unique_ptr` to eliminate memory leaks, as we create the singleton object on the heap.

	> __Usage__

	To create a singleton you must make this class a friend of your class, passing in your
	class type `T` to `_Singleton<T>`.
	All pre-defined singleton classes within the Pretty Engine are accessed via a typedef only, for ease of use.
	They are also prefixed with `_`, which reminds the user that they should not be accessed directly.
	If you are creating your own singleton object, it is recommended (but not necessary) you follow the same practice.

		class _Whatever
		{
			friend class Pretty::Utilities::_Singleton<_Whatever>;

			Some methods and variables...
		}
		typedef Pretty::Utilities::_Singleton<_Whatever> Whatever;

	Any singleton class `Instance()` method can now be accessed easily, by calling:

		Whatever::Instance()->SomeFunction();

	To access all pre-defined singleton classes within the Pretty Engine, you would prefix the above code with `Pretty::`

		Pretty::Whatever::Instance()->SomeFunction();

	## References
	- [Double-checked locking](https://www.codeproject.com/Articles/96942/Singleton-Design-Pattern-and-Thread-Safety)

*******************************************************************************************************************/

/*! @cond */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
/*! @endcond */

#include <windows.h>
#include <memory>
#include <iostream>
#include "utilities/NoCopyOrAssignment.h"

	/*! @brief Used for implementing lock/unlock procedures for thread-safety. */
class CriticalSection : public CRITICAL_SECTION, private NoCopyOrAssignment {

public:
	CriticalSection() { InitializeCriticalSection(this); }
	~CriticalSection() { DeleteCriticalSection(this); }
};


template <class T>
class Singleton : private NoCopyOrAssignment {

public:
	static T* Instance();

private:
	Singleton() = default;
	~Singleton() = default;

private:
	static CriticalSection s_singletonLock;			/*!< A static singleton lock, used for thread-safety. */
	static std::unique_ptr<T> s_singletonObject;	/*!< A static singleton object. */
};

template <class T> CriticalSection Singleton<T>::s_singletonLock;
template <class T> std::unique_ptr<T> Singleton<T>::s_singletonObject = nullptr;

/*!
@brief
	Providing no instance already exists and no other thread is within the critical section,
	this function creates a new singleton object.

@return
	The instance of the templated object <T>
*******************************************************************************************************************/
template <class T> T* Singleton<T>::Instance()
{
	/* Use double-checked locking and lazy instantiation */
	if (!s_singletonObject) {
		EnterCriticalSection(&s_singletonLock);
		if (!s_singletonObject) {

			/* Only one thread at a time can access this part of the function */
			s_singletonObject = std::unique_ptr<T>(new T());
		}
		LeaveCriticalSection(&s_singletonLock);
	}
	/* Upon re-visiting the function we will use the same heap memory and just return it */
	return s_singletonObject.get();
}
