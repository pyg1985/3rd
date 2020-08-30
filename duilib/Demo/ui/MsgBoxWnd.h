#ifndef _MsgBox_h__
#define _MsgBox_h__

#include "UIlib.h"

using namespace DuiLib;

class MsgBoxWnd : public WindowImplBase
{
public:
	MsgBoxWnd(const TCHAR* pszMsg);
	~MsgBoxWnd();

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

private:
	CDuiString m_strMsg;
};

UINT MsgBox(HWND hWnd, const TCHAR* pszMsg);

#endif	//_MsgBoxWnd_h__