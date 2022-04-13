#pragma once

namespace Minicat
{
	template<typename Key, typename Value>
	class CMap
	{
		enum
		{
			Key_Invalid = (Key)(-1),
			Color_Red,
			Color_Black,
		};
	public:
		class TreeNode
		{
			friend class CMap;
		public:
			TreeNode() : m_Key(Key_Invalid), m_nColor(Color_Black), m_pParent(NULL), m_pLeft(NULL), m_pRight(NULL)
			{
			}

			~TreeNode()
			{
			}

			bool IsNil()
			{
				return m_Key == Key_Invalid;
			}

			TreeNode *Next()
			{
				if (!m_pRight->IsNil())
				{
					TreeNode *pNode = m_pRight;
					while (!pNode->m_pLeft->IsNil())
					{
						pNode = pNode->m_pLeft;
					}
					return pNode;
				}
				else
				{
					TreeNode *pNode = this;
					TreeNode* pParent = m_pParent;
					while (!pParent->IsNil() && pNode == pParent->m_pRight)
					{
						pNode = pParent;
						pParent = pNode->m_pParent;
					}

					if (pParent->IsNil())
					{
						return NULL;
					}
					else
					{
						return pParent;
					}
				}
			}

		public:
			Key	m_Key;
			Value m_Value;
		private:
			int m_nColor;
			TreeNode *m_pParent;
			TreeNode *m_pLeft;
			TreeNode *m_pRight;
		};
	public:
		CMap()
		{
			m_pNil = new TreeNode();
			m_pNil->m_pLeft = m_pNil;
			m_pNil->m_pRight = m_pNil;
			m_pRoot = m_pNil;
			m_nCount = 0;
		}

		CMap(CMap& other)
		{
			*this = other;
		}

		CMap& operator=(CMap &other)
		{
			TreeNode *pNode = other.Begin();
			while (pNode != NULL)
			{
				Set(pNode->m_Key, pNode->m_Value);
				pNode = pNode->Next();
			}
			return *this;
		}

		~CMap()
		{
			Destroy(m_pRoot);
			delete m_pNil;
		}

		inline bool Empty()
		{
			if (m_pRoot->IsNil())
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		inline int Size()
		{
			return m_nCount;
		}

		inline bool Exist(Key key)
		{
			if (Find(key) != NULL)
			{
				return true;
			}
			return false;
		}

		bool Set(Key key, Value value)
		{
			TreeNode *pParent = m_pNil;
			TreeNode *pNode = m_pRoot;
			while (!pNode->IsNil())
			{
				pParent = pNode;
				if (key < pNode->m_Key)
				{
					pNode = Left(pNode);
				}
				else if (key > pNode->m_Key)
				{
					pNode = Right(pNode);
				}
				else
				{
					pNode->m_Value = value;
					return true;
				}
			}
			
			pNode = new TreeNode();
			pNode->m_Key = key;
			pNode->m_Value = value;
			pNode->m_nColor = Color_Red;
			pNode->m_pLeft = m_pNil;
			pNode->m_pRight = m_pNil;
			if (pParent->IsNil())
			{
				m_pRoot = pNode;
			}
			else
			{
				if (key < pParent->m_Key)
				{
					pParent->m_pLeft = pNode;
				}
				else
				{
					pParent->m_pRight = pNode;
				}
			}
			pNode->m_pParent = pParent;
			Insert_Fixup(pNode);
			m_nCount++;
			return true;
		}

		bool Get(Key key, Value& value)
		{
			TreeNode *pNode = Find(key);
			if (pNode)
			{
				value = pNode->m_Value;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool Erase(Key key)
		{
			TreeNode *pNode = Find(key);
			if (!pNode)
			{
				return false;
			}
			Erase(pNode);
			return true;
		}

		inline TreeNode* Erase(TreeNode *pNode)
		{
			TreeNode *pNext = pNode->Next();
			TreeNode *pReplace = m_pNil;
			if (Left(pNode)->IsNil() || Right(pNode)->IsNil())
			{
				pReplace = pNode;
			}
			else
			{
				pReplace = Successor(pNode);
				swap(pReplace->m_Key, pNode->m_Key);
				swap(pReplace->m_Value, pNode->m_Value);
			}
			TreeNode *pParent = Parent(pReplace);
			TreeNode *pChild = (!Left(pReplace)->IsNil()) ? Left(pReplace) : Right(pReplace);
			pChild->m_pParent = pParent;
			if (Left(pParent) == pReplace)
			{
				pParent->m_pLeft = pChild;
			}
			else
			{
				pParent->m_pRight = pChild;
			}
			if (pParent->IsNil())
			{
				m_pRoot = pChild;
			}
			if (pReplace->m_nColor == Color_Black)
			{
				Erase_Fixup(pChild);
			}
			delete pReplace;
			m_nCount--;
			return pNext;
		}

		inline TreeNode* Find(Key key)
		{
			TreeNode *pNode = m_pRoot;
			while (!pNode->IsNil() && pNode->m_Key != key)
			{
				if (key < pNode->m_Key)
				{
					pNode = pNode->m_pLeft;
				}
				else
				{
					pNode = pNode->m_pRight;
				}
			}
			if (pNode->IsNil())
			{
				return NULL;
			}
			else
			{
				return pNode;
			}
		}

		TreeNode* Begin()
		{
			TreeNode *pNode = m_pRoot;
			while (!Left(pNode)->IsNil())
			{
				pNode = Left(pNode);
			}
			if (pNode->IsNil())
			{
				return NULL;
			}
			else
			{
				return pNode;
			}
		}

		void Clear()
		{
			TreeNode *pNode = Begin();
			while (pNode != NULL)
			{
				TreeNode *pNext = pNode->Next();
				Erase(pNode);
				pNode = pNext;
			}
			m_nCount = 0;
		}
	private:

		inline TreeNode* Parent(TreeNode *pNode)
		{
			return pNode->m_pParent;
		}
		
		inline TreeNode* Left(TreeNode *pNode)
		{
			return pNode->m_pLeft;
		}

		inline TreeNode* Right(TreeNode *pNode)
		{
			return pNode->m_pRight;
		}

		inline bool IsLeft(TreeNode *pNode)
		{
			return pNode == Left(Parent(pNode));
		}

		inline bool IsRight(TreeNode *pNode)
		{
			return pNode == Right(Parent(pNode));
		}

		//ºó¼Ì
		inline TreeNode* Successor(TreeNode* pNode)
		{
			pNode = Right(pNode);
			while (!Left(pNode)->IsNil())
			{
				pNode = Left(pNode);
			}
			return pNode;
		}

		inline void Insert_Fixup(TreeNode *pNode)
		{
			while (Parent(pNode)->m_nColor == Color_Red)
			{
				TreeNode *pParent = Parent(pNode);
				if (IsLeft(pParent))
				{
					TreeNode *pUncle = Right(Parent(pParent));
					if (pUncle->m_nColor == Color_Red)
					{
						pParent->m_nColor = Color_Black;
						pUncle->m_nColor = Color_Black;
						pNode = Parent(pParent);
						pNode->m_nColor = Color_Red;
					}
					else
					{
						if (IsRight(pNode))
						{
							pNode = Parent(pNode);
							Left_Rotete(pNode);
							pParent = Parent(pNode);
						}
						pParent->m_nColor = Color_Black;
						Parent(pParent)->m_nColor = Color_Red;
						Right_Rotate(Parent(pParent));
					}
				}
				else
				{
					TreeNode *pUncle = Left(Parent(pParent));
					if (pUncle->m_nColor == Color_Red)
					{
						pParent->m_nColor = Color_Black;
						pUncle->m_nColor = Color_Black;
						pNode = Parent(pParent);
						pNode->m_nColor = Color_Red;
					}
					else
					{
						if (IsLeft(pNode))
						{
							pNode = Parent(pNode);
							Right_Rotate(pNode);
							pParent = Parent(pNode);
						}
						pParent->m_nColor = Color_Black;
						Parent(pParent)->m_nColor = Color_Red;
						Left_Rotete(Parent(pParent));
					}
				}
			}
			m_pRoot->m_nColor = Color_Black;
		}

		inline void Erase_Fixup(TreeNode *pNode)
		{
			while (pNode != m_pRoot && pNode->m_nColor == Color_Black)
			{
				if (IsLeft(pNode))
				{
					TreeNode *pBrother = Right(Parent(pNode));
					if (pBrother->m_nColor == Color_Red)
					{
						pBrother->m_nColor = Color_Black;
						Parent(pNode)->m_nColor = Color_Red;
						Left_Rotete(Parent(pNode));
						pBrother = Right(Parent(pNode));
					}
					
					if (Left(pBrother)->m_nColor == Color_Black && Right(pBrother)->m_nColor == Color_Black)
					{
						pBrother->m_nColor = Color_Red;
						pNode = Parent(pNode);
					}
					else
					{
						if (Right(pBrother)->m_nColor == Color_Black)
						{
							Left(pBrother)->m_nColor = Color_Black;
							pBrother->m_nColor = Color_Red;
							Right_Rotate(pBrother);
							pBrother = Right(Parent(pNode));
						}
						pBrother->m_nColor = Parent(pNode)->m_nColor;
						Parent(pNode)->m_nColor = Color_Black;
						Right(pBrother)->m_nColor = Color_Black;
						Left_Rotete(Parent(pNode));
						pNode = m_pRoot;
					}
				}
				else
				{
					TreeNode *pBrother = Left(Parent(pNode));
					if (pBrother->m_nColor == Color_Red)
					{
						pBrother->m_nColor = Color_Black;
						Parent(pNode)->m_nColor = Color_Red;
						Right_Rotate(Parent(pNode));
						pBrother = Left(Parent(pNode));
					}
					if (Left(pBrother)->m_nColor == Color_Black && Right(pBrother)->m_nColor == Color_Black)
					{
						pBrother->m_nColor = Color_Red;
						pNode = Parent(pNode);
					}
					else
					{
						if (Left(pBrother)->m_nColor == Color_Black)
						{
							Right(pBrother)->m_nColor = Color_Black;
							pBrother->m_nColor = Color_Red;
							Left_Rotete(pBrother);
							pBrother = Left(Parent(pNode));
						}
						pBrother->m_nColor = Parent(pNode)->m_nColor;
						Parent(pNode)->m_nColor = Color_Black;
						Left(pBrother)->m_nColor = Color_Black;
						Right_Rotate(Parent(pNode));
						pNode = m_pRoot;
					}
				}
			}
			pNode->m_nColor = Color_Black;
		}

		inline void Left_Rotete(TreeNode *pNode)
		{
			if (pNode->IsNil() || Right(pNode)->IsNil())
			{
				return;
			}
			TreeNode *pRight = Right(pNode);
			pNode->m_pRight = Left(pRight);
			if (!Left(pRight)->IsNil())
			{
				Left(pRight)->m_pParent = pNode;
			}
			pRight->m_pParent = Parent(pNode);
			if (Parent(pNode)->IsNil())
			{
				m_pRoot = pRight;
			}
			else if (IsLeft(pNode))
			{
				Parent(pNode)->m_pLeft = pRight;
			}
			else
			{
				Parent(pNode)->m_pRight = pRight;
			}
			pRight->m_pLeft = pNode;
			pNode->m_pParent = pRight;
		}

		inline void Right_Rotate(TreeNode *pNode)
		{
			if (pNode->IsNil() || Left(pNode)->IsNil())
			{
				return;
			}
			TreeNode *pLeft = Left(pNode);
			pNode->m_pLeft = Right(pLeft);
			if (!Right(pLeft)->IsNil())
			{
				Right(pLeft)->m_pParent = pNode;
			}
			pLeft->m_pParent = Parent(pNode);
			if (Parent(pNode)->IsNil())
			{
				m_pRoot = pLeft;
			}
			else if (IsLeft(pNode))
			{
				Parent(pNode)->m_pLeft = pLeft;
			}
			else
			{
				Parent(pNode)->m_pRight = pLeft;
			}
			pLeft->m_pRight = pNode;
			pNode->m_pParent = pLeft;
		}

		void Destroy(TreeNode *pNode)
		{
			if (pNode->IsNil())
			{
				return;
			}
			Destroy(Left(pNode));
			Destroy(Right(pNode));
			delete pNode;
		}
	private:
		TreeNode *m_pNil;
		TreeNode *m_pRoot;
		int m_nCount;
	};
}