/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  哈希表
**********************************************************************************/

#pragma once

namespace Minicat
{
	template<typename Key, typename Value>
	class CHashMap
	{
		enum 
		{
			Default_Group_Count = 1024,
		};
	public:
		struct HashFunc
		{
			HashFunc() {}

			inline unsigned int	operator()(int data)	const { return data; }
			inline unsigned int	operator()(long data)	const { return data; }
			inline unsigned int	operator()(unsigned int data)	const { return data; }
			inline unsigned long long operator()(long long data)	const { return data; }
			inline unsigned long long operator()(unsigned long long data)	const { return data; }
			inline unsigned long operator()(void *data)	const
			{
				return ((unsigned long)data) >> 4;
			}
			inline unsigned int	operator()(char *s)		const
			{
				return	operator()((const char *)s);
			}
			inline unsigned int	operator()(const char *s)	const
			{
				unsigned int h = 0;
				for (; *s; s++)
				{
					h = h * 31 + *(unsigned char *)s;
				}
				return h;
			}
		};

		class CChain;
		class HashNode
		{
			friend class CChain;
			friend class CHashMap;
		public:
			HashNode(CChain *pParent) :m_Key(0), m_pNext(nullptr), m_pParent(pParent)
			{
			}
			~HashNode()
			{
			}

			inline HashNode* Next()
			{
				if (m_pNext)
				{
					return m_pNext;
				}
				else
				{
					CChain *pNextChain = m_pParent->m_pNext;
					if (pNextChain)
					{
						return pNextChain->m_pHead;
					}
					else
					{
						return nullptr;
					}
				}
			}
		public:
			Key	m_Key;
			Value m_Value;
		private:
			HashNode *m_pNext;
			CChain *m_pParent;
		};

		class CChain
		{
			friend class CHashMap;
		public:
			CChain() : m_pHead(nullptr), m_pPrev(nullptr), m_pNext(nullptr)
			{
			}

			~CChain() 
			{
				FreeAll();
			}

			inline void FreeAll()
			{
				while (m_pHead)
				{
					HashNode *pNext = m_pHead->m_pNext;
					delete m_pHead;
					m_pHead = pNext;
				}
				m_pHead = nullptr;
			}

			inline bool Exist(const Key &key)
			{
				for (HashNode *pCurr = m_pHead; pCurr; pCurr = pCurr->m_pNext)
				{
					if (pCurr->m_Key == key)
					{
						return true;
					}
				}
				return false;
			}

			inline bool Get(const Key &key, HashNode* &pNode)
			{
				for (HashNode *pCurr = m_pHead; pCurr; pCurr = pCurr->m_pNext)
				{
					if (pCurr->m_Key == key)
					{
						pNode = pCurr;
						return true;
					}
				}
				return false;
			}

			inline void Insert(HashNode* pNode)
			{
				pNode->m_pNext = m_pHead;
				m_pHead = pNode;
			}

			inline bool Erase(const Key &key)
			{
				HashNode *pPrev = nullptr;
				for (HashNode *pCurr = m_pHead; pCurr; pPrev = pCurr, pCurr = pCurr->m_pNext)
				{
					if (pCurr->m_Key == key)
					{
						if (pCurr == m_pHead)
						{
							m_pHead = pCurr->m_pNext;
						}
						else
						{
							pPrev->m_pNext = pCurr->m_pNext;
						}
						delete pCurr;
						return true;
					}
				}
				return false;
			}

			inline bool Erase(HashNode *pNode)
			{
				HashNode *pPrev = nullptr;
				for (HashNode *pCurr = m_pHead; pCurr; pPrev = pCurr, pCurr = pCurr->m_pNext)
				{
					if (pCurr == pNode)
					{
						if (pCurr == m_pHead)
						{
							m_pHead = pCurr->m_pNext;
						}
						else
						{
							pPrev->m_pNext = pCurr->m_pNext;
						}
						delete pCurr;
						return true;
					}
				}
				return false;
			}

			inline bool IsEmpty()
			{
				return (m_pHead == nullptr);
			}

		private:
			HashNode *m_pHead;
			CChain *m_pPrev;
			CChain *m_pNext;
		};

	public:
		CHashMap():m_pTravelHead(nullptr)
		{
			InitGroup(Default_Group_Count);
		}

		CHashMap(int nGroupCount):m_pTravelHead(nullptr)
		{
			InitGroup(nGroupCount);
		}

		CHashMap(CHashMap &other)
		{
			*this = other;
		}

		CHashMap& operator=(CHashMap &other)
		{
			HashNode *pNode = other.Begin();
			while (pNode != NULL)
			{
				Set(pNode->m_Key, pNode->m_Value);
				pNode = pNode->Next();
			}
			return *this;
		}

		~CHashMap()
		{
			SAFE_DELETE_ARRAY(m_arrChains)
		}

		bool Exist(const Key &key)
		{
			int nGroup = (m_fnHash(key) % m_nGroupCount);
			return m_arrChains[nGroup].Exist(key);
		}

		bool Get(const Key key, Value &value)
		{
			int nPos = (m_fnHash(key) % m_nGroupCount);
			HashNode *pNode = nullptr;
			if (m_arrChains[nPos].Get(key, pNode))
			{
				value = pNode->m_Value;
				return true;
			}
			return false;
		}

		bool Set(Key key, Value value)
		{
			int nPos = ((m_fnHash(key)) % m_nGroupCount);
			CChain *pChain = &m_arrChains[nPos];
			HashNode *pNode = nullptr;
			if (pChain->Get(key, pNode))
			{
				pNode->m_Value = value;
				return true;
			}
			else
			{
				HashNode *pNode = new HashNode(pChain);
				if (!pNode)
				{
					return false;
				}
				pNode->m_Key = key;
				pNode->m_Value = value;
				if (pChain->IsEmpty())
				{
					InsertTravel(pChain);
				}
				pChain->Insert(pNode);
				return true;
			}
		}

		inline void Erase(Key key)
		{
			int nPos = (m_fnHash(key) % m_nGroupCount);
			CChain *pChain = &m_arrChains[nPos];
			pChain->Erase(key);
			if (pChain->IsEmpty())
			{
				RemoveTravel(pChain);
			}
			return;
		}

		inline HashNode* Erase(HashNode *pNode)
		{
			HashNode *pNext = pNode->Next();
			CChain *pChain = pNode->m_pParent;
			pChain->Erase(pNode);
			if (pChain->IsEmpty())
			{
				RemoveTravel(pChain);
			}
			return pNext;
		}

		HashNode* Begin()
		{
			if (m_pTravelHead)
			{
				return m_pTravelHead->m_pHead;
			}
			return nullptr;
		}

		void Clear()
		{
			for (int i = 0; i < m_nGroupCount; i++)
			{
				m_arrChains[i].FreeAll();
			}
			m_pTravelHead = nullptr;
		}
	
	protected:
		inline void InitGroup(int nGroupCount)
		{
			m_nGroupCount = nGroupCount;
			m_arrChains = new CChain[m_nGroupCount];
		}

		inline void InsertTravel(CChain *pChain)
		{
			if (pChain->m_pPrev || pChain->m_pNext)
			{
				return;
			}
			if (m_pTravelHead)
			{
				pChain->m_pNext = m_pTravelHead;
				m_pTravelHead->m_pPrev = pChain;
			}
			m_pTravelHead = pChain;
		}

		inline void RemoveTravel(CChain *pChain)
		{
			if (pChain->m_pPrev)
			{
				pChain->m_pPrev->m_pNext = pChain->m_pNext;
			}
			if (pChain->m_pNext)
			{
				pChain->m_pNext->m_pPrev = pChain->m_pPrev;
			}
			if (m_pTravelHead == pChain)
			{
				m_pTravelHead = pChain->m_pNext;
			}
			pChain->m_pPrev = nullptr;
			pChain->m_pNext = nullptr;
		}
	private:
		int m_nGroupCount;      //组数
		CChain* m_arrChains;    //链表数组
		HashFunc m_fnHash;      //哈希函数
		CChain *m_pTravelHead;  //迭代
	};
}