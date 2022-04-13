#pragma once
#include "Msg.h"
#include "ServerBaseProtocol.h"
namespace Minicat
{
	class HeartbeatMsg : public CMsg
	{
	public:
		HeartbeatMsg()
		{
			nHeartbeat = 1;
		}
		virtual int MsgID()
		{
			return Heartbeat_Msg;
		}
		virtual int Length()
		{
			return sizeof(nHeartbeat);
		}
		virtual bool Read(CCircleQueue &queue)
		{
			ReadInt32(queue, nHeartbeat);
			return true;
		}
		virtual bool Write(CCircleQueue &queue)
		{
			WriteInt32(queue, nHeartbeat);
			return true;
		}
	public:
		int nHeartbeat;
	};
}