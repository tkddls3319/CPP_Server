#include "pch.h"
#include <iostream>
#include <thread>
#include <mutex>

class SpinLock {
public:
	void lock() 
	{
		bool expected = false;
		bool desired = true;

		while (_locked.compare_exchange_strong(expected, desired) == false)
		{
			expected = false;
			//this_thread::sleep_for(std::chrono::milliseconds(100));//언제까지 멈춰라
			this_thread::sleep_for(0ms);//입력한 시간동안 제 스케줄링되지않는다.
			//this_thread::yield();////언제든지 스케줄링될 수 있지만 지금은 필요하지않으니 반환하겠다.
		}
	}
	void unlock() 
	{
		_locked.store(false);
	}
private:
	atomic<bool> _locked = false;
};
int32 sum = 0;
mutex m;
SpinLock spinLock;
void Add() {
	for (int32 i = 0; i < 10'0000; i++) 
	{
		lock_guard<SpinLock> gard(spinLock);
		sum++;
	}
}
void Sub() {
	for (int32 i = 0; i < 10'0000; i++)
	{
		lock_guard<SpinLock> gard(spinLock);
		sum--;
	}
}
int main()
{
	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();
	cout << sum << endl;
}
