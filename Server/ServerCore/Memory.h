
#pragma once
#include "Allocator.h"

template<typename Type, typename... Args>
Type* xnew(Args&&... args)//보편참조
{
	//palacement new - 인자가 몇개든 상관없이 받을 수 있음.
	Type* memory = static_cast<Type*>(xxalloc(sizeof(Type)));
	new(memory)Type(forward<Args>(args)...); // placement new
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();//소멸자 호출
	xxrelease(obj);//메모리 해제
}