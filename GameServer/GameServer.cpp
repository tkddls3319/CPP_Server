#include "pch.h"
#include <iostream>
#include <thread>
#include <atomic>
//atomic : All-or-Nothing(전부실행 하거나 하나도 실행하지 않게) db의 트렌젝션 개념
atomic<int32> sum = 0;
void Add() {
	for (int32 i = 0; i < 100'0000; i++) 
	{
		sum.fetch_add(1);
	}
}
void Sub() {
	for (int32 i = 0; i < 100'0000; i++)
	{
		sum.fetch_add(-1);
	}
}
int main()
{
	Add();
	Sub();

	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();
	cout << sum << endl;
}
