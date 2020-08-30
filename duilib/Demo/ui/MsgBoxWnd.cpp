#include "MsgBoxWnd.h"

MsgBoxWnd::MsgBoxWnd(const TCHAR* pszMsg)
	: m_strMsg(pszMsg)
{

}

MsgBoxWnd::~MsgBoxWnd()
{

}

CDuiString MsgBoxWnd::GetSkinFolder()
	{	return _T("skin\\DemoRes\\");	}

CDuiString MsgBoxWnd::GetSkinFile()
	{	return _T("MsgBox.xml");	}

LPCTSTR MsgBoxWnd::GetWindowClassName(void) const
	{	return _T("MsgBoxWnd");	}

UINT MsgBoxWnd::GetClassStyle() const
	{	return UI_CLASSSTYLE_DIALOG;	}

void MsgBoxWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void MsgBoxWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		CDuiString strCtrlName = msg.pSender->GetName();
		if (strCtrlName == _T("OK"))
		{
			Close(IDOK);
			return;
		}

		if (strCtrlName == _T("Cancel") || strCtrlName == _T("close_btn"))
		{
			Close(IDCANCEL);
			return;
		}
		
	}
	WindowImplBase::Notify(msg);
}

LRESULT MsgBoxWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lr =  WindowImplBase::OnCreate(uMsg, wParam, lParam, bHandled);
	
	CTextUI* pText = static_cast<CTextUI*>(m_PaintManager.FindControl(_T("msg_txt")));
	if (pText)
		pText->SetText(m_strMsg.GetData());

	return lr;
}

UINT MsgBox(HWND hWnd, const TCHAR* pszMsg)
{
	CDuiString strMsg = _T("游戏ID,名称,路径均不能为空!");
	MsgBoxWnd* pMsgWnd = new MsgBoxWnd(pszMsg);
	pMsgWnd->Create(hWnd, NULL, UI_WNDSTYLE_DIALOG, 0);
	pMsgWnd->CenterWindow();
	return pMsgWnd->ShowModal();
}