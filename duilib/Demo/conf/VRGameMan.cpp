#include <fstream>
#include <vector>
#include "VRGameMan.h"
#include "json.h"
#include "xstrcodec.h"
#include "moduleinfo.h"
#include "xfdk.h"
#include "VRDict.h"
#include "xprocess.h"
#include "xcore.h"
#include "xfdk.h"

VRGameMan::VRGameMan()
	: m_fun(NULL)
	, m_pUserData(NULL)
{
	OnLoadConf();
	if (m_bAutoSearchGame)
		SearchVRGame();
	m_checkTimer.Start(CheckTimer, this, 5000);
}

VRGameMan::~VRGameMan()
{
	m_checkTimer.Stop();
}

void VRGameMan::SearchVRGame()
{
	class VRGameDirEnumerator : public IFileEnumerationA
	{
	public:
		VRGameDirEnumerator(std::vector<std::string>& gameDirVec)
			: m_gameDirVec(gameDirVec)
		{
		}
		~VRGameDirEnumerator()
		{
		}

		virtual void EnumFile(const char* pszFile, LTMSEL lt_create, LTMSEL lt_access, LTMSEL lt_mod, BOOL fileORDir)
		{
			if (!fileORDir)
				m_gameDirVec.push_back(pszFile);
		}

	private:
		std::vector<std::string>& m_gameDirVec;
	};

	class VRGameExeEnumerator : public IFileEnumerationA
	{
	public:
		VRGameExeEnumerator(std::map<std::string, SVRGameItem>& gameMap, const char* pszGameName)
			: m_gameMap(gameMap)
			, m_gameName(pszGameName)
		{
		}
		~VRGameExeEnumerator()
		{
		}

		virtual void EnumFile(const char* pszFile, LTMSEL lt_create, LTMSEL lt_access, LTMSEL lt_mod, BOOL fileORDir)
		{
			const char* pszName = GetFileName(pszFile);
			if (pszName == NULL)
				return;

			std::string strPath;
			LPCTSTR pszCurExePath = GetExeDir();
			const char* pszSep = strstr(pszFile, pszCurExePath);
			if (pszSep)
			{
				strPath = ".\\";
				strPath += (pszFile + strlen(pszCurExePath));
			}
			else
			{
				strPath = pszFile;
			}

			std::map<std::string, SVRGameItem>::iterator it = m_gameMap.find(m_gameName);
			if (it == m_gameMap.end())
			{
				SVRGameItem vr;
				vr.strId = m_gameName;
				vr.strName = m_gameName;
				vr.strRunPath = strPath;
				m_gameMap.insert(std::make_pair(m_gameName, vr));
			}
			else
			{
				it->second.strRunPath = strPath;
			}
		}

	private:
		std::string m_gameName;
		std::map<std::string, SVRGameItem>& m_gameMap;
	};

	std::vector<std::string> gameDirVec;
	VRGameDirEnumerator dirEnum(gameDirVec);
	EnumPathFile(GetExeDir(), &dirEnum, true);

	for (int i = 0; i < (int)gameDirVec.size(); ++i)
	{
		const char* pszName = GetFileName(gameDirVec[i].c_str());
		if (pszName == NULL)
			continue;

		VRGameExeEnumerator exeEnum(m_VRGameMap, pszName);
		EnumPathFile(gameDirVec[i].c_str(), &exeEnum, true, "*.exe");
	}
}

UINT32 VRGameMan::GetToken() const
	{	return m_token;	}

UINT32 VRGameMan::GenerateToken()
{
	m_token = GetTickCount();
	return m_token;
}

const TCHAR* VRGameMan::GetMainWndSkinFile() const
{
	const TCHAR* pszFile = _T("");

	if (m_strMode == "multi")
		pszFile = _T("MainWnd.xml");
	else if (m_strMode == "personal")
		pszFile = _T("MainWnd_Personal.xml");

	return pszFile;
}

int VRGameMan::GetServeBindPort() const
	{	return m_serveBindPort;	}

UINT32 VRGameMan::GetHeartbeatSec() const
	{	return m_heartbeatSec;	}

BOOL VRGameMan::IsAuthEnable() const
	{	return m_bAuthEnable;	}

UINT32 VRGameMan::GetStartGameTimeout() const
	{	return m_startGameTimeout;	}

void VRGameMan::SetStartGameTimeout(UINT32 timeout)
	{	m_startGameTimeout = timeout;	}

const char* VRGameMan::GetAuthServerIp() const
	{	return m_authServerIp.c_str();	}

int VRGameMan::GetAuthServerPort() const
	{	return m_authServerPort;	}

const char* VRGameMan::GetDeviceId() const
	{	return m_deviceId.c_str();	}

void VRGameMan::OnLoadConf()
{
	m_startGameTimeout = 60;
	m_serveBindPort = 8521;
	m_heartbeatSec = 10;
	m_strMode = "personal";
	m_bAutoSearchGame = true;
	m_bAuthEnable = false;
	m_authServerIp = "127.0.0.1";
	m_authServerPort = 8501;
	m_deviceId = "202";
	char szPath[MAX_PATH] = { 0 };
	MergeFileName(GetExeDir(), "InfunVR.conf", szPath);
	WCHAR wszFilePath[MAX_PATH] = { 0 };
	ctowc(szPath, wszFilePath, sizeof(wszFilePath));

	std::ifstream ifs;
	ifs.open(wszFilePath);
	if (!ifs.is_open())
		return;

	Json::Reader reader;
	Json::Value rootValue;
	if (!reader.parse(ifs, rootValue))
	{
		ifs.close();
		return;
	}
	ifs.close();

	if (rootValue.isNull() || !rootValue.isObject())
		return;

	if (rootValue.isMember("game_list") && rootValue["game_list"].isArray())
	{
		int si = rootValue["game_list"].getArraySize();
		for (int i = 0; i < si; ++i)
		{
			SVRGameItem item;
			item.status = E_VR_STOP;
			item.strName = rootValue["game_list"][i]["name"].asCString();
			item.strRunCmd = rootValue["game_list"][i]["cmd"].asCString();
			item.strRunPath = rootValue["game_list"][i]["exe"].asCString();
			item.strId = rootValue["game_list"][i]["id"].asCString();
			m_VRGameMap.insert(std::make_pair(item.strId, item));
		}
	}

	if (rootValue.isMember("mode") && rootValue["mode"].isString())
		m_strMode = rootValue["mode"].asCString();

	if (rootValue.isMember("start_timeout") && rootValue["start_timeout"].isInt())
		m_startGameTimeout = rootValue["start_timeout"].asInt();

	if (rootValue.isMember("heartbeat_interval") && rootValue["heartbeat_interval"].isInt())
		m_heartbeatSec = rootValue["heartbeat_interval"].asInt();

	if (rootValue.isMember("auto_search_game") && rootValue["auto_search_game"].isBool())
		m_bAutoSearchGame = rootValue["auto_search_game"].asBool();

	if (rootValue.isMember("auth_enable") && rootValue["auth_enable"].isBool())
		m_bAuthEnable = rootValue["auth_enable"].asBool();

	if (rootValue.isMember("auth_server_ip") && rootValue["auth_server_ip"].isString())
		m_authServerIp = rootValue["auth_server_ip"].asCString();

	if (rootValue.isMember("auth_server_port") && rootValue["auth_server_port"].isInt())
		m_authServerPort = rootValue["auth_server_port"].asInt();

	if (rootValue.isMember("device_id") && rootValue["device_id"].isString())
		m_deviceId = rootValue["device_id"].asCString();
}

void VRGameMan::Save()
{
	Json::Value rootValue;
	rootValue["mode"] = m_strMode;
	rootValue["start_timeout"] = m_startGameTimeout;
	rootValue["heartbeat_interval"] = m_heartbeatSec;
	rootValue["auth_enable"] = m_bAuthEnable ? true : false;
	rootValue["auto_search_game"] = m_bAutoSearchGame ? true : false;
	rootValue["auth_server_ip"] = m_authServerIp;
	rootValue["auth_server_port"] = m_authServerPort;
	rootValue["device_id"] = m_deviceId;

	std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.begin();
	for (; it != m_VRGameMap.end(); ++it)
	{
		Json::Value jval;
		jval["id"] = it->first.c_str();
		jval["name"] = it->second.strName;
		jval["exe"] = it->second.strRunPath;
		jval["cmd"] = it->second.strRunCmd;
		rootValue["game_list"].append(jval);
	}

	char szPath[MAX_PATH] = { 0 };
	MergeFileName(GetExeDir(), "InfunVR.conf", szPath);
	WCHAR wszFilePath[MAX_PATH] = { 0 };
	ctowc(szPath, wszFilePath, sizeof(wszFilePath));
	std::ofstream ofs;
	ofs.open(wszFilePath);
	Json::StyledStreamWriter writer;
	writer.write(ofs, rootValue);
	ofs.close();
}

void VRGameMan::GetVRGameMap(std::map<std::string, SVRGameItem>& gameMap)
{	
	gameMap.insert(m_VRGameMap.begin(), m_VRGameMap.end());
}

int VRGameMan::AddVRGame(const char* pszID, const char* pszName, const char* pszRunPath, const char* pszRunCmd)
{
	int nRet = 1;
	std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.find(pszID);
	if (it == m_VRGameMap.end())
	{
		nRet = 0;
		SVRGameItem item;
		item.strId = pszID;
		item.startMsel = INVALID_UTC_MSEL;
		item.pid = INVALID_OSPROCESS_ID;
		item.status = E_VR_STOP;
		item.strName = pszName;
		item.strRunCmd = pszRunCmd;

		const char* pszExePath = GetExeDir();
		const char* pszSep = strstr(pszRunPath, pszExePath);
		if (pszSep)
		{
			item.strRunPath = ".\\";
			item.strRunPath += (pszRunPath + strlen(pszExePath));
		}
		else
		{
			item.strRunPath = pszRunPath;
		}

		m_VRGameMap.insert(std::make_pair(pszID, item));
	}
	return nRet;
}

int VRGameMan::DelVRGame(const char* pszID)
{
	m_VRGameMap.erase(pszID);
	return 0;
}

int VRGameMan::GetVRGame(const char* pszID, std::string& strRunPath, std::string& strRunCmd)
{
	int nRet = 1;
	std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.find(pszID);
	if (it != m_VRGameMap.end())
	{
		nRet = 0;
		
		if (IsAbsolutePath(it->second.strRunPath.c_str()))
			strRunPath = it->second.strRunPath;
		else
		{
			char szPath[MAX_PATH] = { 0 };
			MergeFileName(GetExeDir(), it->second.strRunPath.c_str(), szPath);
			strRunPath = szPath;
		}
		strRunCmd = it->second.strRunCmd;
	}
	return nRet;
}

void VRGameMan::SetVRGame(const char* pszID, const char* pszName, const char* pszRunPath, const char* pszRunCmd)
{
	std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.find(pszID);
	if (it != m_VRGameMap.end())
	{
		it->second.strName = pszName;
		const char* pszExePath = GetExeDir();
		const char* pszSep = strstr(pszRunPath, pszExePath);
		if (pszSep)
		{
			it->second.strRunPath = ".\\";
			it->second.strRunPath += (pszRunPath + strlen(pszExePath));
		}
		else
		{
			it->second.strRunPath = pszRunPath;
		}
		it->second.strRunCmd = pszRunCmd;
	}
}

int VRGameMan::StartGame(const char* pszID, OSProcessID& pid)
{
	int nRet = 1;
	std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.find(pszID);
	if (it != m_VRGameMap.end())
	{
		std::string strExe;
		if (IsAbsolutePath(it->second.strRunPath.c_str()))
			strExe = it->second.strRunPath;
		else
		{
			char szPath[MAX_PATH] = { 0 };
			MergeFileName(GetExeDir(), it->second.strRunPath.c_str(), szPath);
			strExe = szPath;
		}

		char szParam[1024] = { 0 };
		sprintf_s(szParam, " ?token=%u?port=%d", GenerateToken(), m_serveBindPort);
		strExe += szParam;

		if (!it->second.strRunCmd.empty())
			strExe += it->second.strRunCmd;

		pid = OSCreateProcess(strExe.c_str());
		if (pid != INVALID_OSPROCESS_ID)
		{
			nRet = 0;
			it->second.status = E_VR_RUN_ING;
			it->second.pid = pid;
			it->second.startMsel = GetMsel();
		}
	}
	return nRet;
}

void VRGameMan::StopGame(const char* pszID)
{
	if (pszID != NULL)
	{
		std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.find(pszID);
		if (it != m_VRGameMap.end())
		{
			SVRGameItem& item = it->second;
			if (item.pid != INVALID_OSPROCESS_ID)
			{
				OSCloseProcess(item.pid);
				item.pid = INVALID_OSPROCESS_ID;
			}
			item.status = E_VR_STOP;
			item.startMsel = INVALID_UTC_MSEL;
		}
	}
	else
	{
		std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.begin();
		while (it != m_VRGameMap.end())
		{
			SVRGameItem& item = it->second;
			if (item.pid != INVALID_OSPROCESS_ID)
			{
				OSCloseProcess(item.pid);
				item.pid = INVALID_OSPROCESS_ID;
			}
			item.status = E_VR_STOP;
			item.startMsel = INVALID_UTC_MSEL;
			++it;
		}
	}
}

int  VRGameMan::GetGameStatus(const char* pszID)
{
	int status = E_VR_STOP;
	std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.find(pszID);
	if (it != m_VRGameMap.end())
		status = it->second.status;
	return status;
}

OSProcessID VRGameMan::GetGameProcessId(const char* pszID)
{
	OSProcessID pid = INVALID_OSPROCESS_ID;
	if (pszID != NULL)
	{
		std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.find(pszID);
		if (it != m_VRGameMap.end())
			pid = it->second.pid;
	}
	else
	{
		std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.begin();
		while (it != m_VRGameMap.end())
		{
			if (it->second.pid != INVALID_OSPROCESS_ID && it->second.status != E_VR_STOP)
			{
				pid = it->second.pid;
				break;
			}
			++it;
		}
	}
	return pid;
}

void VRGameMan::SetEventCallback(fnEventCallback fn, void* pUserData)
{
	m_fun = fn;
	m_pUserData = pUserData;
}

void VRGameMan::CheckTimer(void* param, TTimerID id)
{
	VRGameMan* pThis = (VRGameMan*)param;
	if (pThis)
		pThis->OnCheck();
}

void VRGameMan::OnCheck()
{
	LTMSEL ltNow = GetMsel();

	for (std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.begin(); it != m_VRGameMap.end(); ++it)
	{
		SVRGameItem& item = it->second;
		if (item.status == E_VR_STOP || item.status == E_VR_RUN_OK)
			continue;

		LTMSEL timeout = m_startGameTimeout;
		LTMSEL n = ltNow - item.startMsel;
		if (n < timeout * 1000)
			continue;

		//Æô¶¯³¬Ê±
		item.startMsel = INVALID_UTC_MSEL;
		item.status = E_VR_STOP;
		if (item.pid != INVALID_OSPROCESS_ID)
		{
			OSCloseProcess(item.pid);
			item.pid = INVALID_OSPROCESS_ID;
		}

		if (m_fun)
			m_fun(E_VR_RUN_TIMEOUT, it->first.c_str(), item, m_pUserData);
	}
}

void VRGameMan::ReportGameStatus(int status, const char* pszId /*= NULL*/)
{
	SVRGameItem vr;
	std::string strName;

	if (pszId)
	{
		std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.find(pszId);
		if (it != m_VRGameMap.end())
		{
			it->second.status = status;
			strName = it->first.c_str();
			vr = it->second;
		}
	}
	
	for (std::map<std::string, SVRGameItem>::iterator it = m_VRGameMap.begin(); it != m_VRGameMap.end(); ++it)
	{
		SVRGameItem& item = it->second;
		if (item.status != E_VR_STOP)
		{
			item.status = status;
			vr = item;
			strName = it->first.c_str();
			break;
		}

	}

	if (m_fun && !strName.empty())
		m_fun(status, strName.c_str(), vr, m_pUserData);
}

STATIC_CLASS_OBJ(VRGameMan)