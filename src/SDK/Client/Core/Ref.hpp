#pragma once

#include <memory>
#include <type_traits>

// LeviLamina

template <typename T>
class SharedCounter {
public:
	explicit SharedCounter(T* p = nullptr) : ptr(p), share_count(1), weak_count(0) {}

	void addShareCount() { share_count++; }

	void addWeakCount() { weak_count++; }

	int getShareCount() const { return share_count.load(); }

	int getWeakCount() const { return weak_count.load(); }

	T* get() const { return ptr; }

	void release() {
		if (--share_count == 0) {
			delete ptr;
			ptr = nullptr;
			if (weak_count == 0) {
				delete this;
			}
		}
	}

	void releaseWeak() {
		if (--weak_count == 0 && share_count == 0) {
			delete this;
		}
	}

	T* ptr;
	std::atomic_int share_count;
	std::atomic_int weak_count;
};

template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
public:
	template <typename... Args>
	static SharedPtr<T> makeShared(Args&&... args) {
		return SharedPtr<T>(new T(std::forward<Args>(args)...));
	}

	SharedPtr() noexcept : counter(nullptr) {}                // NOLINT
	SharedPtr(std::nullptr_t) noexcept : counter(nullptr) {}  // NOLINT

	explicit SharedPtr(T* p) : counter(new SharedCounter<T>(p)) {}

	template <class Y>
	explicit SharedPtr(SharedPtr<Y> const& other)
		requires(std::convertible_to<Y*, T*>)
	{
		counter = (SharedCounter<T>*)other.counter;
		if (counter) {
			counter->addShareCount();
		}
	}

	template <class Y>
	explicit SharedPtr(SharedPtr<Y>&& other)
		requires(std::convertible_to<Y*, T*>)
	{
		counter = (SharedCounter<T>*)other.counter;
		other.counter = nullptr;
	}

	template <class Y>
	explicit SharedPtr(WeakPtr<Y> const& other)
		requires(std::convertible_to<Y*, T*>)
	{
		counter = (SharedCounter<T>*)other.counter;
		if (other) {
			counter->addShareCount();
		}
	}

	~SharedPtr() {
		if (counter) {
			counter->release();
		}
	}

	template <class Y>
	SharedPtr<T>& operator=(SharedPtr<Y> const& other)
		requires(std::convertible_to<Y*, T*>)
	{
		if (counter != (SharedCounter<T>*)other.counter) {
			counter = (SharedCounter<T>*)other.counter;
			if (counter) {
				counter->addShareCount();
			}
		}
		return *this;
	}

	template <class Y>
	SharedPtr<T>& operator=(SharedPtr<Y>&& other)
		requires(std::convertible_to<Y*, T*>)
	{
		if (counter != (SharedCounter<T>*)other.counter) {
			counter = (SharedCounter<T>*)other.counter;
			other.counter = nullptr;
		}
		return *this;
	}

	template <class Y>
	SharedPtr<T>& operator=(WeakPtr<Y> const& other)
		requires(std::convertible_to<Y*, T*>)
	{
		counter = (SharedCounter<T>*)other.counter;
		if (other) {
			counter->addShareCount();
		}
		return *this;
	}

	T* get() const { return counter ? counter->get() : nullptr; }

	T* operator->() const { return get(); }

	T& operator*() const { return *get(); }

	explicit operator bool() const { return get() != nullptr; }

	[[nodiscard]] int use_count() const { return counter ? counter->getShareCount() : 0; }

	void reset() {
		if (counter) {
			counter->release();
			counter = nullptr;
		}
	}

	SharedCounter<T>* counter;
};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr {
public:
	WeakPtr() noexcept : counter(nullptr) {}                // NOLINT
	WeakPtr(std::nullptr_t) noexcept : counter(nullptr) {}  // NOLINT

	template <class Y>
	explicit WeakPtr(SharedPtr<Y> const& other)
		requires(std::convertible_to<Y*, T*>)
	{
		counter = (SharedCounter<T>*)other.counter;
		if (counter) {
			counter->addWeakCount();
		}
	}

	template <class Y>
	explicit WeakPtr(WeakPtr<Y> const& other)
		requires(std::convertible_to<Y*, T*>)
	{
		counter = (SharedCounter<T>*)other.counter;
		if (counter) {
			counter->addWeakCount();
		}
	}

	template <class Y>
	explicit WeakPtr(WeakPtr<Y>&& other)
		requires(std::convertible_to<Y*, T*>)
	{
		counter = (SharedCounter<T>*)other.counter;
		other.counter = nullptr;
	}

	~WeakPtr() {
		if (counter) {
			counter->releaseWeak();
		}
	}

	template <class Y>
	WeakPtr<T>& operator=(SharedPtr<Y> const& other)
		requires(std::convertible_to<Y*, T*>)
	{
		if (counter != (SharedCounter<T>*)other.counter) {
			counter = (SharedCounter<T>*)other.counter;
			if (counter) {
				counter->addWeakCount();
			}
		}
		return *this;
	}

	template <class Y>
	WeakPtr<T>& operator=(WeakPtr<Y> const& other)
		requires(std::convertible_to<Y*, T*>)
	{
		if (counter != (SharedCounter<T>*)other.counter) {
			counter = (SharedCounter<T>*)other.counter;
			if (counter) {
				counter->addWeakCount();
			}
		}
		return *this;
	}

	template <class Y>
	WeakPtr<T>& operator=(WeakPtr<Y>&& other)
		requires(std::convertible_to<Y*, T*>)
	{
		if (counter != (SharedCounter<T>*)other.counter) {
			counter = (SharedCounter<T>*)other.counter;
			other.counter = nullptr;
		}
		return *this;
	}

	[[nodiscard]] int use_count() const { return counter ? counter->getShareCount() : 0; }

	[[nodiscard]] bool expired() const { return use_count() == 0; }

	SharedPtr<T> lock() const { return expired() ? SharedPtr<T>() : SharedPtr<T>(*this); }

	T* get() const { return counter ? counter->get() : nullptr; }

	T* operator->() const { return get(); }

	T& operator*() const { return *get(); }

	explicit operator bool() const { return get() != nullptr; }

	SharedCounter<T>* counter;
};