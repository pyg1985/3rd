#include "GameWnd.h"
#include "commdlg.h"
#include "MsgBoxWnd.h"

GameWnd::GameWnd(std::string& strId, std::string& strName, std::string& strPath, std::string& strCmd)
	: m_strId(strId)
	, m_strName(strName)
	, m_strPath(strPath)
	, m_strCmd(strCmd)
{

}

GameWnd::~GameWnd()
{

}

CDuiString GameWnd::GetSkinFolder()
	{	return _T("skin\\DemoRes\\");	}

CDuiString GameWnd::GetSkinFile()
	{	return _T("GameWnd.xml");	}

LPCTSTR GameWnd::GetWindowClassName(void) const
	{	return _T("GameEditWnd");	}

UINT GameWnd::GetClassStyle() const
	{	return UI_CLASSSTYLE_DIALOG;	}

void GameWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void GameWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		CDuiString strCtrlName = msg.pSender->GetName();
		if (strCtrlName == _T("OK"))
		{
			OnOK();
			return;
		}

		if (strCtrlName == _T("Cancel") || strCtrlName == _T("close_btn"))
		{
			OnCancel();
			return;
		}

		if (strCtrlName == _T("Browe"))
		{
			OnBrowe();
			return;
		}
		
	}
	WindowImplBase::Notify(msg);
}

LRESULT GameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lr =  WindowImplBase::OnCreate(uMsg, wParam, lParam, bHandled);
	OnInit();
	return lr;
}

void GameWnd::OnInit()
{
	CControlUI* pControl = m_PaintManager.FindControl(_T("ID"));
	if (pControl)
		pControl->SetText(m_strId.c_str());

	pControl = m_PaintManager.FindControl(_T("Name"));
	if (pControl)
		pControl->SetText(m_strName.c_str());

	pControl = m_PaintManager.FindControl(_T("Path"));
	if (pControl)
		pControl->SetText(m_strPath.c_str());

	pControl = m_PaintManager.FindControl(_T("Cmd"));
	if (pControl)
		pControl->SetText(m_strCmd.c_str());
}

void GameWnd::OnOK()
{
	CControlUI* pControl = m_PaintManager.FindControl(_T("ID"));
	if (pControl)
		m_strId = pControl->GetText().GetData();

	pControl = m_PaintManager.FindControl(_T("Name"));
	if (pControl)
		m_strName = pControl->GetText().GetData();

	pControl = m_PaintManager.FindControl(_T("Path"));
	if (pControl)
		m_strPath = pControl->GetText().GetData();

	pControl = m_PaintManager.FindControl(_T("Cmd"));
	if (pControl)
		m_strCmd = pControl->GetText().GetData();

	if (m_strId.empty() || m_strName.empty() || m_strPath.empty())
	{
		MsgBox(m_hWnd, _T("游戏ID,名称,路径均不能为空!"));
		return;
	}

	Close(IDOK);
}

void GameWnd::OnCancel()
{
	Close(IDCANCEL);
}

void GameWnd::OnBrowe()
{
	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[MAX_PATH] = { 0 };       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("所有文件(*.*)\0*.*\0可执行文件(*.exe)\0*.exe\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE)
	{
		CControlUI* pControl = m_PaintManager.FindControl(_T("Path"));
		if (pControl)
			pControl->SetText(ofn.lpstrFile);
	}
}