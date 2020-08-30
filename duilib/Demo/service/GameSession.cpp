#include "GameSession.h"
#include "GameLogicProc.h"
#include "CCNet.h"
#include "PubErrorDesc.h"
#include "VRGameMan.h"

static xtcp_IServeMan* sg_pServeMan = NULL;
static xtcp_ISessionMaker* sg_pSessionMaker = NULL;

int StartGameServe()
{
	ASSERT(sg_pServeMan == NULL && sg_pSessionMaker == NULL);
	sg_pSessionMaker = new GameSessionMaker();
	ASSERT(sg_pSessionMaker != NULL);
	xtcp_MakeServeMan(sg_pServeMan, sg_pSessionMaker, 2048, GetVRGameMan()->GetHeartbeatSec() * 3);
	ASSERT(sg_pServeMan != NULL);
	return sg_pServeMan->StartServe(IPPADDR(INADDR_ANY, GetVRGameMan()->GetServeBindPort()));
}

void StopGameServe()
{
	if (sg_pServeMan != NULL)
	{
		sg_pServeMan->StopServe();
		sg_pServeMan->DeleteThis();
		sg_pServeMan = NULL;
	}

	if (sg_pSessionMaker != NULL)
	{
		delete sg_pSessionMaker;
		sg_pSessionMaker = NULL;
	}
}

GameSessionMaker::GameSessionMaker()
{
}

GameSessionMaker::~GameSessionMaker()
{

}

int GameSessionMaker::GetSessionObjSize()
	{	return sizeof(GameSession);	}

void GameSessionMaker::MakeSessionObj(void* session)
	{	::new(session) GameSession();	}

GameSession::GameSession()
{

}

GameSession::~GameSession()
{

}

void GameSession::InitParam()
{
	m_recvStatus = recv_header;
	m_dwPeerIp = INADDR_ANY;
}

void GameSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param/* = NULL*/)
{
	InitParam();
	m_dwPeerIp = peerIPP.ip_;
}

void GameSession::OnClosed(int cause)
{
	if (cause == XTCP_SELF_ACTIVE_CLOSED)
		return;

	GetVRGameMan()->ReportGameStatus(E_VR_STOP);
	InitParam();
}

int GameSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	if (recv_header == m_recvStatus)
	{
		buf = &m_pubNetHeader;
		bufLen = PUB_NET_HEADER_LEN;
		rt = io_mustRead;
		asynNotify = false;
	}
	else if (recv_payload == m_recvStatus)
	{
		buf = NULL;
		bufLen = m_pubNetHeader.pduLen;
		rt = io_mustRead;
		asynNotify = true;
	}
	else
		ASSERT(false);

	return XTCP_OK;
}

int GameSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	continueRecv = true;
	if (recv_header == m_recvStatus)
	{
		ASSERT(NULL != buf && PUB_NET_HEADER_LEN == bufLen);
		if (m_pubNetHeader.pduLen < PUB_REQ_HEAD_LEN || m_pubNetHeader.pduLen > MAX_PACKET_LEN)
			return XTCP_PASS_MAX_PACKET_SIZE;

		m_recvStatus = recv_payload;
	}
	else if (recv_payload == m_recvStatus)
	{
		ASSERT(buf != NULL && bufLen >= PUB_REQ_HEAD_LEN);
		ProcPacket(buf, bufLen);
		m_recvStatus = recv_header;
	}

	return XTCP_OK;
}

void GameSession::ProcPacket(const void* buf, int bufLen)
{
	SPubPayloadReq* req = (SPubPayloadReq*)buf;
	int nPayloadLen = bufLen - PUB_REQ_HEAD_LEN;
	BYTE* pPayload = req->data;

	switch (req->cmdCode)
	{
	case PUB_REGISTER:
		Register(this, pPayload, nPayloadLen);
		break;
	case PUB_HEARTBEAT:
		Heartbeat(this, pPayload, nPayloadLen);
		break;
	case VRCC_QUERY_AUTH_CODE:
		QueryAuthCode(this, pPayload, nPayloadLen);
		break;
	case PUB_REPORT_COMBAT_RESULT:
		ReportCombatResult(this, pPayload, nPayloadLen);
		break;
	case VRCC_GAME_EXCEPTION:
		ReportException(this, pPayload, nPayloadLen);
		break;
	case VRCC_GAME_READY_OK:
		ReadyOK(this, pPayload, nPayloadLen);
		break;
	default:
		SendBasicRes(req->cmdCode, YF_VRCC_UNKNOWN_CMD);
		break;
	}
}

int GameSession::SendBasicRes(UINT16 cmdCode, UINT16 retCode, BYTE pktType/* = E_PUB_PACKET_RES*/)
{
	BYTE buf[PUB_BASIC_RES_LEN] = { 0 };
	SPubNetHeader* pNetHeader = (SPubNetHeader*)buf;

	//协议头
	pNetHeader->isCrypt = 0;
	pNetHeader->pduLen = PUB_RES_HEAD_LEN;
	pNetHeader->pktType = pktType;
	pNetHeader->pktFlag = 1;
	pNetHeader->ver = PROTOCOL_VER;

	//公共数据响应头
	SPubPayloadRes* pPubRes = (SPubPayloadRes*)(buf + PUB_NET_HEADER_LEN);
	pPubRes->cmdCode = cmdCode;
	pPubRes->retCode = retCode;
	pPubRes->rawLen = PUB_RES_HEAD_LEN;

	return GetServeMan()->SendSessionData(this, buf, PUB_BASIC_RES_LEN);
}

int GameSession::SendResponse(UINT16 cmdCode, UINT16 retCode, const void* pData, int nDataLen, BYTE pktType/* = E_PUB_PACKET_RES*/)
{
	BYTE buf[PUB_BASIC_RES_LEN] = { 0 };
	SPubNetHeader* pNetHeader = (SPubNetHeader*)buf;

	//协议头
	pNetHeader->isCrypt = 0;
	pNetHeader->pduLen = PUB_RES_HEAD_LEN + nDataLen;
	pNetHeader->pktType = pktType;
	pNetHeader->pktFlag = 1;
	pNetHeader->ver = PROTOCOL_VER;

	//公共数据响应头
	SPubPayloadRes* pPubRes = (SPubPayloadRes*)(buf + PUB_NET_HEADER_LEN);
	pPubRes->cmdCode = cmdCode;
	pPubRes->retCode = retCode;
	pPubRes->rawLen = PUB_RES_HEAD_LEN + nDataLen;

	SockBufVec bufs[2];
	bufs[0].buf = buf;
	bufs[0].bufBytes = PUB_BASIC_RES_LEN;
	bufs[1].buf = (void*)pData;
	bufs[1].bufBytes = nDataLen;

	return GetServeMan()->SendSessionDataVec(this, bufs, 2);
}

