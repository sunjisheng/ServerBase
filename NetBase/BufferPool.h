/*********************************************************************************
  *作者:  SunJiSheng
  *日期:  2020/04/15
  *描述:  缓冲区池，为定长内存块链表，内存块可以循环使用，
		 用于分配不定长的消息包内存
**********************************************************************************/
#pragma once

#include "Singleton.h"
namespace Minicat
{
	enum
	{
		Max_Block_Count = 8192,   //最大块个数
		Max_Block_Size = 65536,   //每块的长度
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
		int nBlockNo;  //块号
		int nAllocLen; //已分配长度
		Block* pNext;  //下一块 
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