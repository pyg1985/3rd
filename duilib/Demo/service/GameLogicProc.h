#ifndef _GameLogicProc_h__
#define _GameLogicProc_h__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

class GameSession;

int Register(GameSession* pSession, const void* pData, int nDataLen);
int Heartbeat(GameSession* pSession, const void* pData, int nDataLen);
int QueryAuthCode(GameSession* pSession, const void* pData, int nDataLen);
int ReportCombatResult(GameSession* pSession, const void* pData, int nDataLen);
int ReportException(GameSession* pSession, const void* pData, int nDataLen);
int ReadyOK(GameSession* pSession, const void* pData, int nDataLen);

#endif	//_GameLogicProc_h__
