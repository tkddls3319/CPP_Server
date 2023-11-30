#pragma once

#include<mutex>
#include<atomic>
template<typename T>
class LockStack
{
public:
	LockStack() {	}
	LockStack(const LockStack&) = delete;//복사연산 금지
	LockStack& operator=(const LockStack&) = delete;//복사대입연산 금지

	void Push(T value) {
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));//값을 복사하는게 아닌 이동을 통해 성능향상.
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;

		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}
	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _stack.empty() == false; });
		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}
private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(value), next(nullptr)
		{

		}
	public:
		T data;
		Node* next;
	};
public:
	//1) 새로운 노드를 만든다.
	//2) 새로운 노드 = head 
	//3) head = 새로운 노드
	void Push(const T& value) {
		Node* node = new Node(value);
		node->next = _head;

		//풀어쓴 compare_exchang_week
		//if (_head == node->next)
		//{
		//	_head = node;
		//	return true;
		//}
		//else {
		//	node->next = _head;
		//	return false;
		//}
		while (_head.compare_exchange_weak(node->next, node) == false)
		{
			//node->next = _head;
		}
		//_head = node;
	}
	//1) head 읽기
	//2) head->next 읽기
	//3) head = head-> next
	//4) data 추출해서 반환
	//5) 추출한 노드 삭제
	bool TryPop(T& value)
	{
		++_popCount;
		Node* oldHead = _head;
		/*풀어쓴 compare_exchang_weak
		if (_head == oldhead) {
			_head = oldHead->next
		}
		else {
			oldHead->next = oldHead;
		}*/
		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{
		}

		if (oldHead == nullptr)
		{
			--_popCount;
			return false;
		}

		value = oldHead->data;
		TryDelete(oldHead);
		//delete oldHead;
		return true;
	}
	void TryDelete(Node* oldHead) {
		//실행중인 쓰레드가 하나인지 확인
		if (_popCount == 1)
		{
			Node* node = _pendingList.exchange(nullptr);

			if (--_popCount == 0)
			{
				DeleteNodes(node);;
			}
			else if(node)
			{
				ChainPendingNodeList(node);
			}
			delete oldHead;
		}
		else 
		{
			ChainPendingNodeList(oldHead);
			_popCount;
		}
	}
	void ChainPendingNodeList(Node* first, Node* last) 
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{
		}
	}
	void ChainPendingNodeList(Node* node) 
	{
		Node* last = node;
		while (last->next)
			last = last->next;

		ChainPendingNodeList(node, last);
	}
	void ChainPendingNode(Node* node) {
		ChainPendingNodeList(node, node);
	}
	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next; 
			delete node;
			node = next;
		}
	}
private:
	atomic<Node*> _head;

	atomic<uint32> _popCount = 0;//pop을 실행중인 쓰레드 개수
	atomic<Node*> _pendingList; // 삭제되어야할 노드들 (첫번쨰 노드)
};