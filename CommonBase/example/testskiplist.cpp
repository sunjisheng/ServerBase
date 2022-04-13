#include "pch.h"
#include "SkipList.h"

void output(CSkipList<int, int> &list)
{
	CSkipList<int, int>::ListNode *pNode = list.Begin();
	while (pNode != NULL)
	{
		printf("%d=%d\n", pNode->m_Key, pNode->m_Value);
		pNode = pNode->Next();
	}
}

void testskiplist_main()
{
	CSkipList<int, int> map1;

	for (int i = 0; i < 30; i++)
	{
		map1.Set(i, i);
	}

	output(map1);

	for (int i = 0; i < 30; i++)
	{
		int v;
		if (map1.Get(i,v))
		{
			printf("get(%d)=%d\n", i, v);
		}
	}

	CSkipList<int, int>::ListNode *pNode = map1.Begin();
	while (pNode != NULL)
	{
		CSkipList<int, int>::ListNode *pNext = pNode->Next();
		if (pNode->m_Key % 2 == 0)
		{
			map1.Erase(pNode->m_Key);
		}
		pNode = pNext;
	}

	output(map1);
}