#pragma once
#include "CircleQueue.h"

namespace Minicat
{
#pragma pack(push, 1)
	struct MsgHeader
	{
		MsgHeader() : nMsgID(0), nLen(0)
		{
		}
		MsgHeader(int _nMsgID, int _nLen) : nMsgID(_nMsgID), nLen(_nLen)
		{
		}
		int nMsgID;
		int nLen;
	};
#pragma pack(pop)
	class CMsg
	{
	public:
		CMsg() {}
		virtual ~CMsg() {};
		virtual int MsgID() = 0;
		virtual int Length() = 0;
		virtual bool Read(CCircleQueue &queue) = 0;
		virtual bool Write(CCircleQueue &queue) = 0;
	public:
		string Name()
		{
			string str = typeid(*this).name();
			return str.substr(15);
		}
	protected:
		//Read
		inline bool ReadUInt8(CCircleQueue &queue, uint8& b)
		{
			return queue.Read((char*)&b, sizeof(uint8));
		}

		inline bool ReadShort(CCircleQueue &queue, short& s)
		{
			return queue.Read((char*)&s, sizeof(short));
		}

		inline bool ReadInt32(CCircleQueue &queue, int& i)
		{
			return queue.Read((char*)&i, sizeof(int));
		}

		inline bool ReadInt64(CCircleQueue &queue, int64& l)
		{
			return queue.Read((char*)&l, sizeof(int64));
		}

		inline bool ReadUInt64(CCircleQueue &queue, uint64& l)
		{
			return queue.Read((char*)&l, sizeof(uint64));
		}

		inline bool ReadBool(CCircleQueue &queue, bool& b)
		{
			return queue.Read((char*)&b, sizeof(bool));
		}

		inline bool ReadBuf(CCircleQueue &queue, char* szBuf, int nLen)
		{
			return queue.Read(szBuf, nLen);
		}
		//Write
		inline bool WriteUInt8(CCircleQueue &queue, uint8 b)
		{
			return queue.Write((char*)&b, sizeof(uint8));
		}

		inline bool WriteShort(CCircleQueue &queue, short& s)
		{
			return queue.Write((char*)&s, sizeof(short));
		}

		inline bool WriteInt32(CCircleQueue &queue, int& i)
		{
			return queue.Write((char*)&i, sizeof(int));
		}

		inline bool WriteInt64(CCircleQueue &queue, int64& l)
		{
			return queue.Write((char*)&l, sizeof(int64));
		}

		inline bool WriteUInt64(CCircleQueue &queue, uint64& l)
		{
			return queue.Write((char*)&l, sizeof(uint64));
		}

		inline bool WriteBool(CCircleQueue &queue, bool& b)
		{
			return queue.Write((char*)&b, sizeof(bool));
		}

		inline bool WriteBuf(CCircleQueue &queue, char* szBuf, int nLen)
		{
			return queue.Write(szBuf, nLen);
		}
	};
}