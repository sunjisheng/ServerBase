#include "pch.h"
#include "List.h"


void testlist_main()
{
	CList<int> list1;
	for (int i = 0; i < 10; i++)
	{
		list1.Push(i);
	}
	
	CList<int>::ListNode *pNode = list1.Begin();
	while (pNode)
	{
		printf("%d\n", pNode->Val());
		if (pNode->Val() % 2 == 0)
		{
			pNode = list1.Erase(pNode);
		}
		else
		{
			pNode = pNode->Next();
		}
	}

	pNode = list1.Begin();
	while (pNode)
	{
		printf("%d\n", pNode->Val());
		pNode = pNode->Next();
	}

	list1.Clear();
	printf("clear all\n");
	pNode = list1.Begin();
	while (pNode)
	{
		printf("%d\n", pNode->Val());
		pNode = pNode->Next();
	}
}