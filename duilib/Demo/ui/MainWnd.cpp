#include "MainWnd.h"
#include "MenuWnd.h"
#include "GameWnd.h"
#include "MsgBoxWnd.h"
#include "xfdk.h"
#include "moduleinfo.h"

MainWnd::MainWnd()
{
	GetVRGameMan()->SetEventCallback(EventCallback, this);
}

MainWnd::~MainWnd()
{
	PostQuitMessage(0);
}

LPCTSTR MainWnd::GetWindowClassName() const
	{	return _T("InfunVR");	}

CDuiString MainWnd::GetSkinFolder()
	{	return _T("skin\\DemoRes\\");	}

CDuiString MainWnd::GetSkinFile()
{	
	return GetVRGameMan()->GetMainWndSkinFile();
}

void MainWnd::Notify(TNotifyUI& msg)
{
	WindowImplBase::Notify(msg);
	if (msg.sType == DUI_MSGTYPE_MENU)
		OnPopupMenu(msg);

	if (msg.sType == DUI_MSGTYPE_WINDOWINIT) 
		OnPrepare();

	else if (msg.sType == _T("game_add"))
		OnAddGame();

	else if (msg.sType == _T("game_edit"))
		OnEditGame();

	else if (msg.sType == _T("game_del"))
		OnDelGame();

	else if (msg.sType == _T("log_copy"))
		OnLogCopy();

	else if (msg.sType == _T("log_paste"))
		OnLogPaste();

	else if (msg.sType == _T("log_cut"))
		OnLogCut();

	else if (msg.sType == _T("log_clear"))
		OnLogClear();

	else if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		CDuiString strName = msg.pSender->GetName();
		if (strName == _T("add"))
			OnAddGame();
		else if (strName == _T("del"))
			OnDelGame();
		else if (strName == _T("edit"))
			OnEditGame();
		else if (strName == _T("start"))
			OnStartGame();
		else if (strName == _T("stop"))
			OnStopGame();
	}
}

void MainWnd::OnPrepare()
{

}

void MainWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

LRESULT MainWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::OnSysCommand(uMsg, wParam, lParam, bHandled);
}

LRESULT MainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false;
	switch (uMsg)
	{
	case ON_GAME_STATUS_MSG:
		bHandled = true;
		OnGameStatusMsg(wParam, lParam);
		break;
	default:
		break;
	}
	return 0;
}

LRESULT MainWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lr = WindowImplBase::OnCreate(uMsg, wParam, lParam, bHandled);
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList)
	{
		pList->SetItemFixedHeight(30);
		//pList->SetAttribute(_T("itemalign"), _T("left"));
		CListHeaderUI* pListHeader = new CListHeaderUI();
 		pListHeader->SetBkImage(_T("list_header_bg.png"));
 		pListHeader->SetFixedHeight(30);

		const TCHAR* pszColTxtArray[] = { _T("ID"), _T("ID"), _T("Name"), _T("名称"), _T("Status"), _T("状态"), _T("Path"), _T("路径") };

		for (int i = 0; i < (int)sizeof(pszColTxtArray) / sizeof(const TCHAR*); i += 2)
		{
			CListHeaderItemUI* pListHeaderItem = new CListHeaderItemUI();
			pListHeaderItem->SetName(pszColTxtArray[i]);
			pListHeaderItem->SetText(pszColTxtArray[i + 1]);
			pListHeaderItem->SetSepImage(_T("list_header_sep.png"));
			pListHeaderItem->SetSepWidth(1);
			pListHeader->Add(pListHeaderItem);
		}
		pList->Add(pListHeader);
	}

	OnInitVRGameList();

	if (pList->GetCount() > 0)
		pList->SelectItem(0);

	return lr;
}

LRESULT MainWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lr = WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList)
	{
		RECT rec;
		::GetClientRect(m_hWnd, &rec);
		int w = rec.right - rec.left;
		pList->GetHeader()->SetSubControlFixedWdith("ID", w / 4);
		pList->GetHeader()->SetSubControlFixedWdith("Name", w / 4);
		pList->GetHeader()->SetSubControlFixedWdith("Status", w / 12);
	}

	CRichEditUI* pRichEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("logedit")));
	if (pRichEdit)
	{
		RECT rec;
		::GetClientRect(m_hWnd, &rec);
		int w = rec.right - rec.left;
		pRichEdit->SetFixedWidth(w - 4);
	}
	return lr;
}

LRESULT MainWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	lpMMI->ptMaxPosition.x = rcWork.left;
	lpMMI->ptMaxPosition.y = rcWork.top;
	lpMMI->ptMaxSize.x = rcWork.right;
	lpMMI->ptMaxSize.y = rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

void MainWnd::OnInitVRGameList()
{
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList == NULL)
		return;

	std::map<std::string, SVRGameItem> gameMap;
	GetVRGameMan()->GetVRGameMap(gameMap);
	for (std::map<std::string, SVRGameItem>::iterator it = gameMap.begin(); it != gameMap.end(); ++it)
	{
		SVRGameItem& item = it->second;
		CListTextElementUI* pListTextElement = new CListTextElementUI();
		pList->Add(pListTextElement);
		pListTextElement->SetText(0, it->first.c_str());
		pListTextElement->SetText(1, item.strName.c_str());
		pListTextElement->SetText(2, GetGameStatusDesc(item.status));

		if (IsAbsolutePath(item.strRunPath.c_str()))
			pListTextElement->SetText(3, item.strRunPath.c_str());
		else
		{
			TCHAR szPath[MAX_PATH] = { 0 };
			MergeFileName(GetExeDir(), item.strRunPath.c_str(), szPath);
			pListTextElement->SetText(3, szPath);
		}
	}
}

void MainWnd::ShowLog(const TCHAR* pszLog)
{
	SYSTEMTIME ltm;
	memset(&ltm, 0, sizeof(SYSTEMTIME));
	::GetLocalTime(&ltm);
	TCHAR szDate[32] = { 0 };
	sprintf_s(szDate, "%d-%02d-%02d %02d:%02d:%02d", ltm.wYear, ltm.wMonth, ltm.wDay, ltm.wHour, ltm.wMinute, ltm.wSecond);
	std::string strLog(szDate);
	strLog += " ";
	strLog += pszLog;
	strLog += "\r\n";

	CRichEditUI* pRichEditLog = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("logedit")));
	if (pRichEditLog)
	{
		if (pRichEditLog->GetTextLength() >= 64 * 1024)
		{
			pRichEditLog->SetSelAll();
			pRichEditLog->Clear();
		}
		pRichEditLog->AppendText(strLog.c_str());
	}
}

void MainWnd::OnPopupMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName() == _T("gamelist"))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		if (pMenu)
		{
			pMenu->SetXMLName(_T("rb_menu.xml"));
			pMenu->SetSize(150, 100);
			pMenu->SetMenuCtrlName(_T("rbmenulist"));
			pMenu->AddMenuItem(_T("game_add"), _T("增加"));
			pMenu->AddMenuItem(_T("game_del"), _T("删除"));
			pMenu->AddMenuItem(_T("game_edit"), _T("编辑"));

			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			::ClientToScreen(*this, &pt);
			pMenu->Init(msg.pSender, pt);
		}
	}
	else if (msg.pSender->GetName() == _T("logedit"))
	{
		CMenuWnd* pMenu = new CMenuWnd();
		if (pMenu)
		{
			pMenu->SetXMLName(_T("rb_menu.xml"));
			pMenu->SetSize(150, 130);
			pMenu->SetMenuCtrlName(_T("rbmenulist"));
			pMenu->AddMenuItem(_T("log_copy"), _T("复制"));
			pMenu->AddMenuItem(_T("log_cut"), _T("剪切"));
			pMenu->AddMenuItem(_T("log_clear"), _T("清除"));
			pMenu->AddMenuItem(_T("log_paste"), _T("粘贴"));
			
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			::ClientToScreen(*this, &pt);
			pMenu->Init(msg.pSender, pt);
		}
	}
}

void MainWnd::OnAddGame()
{
	std::string strId, strName, strPath, strCmd;
	GameWnd* pGameWnd = new GameWnd(strId, strName, strPath, strCmd);
	pGameWnd->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, 0);
	pGameWnd->CenterWindow();
	if (IDOK != pGameWnd->ShowModal())
		return;

	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList == NULL)
		return;
	
	int nRet = GetVRGameMan()->AddVRGame(strId.c_str(), strName.c_str(), strPath.c_str(), strCmd.c_str());
	if (nRet == 0)
	{
		CListTextElementUI* pItem = new CListTextElementUI();
		pList->Add(pItem);
		pItem->SetText(0, strId.c_str());
		pItem->SetText(1, strName.c_str());
		pItem->SetText(2, "停止");
		pItem->SetText(3, strPath.c_str());
		GetVRGameMan()->Save();
		int nCount = pList->GetCount();
		if (nCount > 0)
			pList->SelectItem(nCount - 1);
	}
	else
	{
		CDuiString strMsg = _T("游戏ID已存在!");
		MsgBoxWnd* pMsgWnd = new MsgBoxWnd(strMsg.GetData());
		pMsgWnd->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, 0);
		pMsgWnd->CenterWindow();
		pMsgWnd->ShowModal();
	}
}

void MainWnd::OnEditGame()
{
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList == NULL)
		return;

	int nCurSel = pList->GetCurSel();
	if (nCurSel < 0)
		return;

	CListTextElementUI* pListTextElement = static_cast<CListTextElementUI*>(pList->GetItemAt(nCurSel));
	if (pListTextElement == NULL)
		return;
	
	std::string strId = pListTextElement->GetText(0);
	std::string strName = pListTextElement->GetText(1);
	std::string strPath, strCmd;
	GetVRGameMan()->GetVRGame(strId.c_str(), strPath, strCmd);
	GameWnd* pGameWnd = new GameWnd(strId, strName, strPath, strCmd);
	pGameWnd->Create(m_hWnd, NULL, UI_WNDSTYLE_DIALOG, 0);
	pGameWnd->CenterWindow();
	if (IDOK == pGameWnd->ShowModal())
	{
		pListTextElement->SetText(0, strId.c_str());
		pListTextElement->SetText(1, strName.c_str());
		pListTextElement->SetText(3, strPath.c_str());
		GetVRGameMan()->SetVRGame(strId.c_str(), strName.c_str(), strPath.c_str(), strCmd.c_str());
		GetVRGameMan()->Save();
	}
}

void MainWnd::OnDelGame()
{
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList == NULL)
		return;

	int nCurSel = pList->GetCurSel();
	if (nCurSel < 0)
		return;

	CListTextElementUI* pListTextElement = static_cast<CListTextElementUI*>(pList->GetItemAt(nCurSel));
	if (pListTextElement == NULL)
		return;

	CDuiString strMsg = _T("你确定要删除");
	strMsg += pListTextElement->GetText(1);
	strMsg += _T("?");

	if (IDOK == MsgBox(m_hWnd, strMsg.GetData()))
	{
		pList->RemoveAt(nCurSel);
		LPCTSTR pszId = pListTextElement->GetText(0);
		GetVRGameMan()->DelVRGame(pszId);
		GetVRGameMan()->Save();
	}
}

void MainWnd::OnStartGame()
{
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList == NULL)
		return;

	CListTextElementUI* pListText = NULL;
	int nCurSel = pList->GetCurSel();
	if (nCurSel < 0 || (pListText = static_cast<CListTextElementUI*>(pList->GetItemAt(nCurSel))) == NULL)
		return;

	char szLog[1024] = { 0 };
	LPCTSTR pszId = pListText->GetText(0);
	LPCTSTR pszName = pListText->GetText(1);
	std::string strPath, strCmd;
	GetVRGameMan()->GetVRGame(pszId, strPath, strCmd);
	OSProcessID pid = INVALID_OSPROCESS_ID;
	int nRet = GetVRGameMan()->StartGame(pszId, pid);
	if (nRet == 0)
	{
		pListText->SetText(2, GetGameStatusDesc(E_VR_RUN_ING));
		sprintf_s(szLog, "启动游戏[%s][%s]cmd[%s]进程成功,pid[%d]", pszName, strPath.c_str(), strCmd.c_str(), pid);

		CControlUI* pStartControl = m_PaintManager.FindControl(_T("start"));
		CControlUI* pStopControl = m_PaintManager.FindControl(_T("stop"));
		if (pStartControl)
			pStartControl->SetVisible(false);
		if (pStopControl)
			pStopControl->SetVisible(true);
	}
	else
	{
		sprintf_s(szLog, "启动游戏[%s][%s %s]进程失败", pszName, strPath.c_str(), strCmd.c_str());
	}
	ShowLog(szLog);
}

void MainWnd::OnStopGame()
{
	CControlUI* pStartControl = m_PaintManager.FindControl(_T("start"));
	CControlUI* pStopControl = m_PaintManager.FindControl(_T("stop"));
	if (pStartControl)
		pStartControl->SetVisible(true);
	if (pStopControl)
		pStopControl->SetVisible(false);

	OSProcessID pid = GetVRGameMan()->GetGameProcessId(NULL);
	if (pid == INVALID_OSPROCESS_ID)
		return;

	GetVRGameMan()->StopGame(NULL);

	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList == NULL)
		return;

	int nCount = pList->GetCount();
	for (int i = 0; i < nCount; ++i)
	{
		CListTextElementUI* pListText = static_cast<CListTextElementUI*>(pList->GetItemAt(i));
		if (pListText)
			pListText->SetText(2, GetGameStatusDesc(E_VR_STOP));
	}

	char szLog[1024] = { 0 };
	sprintf_s(szLog, "停止游戏[%d]", pid);
	ShowLog(szLog);
}

void MainWnd::OnLogCopy()
{
	CRichEditUI* pRichEditLog = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("logedit")));
	if (pRichEditLog)
		pRichEditLog->Copy();
}
void MainWnd::OnLogCut()
{
	CRichEditUI* pRichEditLog = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("logedit")));
	if (pRichEditLog)
		pRichEditLog->Cut();
}

void MainWnd::OnLogPaste()
{
	CRichEditUI* pRichEditLog = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("logedit")));
	if (pRichEditLog)
		pRichEditLog->Paste();
}

void MainWnd::OnLogClear()
{
	CRichEditUI* pRichEditLog = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("logedit")));
	if (pRichEditLog)
	{
		pRichEditLog->SetSelAll();
		pRichEditLog->Clear();
	}
}

void MainWnd::EventCallback(int evt, const char* pszVRID, const SVRGameItem& VRGame, void* pUserData)
{
	MainWnd* pThis = (MainWnd*)pUserData;
	if (pThis)
		pThis->OnEvent(evt, pszVRID, VRGame);
}

void MainWnd::OnEvent(int evt, const char* pszVRID, const SVRGameItem& VRGame)
{
	SVRGameItem* pVRGame = new SVRGameItem();
	*pVRGame = VRGame;
	::SendMessage(m_hWnd, ON_GAME_STATUS_MSG, (WPARAM)pVRGame, evt);
}

void MainWnd::OnGameStatusMsg(WPARAM wParam, LPARAM lParam)
{
	SVRGameItem* pVRGame = (SVRGameItem*)wParam;
	if (pVRGame == NULL)
		return;

	int evt = 0;
	char szLog[1024] = { 0 };
	if (lParam == E_VR_RUN_TIMEOUT)
	{
		CControlUI* pStartControl = m_PaintManager.FindControl(_T("start"));
		CControlUI* pStopControl = m_PaintManager.FindControl(_T("stop"));
		if (pStartControl)
			pStartControl->SetVisible(true);
		if (pStopControl)
			pStopControl->SetVisible(false);

		evt = E_VR_STOP;
		sprintf_s(szLog, "启动游戏[%s]超时, 强制关闭进程", pVRGame->strName.c_str());
	}
	else if (lParam == E_VR_STOP)
	{
		CControlUI* pStartControl = m_PaintManager.FindControl(_T("start"));
		CControlUI* pStopControl = m_PaintManager.FindControl(_T("stop"));
		if (pStartControl)
			pStartControl->SetVisible(true);
		if (pStopControl)
			pStopControl->SetVisible(false);

		evt = E_VR_STOP;
		sprintf_s(szLog, "游戏[%s]关闭", pVRGame->strName.c_str());
	}
	else
	{
		evt = lParam;
		sprintf_s(szLog, "游戏[%s]%s", pVRGame->strName.c_str(), GetGameStatusDesc((int)lParam));
	}

	UpdateGameStatus(pVRGame->strId.c_str(), evt);
	ShowLog(szLog);
	delete pVRGame;
}

void MainWnd::UpdateGameStatus(const char* pszVRID, int status)
{
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("gamelist")));
	if (pList == NULL)
		return;

	int nCount = pList->GetCount();
	for (int i = 0; i < nCount; ++i)
	{
		CListTextElementUI* pListText = static_cast<CListTextElementUI*>(pList->GetItemAt(i));
		if (pListText == NULL)
			continue;
		
		CDuiString strId = pListText->GetText(0);
		if (strId == pszVRID)
		{
			pListText->SetText(2, GetGameStatusDesc(status));
			break;
		}
	}
}