/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  �������أ�Ϊ�����ڴ�������ڴ�����ѭ��ʹ�ã�
		 ���ڷ��䲻��������Ϣ���ڴ�
**********************************************************************************/
#pragma once

#include "Singleton.h"
namespace Minicat
{
	enum
	{
		Max_Block_Count = 8192,   //�������
		Max_Block_Size = 65536,   //ÿ��ĳ���
	};
#pragma pack(1) 
	struct PacketBuf
	{
		PacketBuf() :nBlockNo(0), szBuf(nullptr), nLen(0)
		{
		}

		void Reset()
		{
			nBlockNo = 0;
			szBuf = nullptr;
			nLen = 0;
		}

		bool IsValid()
		{
			return (szBuf != nullptr);
		}
		int nBlockNo;
		char *szBuf;
		int nLen;

	};

	struct Block
	{
		Block() :nBlockNo(0), nAllocLen(0), pNext(nullptr)
		{
		}
		int nBlockNo;  //���
		int nAllocLen; //�ѷ��䳤��
		Block* pNext;  //��һ�� 
	};
#pragma pack()
	class CBufferPool : public Singleton<CBufferPool>
	{
	public:
		static void Init();
		static void Release();
		static bool Alloc(PacketBuf *pBuf);
		static bool Free(PacketBuf *pBuf);
	protected:
		static Block* Expand();
		static void Recycle(Block *pBlock);
	public:
		static Block* m_pBlocks[Max_Block_Size];
		static Block* m_pCurrent;
		static int m_nBlockCount;
	};
}