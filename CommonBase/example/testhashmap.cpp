#include "pch.h"
#include "HashMap.h"

void output(CHashMap<int, int> &map)
{
	CHashMap<int, int>::HashNode *pNode = map.Begin();
	while (pNode != NULL)
	{
		printf("%d=%d\n", pNode->m_Key, pNode->m_Value);
		pNode = pNode->Next();
	}
}

void testhashmap_main()
{
	CHashMap<uint64, uint64> map1;
	uint64 a;
	map1.Set(72418732353781766, 72418732353781766);

	map1.Get(72418732353781766, a);
	map1.Get(72418732353781766, a);
	map1.Get(72418732353781766, a);

	map1.Set(72418732353781765, 72418732353781765);
	map1.Get(72418732353781765, a);
	map1.Get(72418732353781765, a);
	map1.Get(72418732353781765, a);

	map1.Set(72418732353781764, 72418732353781764);
	map1.Get(72418732353781764, a);
	map1.Get(72418732353781764, a);
	map1.Get(72418732353781764, a);
	map1.Set(72418732353781763, 72418732353781763);
	map1.Get(72418732353781763, a);
	map1.Get(72418732353781763, a);
	map1.Get(72418732353781763, a);
	map1.Set(72418732353781762, 72418732353781762);
	map1.Get(72418732353781762, a);
	map1.Get(72418732353781762, a);
	map1.Get(72418732353781762, a);
	map1.Set(72418732353781761, 72418732353781761);
	map1.Get(72418732353781761, a);
	map1.Get(72418732353781761, a);
	map1.Get(72418732353781761, a);

	for (int i = 0; i < 5; i++)
	{
		CHashMap<uint64, uint64>::HashNode *pNode = map1.Begin();
		while (pNode != NULL)
		{
			CHashMap<uint64, uint64>::HashNode *pNext = pNode->Next();
			printf("%lld=%lld\r\n", pNode->m_Key, pNode->m_Value);
			pNode = pNext;
		}
	}
}