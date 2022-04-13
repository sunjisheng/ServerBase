#pragma once
namespace Minicat
{
	enum ServerConstDef
	{
		PeerType_Unknow = -1,
		Max_ServerName = 32,
		Max_ServerType_Count = 8,
		PeerType_Client = Max_ServerType_Count,
		Min_ServerID = 1,
		Max_ServerID = 16,
	};
}