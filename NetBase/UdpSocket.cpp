#include "CommonBase.h"
#include "UdpSocket.h"
#include "SocketAPI.h"
#include "UdpProto.h"
#include "UdpManager.h"
#include "GlobalFunction.h"
#include "Random.h"
namespace Minicat
{
	CUdpSocket::CUdpSocket() : m_nStatus(Udp_Connect_None), m_nSocketID(INVALID_SOCKET)
	{
		m_nRTO = m_RTTStat.GetRTO();
		m_nSeq2Peer = (uint)CRandom::Rand();
		m_nAck2Me = 0;
		m_nSeq2Me = 0;
		m_nAck2Peer = 0;
		m_InStream.Init(1024);
		m_OutStream.Init(8192);

		for (int i = 0; i < Max_TimerID; i++)
		{
			m_arrTimeEvent[i].pOwner = this;
		}
		m_arrTimeEvent[TimerID_Connect].fnOnTimer = &CUdpSocket::OnTimeout_Connect;
		m_arrTimeEvent[TimerID_Send].fnOnTimer = &CUdpSocket::OnTimeout_Send;
		m_arrTimeEvent[TimerID_DelayAck].fnOnTimer = &CUdpSocket::OnTimeout_DelayAck;
	}

	CUdpSocket::~CUdpSocket()
	{

	}

	void CUdpSocket::Reset()
	{
		memset((char*)&m_addrPeer, 0, sizeof(m_addrPeer));
		m_nSocketID = INVALID_SOCKET;
		m_nPeerSocketID = INVALID_SOCKET;
		m_nRTO = m_RTTStat.GetRTO();
		m_nSeq2Peer = (uint)CRandom::Rand();
		m_nAck2Me = 0;
		m_nSeq2Me = 0;
		m_nAck2Peer = 0;
		m_nStatus = Udp_Connect_None;
		m_InputPackets.FreeAll();
		m_OutputPackets.FreeAll();
		m_InStream.Reset();
		m_OutStream.Reset();
		KillAllTimer();
	}

	void CUdpSocket::Dump(const char *str)
	{
		printf("%s sockid=%d peersockid=%d rto=%d seq2peer=%d ack2me=%d seq2me=%d ack2peer=%d status=%d inpackets=%d_%d,outpackets=%d_%d\r\n",
			str,
			m_nSocketID,
			m_nPeerSocketID,
			m_nRTO,
			m_nSeq2Peer,
			m_nAck2Me,
			m_nSeq2Me,
			m_nAck2Peer,
			m_nStatus,
			m_InputPackets.m_nHead, m_InputPackets.m_nDiff,
			m_OutputPackets.m_nHead, m_OutputPackets.m_nCount);
	}

	int CUdpSocket::SendPacket(UdpHeader *pPacket)
	{
		printf("SendPacket cmd=%d, len=%d, seq=%d, ack=%d, srcsock=%d, dstsock=%d\r\n",
			pPacket->nCmd,
			pPacket->nLen,
			pPacket->nSequence,
			pPacket->nAck,
			pPacket->nSrcSockID,
			pPacket->nDstSockID);
		return Minicat::SendTo(CUdpManager::Instance()->m_nSocketID, (const char*)pPacket, sizeof(UdpHeader), (sockaddr*)&m_addrPeer, sizeof(m_addrPeer));
	}

	void CUdpSocket::ReqConnect()
	{
		UdpHeader cmd;
		cmd.nCmd = Udp_Connect;
		cmd.nSequence = m_nSeq2Peer++;  //告知对方起始序号
		cmd.nSrcSockID = m_nSocketID;
		SendPacket(&cmd);
		SetTimer(TimerID_Connect, Connect_Timeout);
		m_nStatus = Udp_Connect_Syning;
	}

	void CUdpSocket::AckConnect()
	{
		SendHeader(Udp_Ack | Udp_Connect);
		m_nStatus = Udp_Connect_Acking;
	}

	void CUdpSocket::ReqClose()
	{
		if (!Send())
		{
			Dump("ReqClose");
			return;
		}
		SendHeader(Udp_Close);
		m_nStatus = Udp_Connect_FinWait;
	}

	void CUdpSocket::AckClose()
	{
		if (!Send())
		{
			Dump("AckClose");
			return;
		}
		SendHeader(Udp_Ack | Udp_Close);
		m_nStatus = Udp_Connect_CloseWait;
	}

	void CUdpSocket::SendHeader(ushort nCmd)
	{
		OutPacketBuf pkbuf;
		pkbuf.nLen = sizeof(UdpHeader);
		CUdpManager::Instance()->m_BufferPool.Alloc(&pkbuf);
		UdpHeader *pHeader = (UdpHeader *)pkbuf.szBuf;
		pHeader->nCmd = nCmd;
		pHeader->nLen = 0;
		pHeader->nSequence = m_nSeq2Peer++;  //告知对方起始序号
		pHeader->nAck = m_nSeq2Me + 1;
		pHeader->nSrcSockID = m_nSocketID;
		pHeader->nDstSockID = m_nPeerSocketID;
		SendPacket(pHeader);
		m_nAck2Peer = m_nSeq2Me + 1;
		m_OutputPackets.Push(&pkbuf);
		SetTimer(TimerID_Send, m_nRTO);
	}

	void CUdpSocket::SendAck()
	{
		UdpHeader cmd;
		cmd.nCmd = Udp_Ack;
		cmd.nLen = 0;
		cmd.nAck = m_nSeq2Me + 1;
		cmd.nSrcSockID = m_nSocketID;
		cmd.nDstSockID = m_nPeerSocketID;
		SendPacket(&cmd);
		m_nAck2Peer = m_nSeq2Me + 1;
	}

	bool CUdpSocket::Send()
	{
		while (m_OutStream.GetReadSize() > 0)
		{
			ushort nLen;
			if(!m_OutStream.ReadPeek((char*)&nLen, sizeof(nLen)))
			{
				Dump("Send ReadPeek");
				break;
			}

			if (m_OutStream.GetReadSize() < nLen + sizeof(UdpHeader))
			{
				Dump("GetReadSize");
				break;
			}

			OutPacketBuf pkbuf;
			pkbuf.m_tmSend = Minicat::GetTickCount();
			pkbuf.nLen = nLen + sizeof(UdpHeader);
			CUdpManager::Instance()->m_BufferPool.Alloc(&pkbuf);
			m_OutStream.Read(pkbuf.szBuf, pkbuf.nLen);
			
			UdpHeader *pHeader = (UdpHeader *)pkbuf.szBuf;
			pHeader->nSequence = m_nSeq2Peer++;
			//捎带确认
			if (m_nAck2Peer != m_nSeq2Me + 1)
			{
				m_nAck2Peer = m_nSeq2Me + 1;
				pHeader->nAck = m_nAck2Peer;
				pHeader->nCmd |= Udp_Ack;
				KillTimer(TimerID_DelayAck);
			}
			//发包
			if(SendPacket(pHeader) < 0)
			{
				Dump("Send SendPacket");
				return false;
			}
			SetTimer(TimerID_Send, m_nRTO);
			if (!m_OutputPackets.Push(&pkbuf))  //缓冲区已满，断开连接
			{
				Dump("Send m_OutputPackets.Push");
				m_nStatus = Udp_Connect_Closed;
				return false;
			}
		}
		return true;
	}

	int CUdpSocket::Recv(char *szBuf, int nLen)
	{
		int nPackLen = 0;
		if (!m_InStream.Peek((char*)&nPackLen, sizeof(nPackLen)))
		{
			return 0;
		}
		if (m_InStream.GetSize() < nPackLen + sizeof(nPackLen))
		{
			return 0;
		}
		if (nPackLen > nLen)
		{
			return 0;
		}
		m_InStream.Skip(sizeof(nPackLen));
		if(!m_InStream.Read(szBuf, nPackLen))
		{
			return 0;
		}
		return nPackLen;
	}

	void CUdpSocket::OnReqConnect(UdpHeader *pPacket, sockaddr_in &addrPeer)
	{
		if (m_nStatus != Udp_Connect_None)
		{
			return;
		}
		m_nPeerSocketID = pPacket->nSrcSockID;
		m_addrPeer = addrPeer;
		m_nSeq2Me = pPacket->nSequence; //记录对方的发送起始序号
		AckConnect();
	}

	void CUdpSocket::OnAckConnect(UdpHeader *pPacket)
	{
		if (m_nStatus != Udp_Connect_Syning)
		{
			return;
		}
		m_nPeerSocketID = pPacket->nSrcSockID;
		m_nSeq2Me = pPacket->nSequence; //记录对方的发送起始序号
		m_nAck2Me = pPacket->nAck;
		SendAck();
		m_nStatus = Udp_Connect_Establish;
		CUdpManager::Instance()->PushConnectEvent(NetEvent_Connect, m_nSocketID, inet_ntoa(m_addrPeer.sin_addr), ntohs(m_addrPeer.sin_port));
	}

	void CUdpSocket::OnReqClose(UdpHeader *pPacket)
	{
		AckClose();
	}

	void CUdpSocket::OnAckClose(UdpHeader *pPacket)
	{
		SendAck();
	}

	void CUdpSocket::OnPacket(UdpHeader *pPacket, sockaddr_in &addrPeer)
	{
		printf("OnPacket cmd=%d, len=%d, seq=%d, ack=%d, srcsock=%d, dstsock=%d\r\n",
			pPacket->nCmd,
			pPacket->nLen,
			pPacket->nSequence,
			pPacket->nAck,
			pPacket->nSrcSockID,
			pPacket->nDstSockID);

		if (pPacket->nCmd & Udp_Connect)
		{
			if (pPacket->nCmd & Udp_Ack)
			{
				OnAckConnect(pPacket);
			}
			else
			{
				OnReqConnect(pPacket, addrPeer);
			}
			return;
		}
		
		if (pPacket->nCmd & Udp_Data)
		{
			if(pPacket->nSequence == m_nAck2Peer)
			{
				char *szBuf = (char*)(pPacket + 1);
				int nLen = pPacket->nLen;
				m_InStream.Write((char*)&nLen, sizeof(nLen));
				m_InStream.Write((char*)(szBuf), nLen);  //未乱序的包直接写入输入缓冲区
				m_nSeq2Me = pPacket->nSequence;
				//再把临时缓冲区里该包后面的连续包写入输入缓冲区
				InPacketBuf pkbuf;
				while (m_InputPackets.Pop(&pkbuf))
				{
					m_InStream.Write((char*)&nLen, sizeof(nLen));
					m_InStream.Write((char*)(szBuf), nLen);
					m_nSeq2Me++;
				}
				SetTimer(TimerID_DelayAck, DelayAck_Timeout);
				CUdpManager::Instance()->PushEvent(NetEvent_Msg, m_nSocketID);

			}
			else if (IsValidSeq2Me(pPacket->nSequence))    //乱序的包写入临时缓冲区列表
			{
				InPacketBuf pkbuf;
				pkbuf.nLen = pPacket->nLen + sizeof(UdpHeader);
				if (!CBufferPool::Alloc(&pkbuf))
				{
					Dump("OnPacket CBufferPool::Alloc");
					return;
				}
				uint nDiff = pPacket->nSequence - m_nAck2Peer - 1;  //m_InputPackets里的0位置对应的序号为m_nAck2Peer + 1，所以再减1
				if (!m_InputPackets.Push(nDiff, &pkbuf))
				{
					Dump("OnPacket m_InputPackets.Push");
					m_nStatus = Udp_Connect_Closed;  //输入缓冲区满
				}
			}
		}

		if (pPacket->nCmd & Udp_Ack)
		{
			if (IsValidAck2Me(pPacket->nAck))  //收到确认
			{
				if (m_nStatus >= Udp_Connect_Establish)
				{
					uint nCount = pPacket->nAck - m_nAck2Me;
					for (int i = 0; i < nCount; i++)
					{
						OutPacketBuf *pBuf = nullptr;
						if (m_OutputPackets.Pick(pBuf))
						{
							m_nRTO = m_RTTStat.Calc(Minicat::GetTickCount() - pBuf->m_tmSend);
							m_OutputPackets.Pop();
						}
					}
				}
				m_nAck2Me = pPacket->nAck;
				if (m_OutputPackets.GetCount() == 0)
				{
					if (m_nStatus == Udp_Connect_Acking)  //被动连接完成
					{
						m_nStatus = Udp_Connect_Establish;
						KillTimer(TimerID_Connect);
						CUdpManager::Instance()->PushConnectEvent(NetEvent_Connect, m_nSocketID, inet_ntoa(m_addrPeer.sin_addr), ntohs(m_addrPeer.sin_port));
					}
					else if (m_nStatus == Udp_Connect_FinWait || m_nStatus == Udp_Connect_CloseWait)
					{
						m_nStatus = Udp_Connect_Closed;
						KillTimer(TimerID_Send);
						CUdpManager::Instance()->PushEvent(NetEvent_Close, m_nSocketID);
					}
				}
				else
				{
					SetTimer(TimerID_Send, m_nRTO);
				}
			}
		}

		if (pPacket->nCmd & Udp_Close)
		{
			if (pPacket->nCmd & Udp_Ack)
			{
				OnAckClose(pPacket);
			}
			else
			{
				OnReqClose(pPacket);
			}
		}
	}

	int CUdpSocket::GetReadSize()
	{
		return m_OutStream.GetReadSize();
	}

	void CUdpSocket::SetTimer(int nTimerID, int nFrame)
	{
		if (nTimerID < 0 || nTimerID >= Max_TimerID)
		{
			return;
		}
		//定时器在调度中
		if (m_arrTimeEvent[nTimerID].IsScheduling())
		{
			return;
		}
		CTimeScheduler<CUdpSocket>::Instance()->AddTimer(nFrame, &m_arrTimeEvent[nTimerID]);
	}

	void CUdpSocket::KillTimer(int nTimerID)
	{
		if (nTimerID < 0 || nTimerID >= Max_TimerID)
		{
			return;
		}
		if (!m_arrTimeEvent[nTimerID].IsScheduling())
		{
			return;
		}
		CTimeScheduler<CUdpSocket>::Instance()->DelTimer(&m_arrTimeEvent[nTimerID]);
	}

	void CUdpSocket::KillAllTimer()
	{
		for (int i = 0; i < Max_TimerID; i++)
		{
			if (!m_arrTimeEvent[i].IsScheduling())
			{
				continue;
			}
			CTimeScheduler<CUdpSocket>::Instance()->DelTimer(&m_arrTimeEvent[i]);
		}
	}

	void CUdpSocket::OnTimeout_Connect()
	{
		if (m_nStatus == Udp_Connect_Syning)
		{
			CUdpManager::Instance()->PushConnectEvent(NetEvent_ConnectFail, m_nSocketID, inet_ntoa(m_addrPeer.sin_addr), ntohs(m_addrPeer.sin_port));
			m_nStatus = Udp_Connect_Closed;
		}
		else
		{
			OutPacketBuf *pBuf = nullptr;
			if (m_OutputPackets.Pick(pBuf))
			{
				if (pBuf->m_nRetry >= AckConnect_TryCount - 1)
				{
					m_nStatus = Udp_Connect_Closed;
					CUdpManager::Instance()->DelSocket(this);
				}
				else
				{
					SendPacket((UdpHeader*)pBuf->szBuf);
					pBuf->m_nRetry++;
					SetTimer(TimerID_Connect, m_nRTO);
				}
			}
		}
	}

	//超时重传
	void CUdpSocket::OnTimeout_Send()
	{
		OutPacketBuf *pBuf = nullptr;
		if (m_OutputPackets.Pick(pBuf))
		{
			/*if (pBuf->m_nRetry >= Send_TryCount - 1)
			{
				m_nStatus = Udp_Connect_Closed;
				CUdpManager::Instance()->DelSocket(this);
			}
			else*/
			{
				SendPacket((UdpHeader*)pBuf->szBuf);
				pBuf->m_nRetry++;
				SetTimer(TimerID_Send, m_nRTO * pBuf->m_nRetry); //重传时间加倍
			}
		}
	}

	//不能捎带确认，必须单独发确认包
	void CUdpSocket::OnTimeout_DelayAck()
	{
		SendAck();
	}
}