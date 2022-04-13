#include "pch.h"
#include "GlobalFunction.h"
#include "CommonBase.h"
class CMyClass
{
public:
	CMyClass()
	{
		m_nScore = 0;
		m_nRank = 0;
		printf("CMyClass construct\n");
	}
	~CMyClass()
	{
		printf("CMyClass destruct %d %d\n", m_nScore, m_nRank);
}
public:
	int m_nScore;
	int m_nRank;
};

void testjemalloc_main()
{
	CMyClass *pClass1 = (CMyClass*)malloc(sizeof(CMyClass));
	new (pClass1) CMyClass();
	pClass1->m_nScore = 100;
	pClass1->m_nRank = 1;
	pClass1->~CMyClass();
	free(pClass1);

	CMyClass *pClass2 = new CMyClass();
	pClass2->m_nScore = 98;
	pClass2->m_nRank = 2;
	delete pClass2;

	std::list<CMyClass*> list1;
	list1.push_back(pClass1);
	list1.push_back(pClass2);
}