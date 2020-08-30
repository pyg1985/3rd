#ifndef _MenuWnd_h__
#define _MenuWnd_h__

#include <windows.h>
#include <objbase.h>

#include "UIlib.h"
using namespace DuiLib;

class CShadowWnd : public CWindowWnd
{
public:
	CShadowWnd();
	~CShadowWnd();

public:
	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND hWnd);
	void RePaint();
  
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	bool m_bNeedUpdate;
	TDrawInfo m_di;
	CPaintManagerUI m_pm;
};

class CMenuWnd : public CWindowWnd
			   , public INotifyUI
{
public:
	CMenuWnd();

	~CMenuWnd();

public:
	void Init(CControlUI* pOwner, POINT pt);
	void AdjustPostion();
	LPCTSTR GetWindowClassName() const;
	void OnFinalMessage(HWND hWnd);

	void Notify(TNotifyUI& msg);
	HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
	void ShowWindow(bool bShow = true, bool bTakeFocus = true);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CPaintManagerUI m_pm;
    CControlUI* m_pOwner;
    POINT m_ptPos;
    CShadowWnd* m_pShadowWnd;

public:
	void SetMenuCtrlName(const TCHAR* pszName);
	void SetXMLName(const TCHAR* pszXMLName);
	void AddMenuItem(const TCHAR* pszName, const TCHAR* pszText);
	void SetSize(int w, int h);

private:
	CDuiString m_strMenuCtrlName;
	CDuiString m_strXMLName;
	CDuiStringPtrMap m_menuItemMap;

private:
	int m_width;
	int m_height;
};
#endif	//_MenuWnd_h__