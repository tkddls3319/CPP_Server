
#pragma once
#include "Allocator.h"

template<typename Type, typename... Args>
Type* xnew(Args&&... args)//��������
{
	//palacement new - ���ڰ� ��� ������� ���� �� ����.
	Type* memory = static_cast<Type*>(xxalloc(sizeof(Type)));
	new(memory)Type(forward<Args>(args)...); // placement new
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();//�Ҹ��� ȣ��
	xxrelease(obj);//�޸� ����
}