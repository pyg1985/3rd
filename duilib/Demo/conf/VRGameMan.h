#ifndef _VRGameMan_h__
#define _VRGameMan_h__

#include <map>
#include "xobj.h"
#include "VRDict.h"
#include "xlocker.h"
#include "xcoretimer.h"

typedef void (*fnEventCallback)(int evt, const char* pszVRID, const SVRGameItem& VRGame, void* pUserData);

class VRGameMan
{
	VRGameMan(const VRGameMan&);
	VRGameMan& operator = (const VRGameMan&);

public:
	VRGameMan();
	~VRGameMan();

	int  GetServeBindPort() const;
	void ReportGameStatus(int status, const char* pszId = NULL);
	const TCHAR* GetMainWndSkinFile() const;

	void Save();
	void SetEventCallback(fnEventCallback fn, void* pUserData);
	void GetVRGameMap(std::map<std::string, SVRGameItem>& gameMap);

	int  AddVRGame(const char* pszID, const char* pszName, const char* pszRunPath, const char* pszRunCmd);
	int  DelVRGame(const char* pszID);

	int  GetVRGame(const char* pszID, std::string& strRunPath, std::string& strRunCmd);
	void SetVRGame(const char* pszID, const char* pszName, const char* pszRunPath, const char* pszRunCmd);
 	int  GetGameStatus(const char* pszID);
	OSProcessID  GetGameProcessId(const char* pszID);

	int  StartGame(const char* pszID, OSProcessID& pid);
	void StopGame(const char* pszID);

	UINT32 GetStartGameTimeout() const;
	void   SetStartGameTimeout(UINT32 timeout);

	UINT32 GetToken() const;
	UINT32 GenerateToken();

	UINT32 GetHeartbeatSec() const;
	void   SearchVRGame();

	BOOL   IsAuthEnable() const;
	const char* GetAuthServerIp() const;
	int GetAuthServerPort() const;
	const char* GetDeviceId() const;

private:
	void OnLoadConf();
	static void CheckTimer(void* param, TimerID id);
	void OnCheck();

private:
	std::map<std::string, SVRGameItem> m_VRGameMap;

	XCoreTimer m_checkTimer;
	fnEventCallback m_fun;
	void* m_pUserData;

	int m_serveBindPort;
	UINT32 m_startGameTimeout;
	UINT32 m_token;
	UINT32 m_heartbeatSec;
	BOOL m_bAutoSearchGame;
	std::string m_strMode;

	BOOL m_bAuthEnable;
	std::string m_authServerIp;
	int m_authServerPort;
	std::string m_deviceId;
};

STATIC_CLASS_OBJ_DECLARE(VRGameMan)

#endif	//_VRGameMan_h__