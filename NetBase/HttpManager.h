/*********************************************************************************
  *����:  SunJiSheng
  *����:  2020/04/15
  *����:  HttpManager�ǵ��߳�Http API����⣬
**********************************************************************************/
#pragma once
#include "Thread.h"
#include "Singleton.h"
#include "SocketPoll.h"
#include "HttpSocket.h"
#include "CircleQueue.h"
#include "NetBase.h"
#include "HashMap.h"
#include "HttpListener.h"
#include "List.h"
#include "SocketFactory.h"
#include "SocketPair.h"
#include "NetBase.h"
namespace Minicat
{
	

	class CHttpManager : public CThread, public Singleton<CHttpManager>
	{
	public:
		CHttpManager();
		~CHttpManager();
	public:
		virtual void Run();											//�̺߳���
	public:
		//����ӿ�
		bool Init();										        //�����߳�
		bool Listen(int nPort);
		bool PostRequest(char *szUrl, HttpRequest *pRequest);
		bool AckResponse(SocketID nSocketID, HttpResponse *pResponse);
		void RegisterRequestHandler(HttpRequestHandler fnOnHttpRequest);
		void RegisterResponseHandler(HttpResponseHandler fnOnHttpResponse);
		void ProcessEvent();
		//SocketPool�ص��ӿ�
		void OnSocketRead(SocketID nSocketID);
		void OnSocketWrite(SocketID nSocketID);
	protected:
		void NotifyAct();
		void ProcessAct();
		bool InnerListen(int nPort);
		bool InnerConnect(HttpReqAct &act);
		bool InnerClose(SocketID nSocketID);
		bool InnerClose(CHttpSocket *pSocket);
		bool PushHttpRequestEvent(SocketID nSocketID, HttpRequest *pRequest);
		bool PushHttpResponseEvent(HttpRequest *pRequest, HttpResponse *pResponse);
	protected:
		HttpRequestHandler m_fnOnHttpRequest;
		HttpResponseHandler m_fnOnHttpResponse;
		CHttpListener m_Listener;
		SocketPoll *m_pSocketPoll;
		CCircleQueue m_queueAct;
		CCircleQueue m_queueEvent;
		CHashMap<SocketID, CHttpSocket*> m_mapSockets;
		CSocketPair *m_pSocketPair;
	};
}