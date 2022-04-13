/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  链表
**********************************************************************************/
#pragma once

namespace Minicat
{
	template<typename T>
	class CList
	{
	public:
		class ListNode
		{
			friend class CList;
		public:
			ListNode() : m_pPrev(nullptr), m_pNext(nullptr)
			{

			}
			ListNode(T& t) : m_t(t), m_pPrev(nullptr), m_pNext(nullptr)
			{

			}

			~ListNode()
			{
			}

			inline T& Val()
			{
				return m_t;
			}

			inline ListNode* Prev()
			{
				return m_pPrev;
			}

			inline ListNode* Next()
			{
				return m_pNext;
			}
		private:
			T m_t;
			ListNode *m_pPrev;
			ListNode *m_pNext;
		};
	public:
		CList() :m_pHead(nullptr), m_pTail(nullptr)
		{

		}

		CList(CList& other)
		{
			*this = other;
		}

		CList& operator=(CList& other)
		{
			ListNode *pNode = other.Begin();
			while (pNode != NULL)
			{
				Push(pNode->m_t);
				pNode = pNode->Next();
			}
			return *this;
		}

		~CList()
		{
			Clear();
		};

		inline ListNode* Begin()
		{
			return m_pHead;
		}

		inline bool Push(T &t)
		{
			ListNode *pNode = new ListNode();
			if (!pNode)
			{
				return false;
			}
			pNode->m_t = t;
			if (m_pTail)
			{
				m_pTail->m_pNext = pNode;
				pNode->m_pPrev = m_pTail;
				m_pTail = pNode;
			}
			else
			{
				m_pHead = pNode;
				m_pTail = pNode;
			}
			return true;
		}

		inline bool Pop(T &t)
		{
			if (m_pHead == nullptr)
			{
				return false;
			}
			t = m_pHead->m_t;
			ListNode *pNext = m_pHead->m_pNext;
			if (pNext)
			{
				pNext->m_pPrev = nullptr;
			}
			else
			{
				m_pTail = nullptr;
			}
			delete m_pHead;
			m_pHead = pNext;
			return true;
		}

		inline ListNode* Erase(ListNode *pNode)
		{
			if (pNode == m_pHead)
			{
				m_pHead = pNode->m_pNext;
			}
			if (pNode == m_pTail)
			{
				m_pTail = pNode->m_pPrev;
			}
			if (pNode->m_pPrev)
			{
				pNode->m_pPrev->m_pNext = pNode->m_pNext;
			}
			if (pNode->m_pNext)
			{
				pNode->m_pNext->m_pPrev = pNode->m_pPrev;
			}
			ListNode *pNext = pNode->m_pNext;
			delete pNode;
			return pNext;
		}

		inline void Clear()
		{
			ListNode *pCurr = Begin();
			while (pCurr != nullptr)
			{
				ListNode *pNext = pCurr->Next();
				delete pCurr;
				pCurr = pNext;
			}
			m_pHead = nullptr;
			m_pTail = nullptr;
		}

		inline bool Empty()
		{
			return (m_pHead == nullptr);
		}

	private:
		ListNode *m_pHead;
		ListNode *m_pTail;
	};
}


