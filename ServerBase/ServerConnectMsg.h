#pragma once
#include "Msg.h"
#include "ServerBaseProtocol.h"
namespace Minicat
{
	class ServerConnectMsg : public CMsg
	{
		virtual int MsgID()
		{
			return ServerConnect_Msg;
		}
		virtual int Length()
		{
			return sizeof(nServerType) + sizeof(lServerID);
		}
		virtual bool Read(CCircleQueue &queue)
		{
			ReadInt32(queue, nServerType);
			ReadInt64(queue, lServerID);
			return true;
		}
		virtual bool Write(CCircleQueue &queue)
		{
			WriteInt32(queue, nServerType);
			WriteInt64(queue, lServerID);
			return true;
		}
	public:
		int nServerType;
		int64 lServerID;
	};
}