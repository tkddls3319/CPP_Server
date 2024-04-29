#pragma once
#include "Allocator.h"


template<typename Type, typename...Args>
Type* xnew(Args&&... args)//보편참조
{
	Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));

	//palacement new - 인자가 몇개든 상관없이 받을 수 있음.
	new(memory)Type(forward<Args>(args)...);

	return memory;
};

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();//소멸자 호출
	xrelease(obj);//메모리 해제
}