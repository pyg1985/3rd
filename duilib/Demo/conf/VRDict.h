#ifndef _VRDict_h__
#define _VRDict_h__

#include "xtypes.h"

#define ON_GAME_STATUS_MSG		 WM_USER + 100

enum EVRGameStatus
{
	E_VR_STOP = 1,
	E_VR_RUN_ING,
	E_VR_REG,
	E_VR_RUN_OK,
	E_VR_RUN_TIMEOUT,
};

struct SVRGameItem
{
	BYTE status;
	OSProcessID pid;
	std::string strId;
	std::string strName;
	std::string strRunPath;
	std::string strRunCmd;
	LTMSEL startMsel;

	SVRGameItem()
	{
		status = E_VR_STOP;
		pid = INVALID_OSPROCESS_ID;
		strId = "";
		strName = "";
		strRunPath = "";
		strRunCmd = "";
	}
};

inline const char* GetGameStatusDesc(int status)
{
	const char* pszDesc = "ֹͣ";
	switch (status)
	{
	case E_VR_STOP:
		pszDesc = "ֹͣ";
		break;
	case E_VR_RUN_ING:
		pszDesc = "������";
		break;
	case E_VR_REG:
		pszDesc = "ע��";
		break;
	case E_VR_RUN_OK:
		pszDesc = "׼������";
		break;
	case E_VR_RUN_TIMEOUT:
		pszDesc = "������ʱ";
		break;
	default:
		break;
	}
	return pszDesc;
}

#endif	//_VRDict_h__