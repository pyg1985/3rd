#ifndef _MainWnd_h__
#define _MainWnd_h__

#include "VRGameMan.h"
#include "UIlib.h"
using namespace DuiLib;

class MainWnd : public WindowImplBase
{
public:
	MainWnd();
	~MainWnd();

public:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	static void EventCallback(int evt, const char* pszVRID, const SVRGameItem& VRGame, void* pUserData);
	void OnEvent(int evt, const char* pszVRID, const SVRGameItem& VRGame);
	void OnGameStatusMsg(WPARAM wParam, LPARAM lParam);

	void UpdateGameStatus(const char* pszVRID, int status);

private:
	void OnInitVRGameList();
	void ShowLog(const TCHAR* pszLog);
	void OnPrepare();
	void OnPopupMenu(TNotifyUI& msg);
	void OnAddGame();
	void OnEditGame();
	void OnDelGame();
	void OnStartGame();
	void OnStopGame();
	void OnLogCopy();
	void OnLogCut();
	void OnLogPaste();
	void OnLogClear();
};

#endif