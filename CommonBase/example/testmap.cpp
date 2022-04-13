#include "pch.h"
#include "Map.h"
#include "Random.h"
void output(CMap<int, int> &map)
{
	printf("begin output\n");
	CMap<int, int>::TreeNode *pNode = map.Begin();
	while (pNode != NULL)
	{
		printf("Key=%d Color=%d\n", pNode->m_Key, pNode->m_nColor);
		pNode = pNode->Next();
	}
	printf("end output\n");
}

void testmap_main()
{
	/*
	int keys[10] = { 8,17,1,11,15,25,6,22,27,4 };
	CMap<int, int> map1;
	for (int i = 0; i < sizeof(keys)/sizeof(int); i++)
	{
		int nKey = keys[i];
		map1.Set(nKey, nKey);
	}
	output(map1);
	*/
	/*
	CRandom::SRand();

	CMap<int, int> map1;

	int keys[10000] = {0};

	for (int i = 0; i < 100; i++)
	{
		int nKey = CRandom::Rand() % 10000;
		keys[i] = nKey;
		map1.Set(keys[i], keys[i]);
		int nHeight;
		bool nCheck = map1.Check(map1.m_pRoot, nHeight);
		if (nCheck == 0)
		{
			printf("add check success height=%d\r\n", nHeight);
		}
		else if (nCheck == -1)
		{
			printf("add error! root is red, height=%d\r\n", nHeight);
		}
		else if (nCheck == -2)
		{
			printf("add error! red is continue, height=%d\r\n", nHeight);
		}
		else if (nCheck == -3)
		{
			printf("add error! black not even, height=%d\r\n", nHeight);
		}
	}
	output(map1);
	
	for (int i = 0; i < 100; i++)
	{
		map1.Erase(keys[i]);
		int nHeight;
		int nCheck = 0; // map1.Check(map1.m_pRoot, nHeight);
		if (nCheck == 0)
		{
			printf("delete check success height=%d\r\n", nHeight);
		}
		else if(nCheck == -1)
		{
			printf("delete error! root is red, height=%d\r\n", nHeight);
		}
		else if (nCheck == -2)
		{
			printf("delete error! red is continue, height=%d\r\n", nHeight);
		}
		else if (nCheck == -3)
		{
			printf("delete error! black not even, height=%d\r\n", nHeight);
		}
	}*/
}