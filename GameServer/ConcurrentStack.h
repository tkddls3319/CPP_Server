#pragma once

#include<mutex>
#include<atomic>
//template<typename T>
//class LockStack
//{
//public:
//	LockStack() {	}
//	LockStack(const LockStack&) = delete;//복사연산 금지
//	LockStack& operator=(const LockStack&) = delete;//복사대입연산 금지
//
//	void Push(T value) {
//		lock_guard<mutex> lock(_mutex);
//		_stack.push(std::move(value));//값을 복사하는게 아닌 이동을 통해 성능향상.
//		_condVar.notify_one();
//	}
//
//	bool TryPop(T& value)
//	{
//		lock_guard<mutex> lock(_mutex);
//		if (_stack.empty())
//			return false;
//
//		value = std::move(_stack.top());
//		_stack.pop();
//		return true;
//	}
//	void WaitPop(T& value) {
//		unique_lock<mutex> lock(_mutex);
//		_condVar.wait(lock, [this] {return _stack.empty() == false; });
//		value = std::move(_stack.top());
//		_stack.pop();
//		return true;
//	}
//private:
//	stack<T> _stack;
//	mutex _mutex;
//	condition_variable _condVar;
//};
//
//template<typename T>
//class LockFreeStack
//{
//	struct Node
//	{
//		Node(const T& value) : data(make_shared<T>(value)), next(nullptr)
//		{
//
//		}
//	public:
//		shared_ptr<T> data;
//		shared_ptr<Node> next;
//	};
//public:
//	void Push(const T& value) {
//		shared_ptr<Node> node = make_shared<Node>(value);
//			node->next = _head;
//
//
//		while (std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
//		{
//		}
//	}
//	shared_ptr<T> TryPop()
//	{
//		shared_ptr<Node> oldHead = std::atomic_load(&_head);
//
//		while (oldHead && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
//		{
//
//		}if (oldHead == nullptr)
//			return shared_ptr<T>();
//
//		return oldHead->data;
//	}
//private:
//	shared_ptr<Node> _head;
//};

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(make_shared<T>(value)), next(nullptr)
		{

		}
	public:
		shared_ptr<T> data;
		shared_ptr<Node> next;
	};
public:
	void Push(const T& value) {
		shared_ptr<Node> node = make_shared<Node>(value);
		node->next = _head;


		while (std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
		{
		}
	}
	shared_ptr<T> TryPop()
	{
		shared_ptr<Node> oldHead = std::atomic_load(&_head);

		while (oldHead && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
		{

		}if (oldHead == nullptr)
			return shared_ptr<T>();

		return oldHead->data;
	}
private:
	shared_ptr<Node> _head;
};