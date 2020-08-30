#ifndef _GameWnd_h__
#define _GameWnd_h__

#include <windows.h>
#include <objbase.h>
#include <string>

#include "UIlib.h"
using namespace DuiLib;

class GameWnd : public WindowImplBase
{
public:
	GameWnd(std::string& strId, std::string& strName, std::string& strPath, std::string& strCmd);
	~GameWnd();

public:
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile();
	virtual UINT GetClassStyle() const;
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void OnFinalMessage(HWND hWnd);

	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void OnOK();
	void OnCancel();
	void OnInit();
	void OnBrowe();

private:
	std::string& m_strId;
	std::string& m_strName;
	std::string& m_strPath;
	std::string& m_strCmd;
};

#endif	//_GameWnd_h__