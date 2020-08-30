#ifndef _GameSession_h__
#define _GameSession_h__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "xtcp.h"
#include "PubNet.h"

class GameSessionMaker : public xtcp_ISessionMaker
{
public:
	GameSessionMaker();
	~GameSessionMaker();

	virtual int GetSessionObjSize();
	virtual void MakeSessionObj(void* session);
};

class GameSession : public xtcp_TcpSessionTmpl
{
public:
	GameSession();
	~GameSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnClosed(int cause);
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

public:
	int SendBasicRes(UINT16 cmdCode, UINT16 retCode, BYTE pktType = E_PUB_PACKET_RES);
	int SendResponse(UINT16 cmdCode, UINT16 retCode, const void* pData, int nDataLen, BYTE pktType = E_PUB_PACKET_RES);

private:
	void InitParam();
	void ProcPacket(const void* buf, int bufLen);

private:
	enum EServerRecvStatus
	{
		recv_header,
		recv_payload
	};

	EServerRecvStatus m_recvStatus;	//数据接收状态
	DWORD m_dwPeerIp;
	SPubNetHeader m_pubNetHeader;
};

int  StartGameServe();
void StopGameServe();

#endif	//_GameSession_h__