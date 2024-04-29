#pragma once
#include "Allocator.h"


template<typename Type, typename...Args>
Type* xnew(Args&&... args)//��������
{
	Type* memory = static_cast<Type*>(xalloc(sizeof(Type)));

	//palacement new - ���ڰ� ��� ������� ���� �� ����.
	new(memory)Type(forward<Args>(args)...);

	return memory;
};

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();//�Ҹ��� ȣ��
	xrelease(obj);//�޸� ����
}