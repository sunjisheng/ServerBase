#include "pch.h"
#include "Vector.h"

void testvector_main()
{
	CVector<int> ve1;
	for (int i = 0; i < 20; i++)
	{
		ve1.Push(i);
	}

	for (int i = 0; i < ve1.Size(); i++)
	{
		printf("%d=%d\n", i, ve1[i]);
	}

	for (int i = 0; i < 10; i++)
	{
		int value;
		ve1.Pop(value);
	}

	for (int i = 0; i < ve1.Size(); i++)
	{
		int value = i + 100;
		ve1.Set(i, value);
	}

	for (int i = 0; i < ve1.Size(); i++)
	{
		printf("%d=%d\n", i, ve1[i]);
	}
}