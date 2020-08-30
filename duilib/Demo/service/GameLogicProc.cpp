#include "GameLogicProc.h"
#include "PubErrorDesc.h"
//#include "CSNet.h"
#include "CCNet.h"
#include "GameSession.h"
#include "VRGameMan.h"

int Register(GameSession* pSession, const void* pData, int nDataLen)
{
	if (nDataLen < sizeof(SRegisterReq))
	{
		ASSERT(false);
		return pSession->SendBasicRes(PUB_REGISTER, YF_INVALID_PACKET);
	}

	SRegisterReq* pReq = (SRegisterReq*)pData;
	if (!IS_VALID_MAGIC_TOKEN(pReq->token))
		return pSession->SendBasicRes(PUB_REGISTER, YF_VRCC_INVALID_TOKEN);

	SRegisterRes res;
	memset(&res, 0, sizeof(SRegisterRes));
	res.heartSec = GetVRGameMan()->GetHeartbeatSec();
	res.retCode = YF_OK;
	pSession->SendResponse(PUB_REGISTER, YF_OK, &res, sizeof(SRegisterRes));
	GetVRGameMan()->ReportGameStatus(E_VR_REG);
	return 0;
}

int Heartbeat(GameSession* pSession, const void* pData, int nDataLen)
{
	pSession->GetServeMan()->UpdateSessionKeepAliveTick(pSession);
	return pSession->SendBasicRes(PUB_HEARTBEAT, YF_OK);
}

int QueryAuthCode(GameSession* pSession, const void* pData, int nDataLen)
{
	SQueryAuthCodeRes res;
	res.authCode = GetVRGameMan()->GetToken();
	return pSession->SendResponse(VRCC_QUERY_AUTH_CODE, YF_OK, &res, sizeof(SQueryAuthCodeRes));
}

int ReportCombatResult(GameSession* pSession, const void* pData, int nDataLen)
{	
	return pSession->SendBasicRes(PUB_REPORT_COMBAT_RESULT, YF_OK);
}

int ReportException(GameSession* pSession, const void* pData, int nDataLen)
{
	return pSession->SendBasicRes(VRCC_GAME_EXCEPTION, YF_OK);
}

int ReadyOK(GameSession* pSession, const void* pData, int nDataLen)
{
	pSession->SendBasicRes(VRCC_GAME_READY_OK, YF_OK);
	GetVRGameMan()->ReportGameStatus(E_VR_RUN_OK);
	return 0;
}
