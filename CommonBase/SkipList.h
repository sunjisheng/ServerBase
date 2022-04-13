#pragma once
#include "Random.h"
namespace Minicat
{
	template<typename Key, typename Value>
	class CSkipList
	{
		enum
		{
			Max_Level = 32,
		};
	public:
		class ListNode
		{
			friend class CSkipList;
		public:
			inline ListNode* Next()
			{
				return m_pNexts[0];
			}
		public:
			Key m_Key;
			Value m_Value;
		private:
			ListNode *m_pNexts[1];
		};
	public:
		CSkipList()
		{
			m_nLevel = 0;
			m_pHead = CreateNode(Max_Level, 0, Value());
			for (int i = 0; i < Max_Level; i++)
			{
				m_pHead->m_pNexts[i] = NULL;
			}
		}

		CSkipList(CSkipList& other)
		{
			*this = other;
		}

		CSkipList& operator=(CSkipList& other)
		{
			ListNode *pNode = other.Begin();
			while (pNode != NULL)
			{
				Push(pNode->m_t);
				pNode = pNode->Next();
			}
			return *this;
		}

		~CSkipList()
		{
			ListNode *pNode = m_pHead;
			while (pNode != NULL)
			{
				ListNode *pNext = pNode->m_pNexts[0];;
				free(pNode);
				pNode = pNext;
			}
		}

		ListNode* Begin()
		{
			return m_pHead->m_pNexts[0];
		}

		void Clear()
		{
			ListNode *pNode = Begin();
			while (pNode != NULL)
			{
				ListNode *pNext = pNode->Next();
				free(pNode);
				pNode = pNext;
			}
			m_nLevel = 0;
			for (int i = 0; i < Max_Level; i++)
			{
				m_pHead->m_pNexts[i] = NULL;
			}
		}

		void Set(Key key, Value value)
		{
			ListNode *arrUpdates[Max_Level] = {0};
			ListNode *pNode = m_pHead;
			ListNode *pNext = NULL;
			for (int i = m_nLevel - 1; i >= 0; i--)
			{
				while ((pNext = pNode->m_pNexts[i]) && (pNext->m_Key < key))
				{
					pNode = pNext;
				}
				arrUpdates[i] = pNode;
			}
			if (pNext && pNext->m_Key == key)
			{
				pNext->m_Value = value;
				return;
			}
			//³¬¹ýµ±Ç°Level
			int nLevel = RandomLevel();
			if (nLevel > m_nLevel)
			{
				arrUpdates[nLevel - 1] = m_pHead;
				m_nLevel = nLevel;
			}

			pNode = CreateNode(nLevel, key, value);
			for (int i = nLevel - 1; i >= 0; i--)
			{
				pNode->m_pNexts[i] = arrUpdates[i]->m_pNexts[i];
				arrUpdates[i]->m_pNexts[i] = pNode;
			}
		}

		bool Erase(Key key)
		{
			ListNode *arrUpdates[Max_Level] = { 0 };
			ListNode *pNode = m_pHead;
			ListNode *pNext = NULL;
			for (int i = m_nLevel - 1; i >= 0; i--)
			{
				while ((pNext = pNode->m_pNexts[i]) && (pNext->m_Key < key))
				{
					pNode = pNext;
				}
				arrUpdates[i] = pNode;
			}

			if (pNext->m_Key != key)
			{
				return false;
			}

			for (int i = m_nLevel - 1; i >= 0; i--)
			{
				if (arrUpdates[i]->m_pNexts[i] == pNext)
				{
					arrUpdates[i]->m_pNexts[i] = pNext->m_pNexts[i];
				}
			}

			free(pNext);

			for (int i = m_nLevel - 1; i >= 0; i--)
			{
				if (m_pHead->m_pNexts[i] == NULL) 
				{
					m_nLevel--;
				}
			}
			return true;
		}

		bool Get(Key key, Value &value)
		{
			ListNode *pNode = m_pHead;
			ListNode *pNext = NULL;

			for (int i = m_nLevel - 1; i >= 0; i--)
			{
				while ((pNext = pNode->m_pNexts[i]) && (pNext->m_Key <= key))
				{
					if (pNext->m_Key == key)
					{
						value = pNext->m_Value;
						return true;
					}
					pNode = pNext;
				}
			}
			return false;
		}

	private:
		ListNode *CreateNode(int nLevel, Key key, Value value)
		{
			ListNode *pNode = (ListNode*)malloc(sizeof(ListNode) + nLevel * sizeof(ListNode*));
			pNode->m_Key = key;
			pNode->m_Value = value;
			for (int i = 0; i < nLevel; i++)
			{
				pNode->m_pNexts[i] = NULL;
			}
			return pNode;
		}

		int RandomLevel()
		{
			int n = 1;
			while (CRandom::Rand(2))
			{
				n++;
			}
			if (n > Max_Level)
			{
				n = Max_Level;
			}
			else if (n > m_nLevel + 1)
			{
				n = m_nLevel + 1;
			}
			return n;
		}
	private:
		ListNode *m_pHead;
		int m_nLevel;
	};
}