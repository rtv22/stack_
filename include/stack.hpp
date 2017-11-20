#include <mutex>
#include <memory>
#include <iostream>

template <typename T>
class stack
{
public:
	stack();/*noexept*/
	stack(const stack<T> &);/*strong*/
	size_t count() const; /*noexcept*/
	void push(T const &); /*strong*/
	void swap(stack<T>&);
	std::shared_ptr<T> pop(); /*strong*/
	stack<T>& operator=(stack<T> &); /*noexcept*/
	~stack();/*noexcept*/
private:
	T * array_;
	mutable std::mutex mutex_;
	size_t array_size_;
	size_t count_;
};

template <typename T> 
stack<T>::stack() : count_(0), array_size_(0), array_{ nullptr }
{}

template<class T>
stack<T>::~stack()
{
	count_ = 0;
	array_size_ = 0;
	delete[] array_;
}

template <typename T> 
stack<T>::stack(const stack<T>& copy)
{
	std::lock_guard<std::mutex> lock(copy.mutex_);
	T *tmp = new T[copy.array_size_];
	count_ = copy.count_;
	array_size_ = copy.array_size_;
	array_ = tmp;
	try
	{
		std::copy(copy.array_, copy.array_ + count_, array_);
	}
	catch(...){
		delete[] array_;
	}
}

template<class T>
size_t stack<T>::count() const
{
	std::lock_guard<std::mutex> lock(mutex_);
	return count_;
}

template<typename T> 
void stack<T>::swap(stack& x) 
{
	std::lock(mutex_, x.mutex_);
	std::swap(x.array_size_, array_size_);
	std::swap(count_, x.count_);
	std::swap(x.array_, array_);
	x.mutex_.lock();
	x.mutex_.unlock();
}

template <typename T>
void stack<T>::push(T const &value)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (array_size_ == 0)
	{
		array_size_ = 1;
		array_ = new T[array_size_];
	}
	if (array_size_ == count_)
	{
		auto array_size = array_size_ * 2;
		T* new_array = new T[array_size]();
		try
		{
			std::copy(array_, array_ + count_, new_array);
		}
		catch(...)
		{
			delete new_array;
			throw "logic error";
		}
		array_size_ = array_size;
		if (array_)
			delete[] array_;
		array_ = new_array;
	}
	array_[count_] = value;
	++count_;
}

template <typename T>
auto stack<T>::pop() -> std::shared_ptr<T>
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (count_ == 0)
	{
		throw "logic error";
	}
	auto top = std::make_shared<T>(array_[count_ - 1]);;
	--count_;
	return top;
}

template<typename T>
stack<T>& stack<T>::operator=(stack<T> & other)
{
	std::lock_guard<std::mutex> lock(other.mutex_);
	if (this != &other)
	{
		stack<T> tmp(other);
		tmp.swap(*this);
	}
	return *this;
}