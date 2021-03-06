#include "GameWnd.h"
#include "MenuWnd.h"

inline HBITMAP CreateMyBitmap(HDC hDC, int cx, int cy, COLORREF** pBits)
{
	LPBITMAPINFO lpbiSrc = NULL;
	lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	if (lpbiSrc == NULL) return NULL;

	lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	lpbiSrc->bmiHeader.biWidth = cx;
	lpbiSrc->bmiHeader.biHeight = cy;
	lpbiSrc->bmiHeader.biPlanes = 1;
	lpbiSrc->bmiHeader.biBitCount = 32;
	lpbiSrc->bmiHeader.biCompression = BI_RGB;
	lpbiSrc->bmiHeader.biSizeImage = cx * cy;
	lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
	lpbiSrc->bmiHeader.biClrUsed = 0;
	lpbiSrc->bmiHeader.biClrImportant = 0;

	HBITMAP hBitmap = CreateDIBSection(hDC, lpbiSrc, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);
	delete[] lpbiSrc;
	return hBitmap;
}

CShadowWnd::CShadowWnd()
{
	m_di.sDrawString = _T("file='rb_menu_bk.png' corner='40,8,8,8'");
}

CShadowWnd::~CShadowWnd()
{

}

LPCTSTR CShadowWnd::GetWindowClassName() const 
	{ return _T("UIShadow"); }

UINT CShadowWnd::GetClassStyle() const 
	{ return UI_CLASSSTYLE_FRAME; }

void CShadowWnd::OnFinalMessage(HWND /*hWnd*/) 
	{ delete this; }

void CShadowWnd::RePaint()
{
	RECT rcClient = { 0 };
	::GetClientRect(m_hWnd, &rcClient);
	DWORD dwWidth = rcClient.right - rcClient.left;
	DWORD dwHeight = rcClient.bottom - rcClient.top;

	HDC hDcPaint = ::GetDC(m_hWnd);
	HDC hDcBackground = ::CreateCompatibleDC(hDcPaint);
	COLORREF* pBackgroundBits;
	HBITMAP hbmpBackground = CreateMyBitmap(hDcPaint, dwWidth, dwHeight, &pBackgroundBits);
	::ZeroMemory(pBackgroundBits, dwWidth * dwHeight * 4);
	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hDcBackground, hbmpBackground);

	CRenderEngine::DrawImage(hDcBackground, &m_pm, rcClient, rcClient, m_di);

	RECT rcWnd = { 0 };
	::GetWindowRect(m_hWnd, &rcWnd);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	POINT ptPos = { rcWnd.left, rcWnd.top };
	SIZE sizeWnd = { dwWidth, dwHeight };
	POINT ptSrc = { 0, 0 };
	UpdateLayeredWindow(m_hWnd, hDcPaint, &ptPos, &sizeWnd, hDcBackground, &ptSrc, 0, &bf, ULW_ALPHA);

	::SelectObject(hDcBackground, hOldBitmap);
	if (hDcBackground != NULL) ::DeleteDC(hDcBackground);
	if (hbmpBackground != NULL) ::DeleteObject(hbmpBackground);
	::ReleaseDC(m_hWnd, hDcPaint);

	m_bNeedUpdate = false;
}

LRESULT CShadowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE) SetTimer(m_hWnd, 9000, 10, NULL);
	else if (uMsg == WM_SIZE) m_bNeedUpdate = true;
	else if (uMsg == WM_CLOSE) KillTimer(m_hWnd, 9000);
	else if (uMsg == WM_TIMER) {
		if (LOWORD(wParam) == 9000 && m_bNeedUpdate == true) {
			if (!::IsIconic(m_hWnd)) RePaint();
		}
	}
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}


CMenuWnd::CMenuWnd() 
		: m_pOwner(NULL)
		, m_pShadowWnd(NULL)
{
};

CMenuWnd::~CMenuWnd()
{

}

void CMenuWnd::SetMenuCtrlName(const TCHAR* pszName)
{
	m_strMenuCtrlName = pszName;
}

void CMenuWnd::SetXMLName(const TCHAR* pszXMLName)
{
	m_strXMLName = pszXMLName;
}

void CMenuWnd::AddMenuItem(const TCHAR* pszName, const TCHAR* pszText)
{
	m_menuItemMap.Insert(pszName, (LPVOID)pszText);
}

void CMenuWnd::SetSize(int w, int h)
{
	m_width = w;
	m_height = h;
}

void CMenuWnd::Init(CControlUI* pOwner, POINT pt)
{
	if (pOwner == NULL) return;
	m_pOwner = pOwner;
	m_ptPos = pt;
	Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
	HWND hWndParent = m_hWnd;
	while (::GetParent(hWndParent) != NULL) hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

void CMenuWnd::AdjustPostion()
{
	CDuiRect rcWnd;
	GetWindowRect(m_hWnd, &rcWnd);
	int nWidth = rcWnd.GetWidth();
	int nHeight = rcWnd.GetHeight();
	rcWnd.left = m_ptPos.x;
	rcWnd.top = m_ptPos.y;
	rcWnd.right = rcWnd.left + nWidth;
	rcWnd.bottom = rcWnd.top + nHeight;
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;

	if (rcWnd.bottom > rcWork.bottom)
	{
		if (nHeight >= rcWork.GetHeight())
		{
			rcWnd.top = 0;
			rcWnd.bottom = nHeight;
		}
		else
		{
			rcWnd.bottom = rcWork.bottom;
			rcWnd.top = rcWnd.bottom - nHeight;
		}
	}
	if (rcWnd.right > rcWork.right)
	{
		if (nWidth >= rcWork.GetWidth())
		{
			rcWnd.left = 0;
			rcWnd.right = nWidth;
		}
		else
		{
			rcWnd.right = rcWork.right;
			rcWnd.left = rcWnd.right - nWidth;
		}
	}
	::SetWindowPos(m_hWnd, NULL, rcWnd.left - 4, rcWnd.top - 4, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

LPCTSTR CMenuWnd::GetWindowClassName() const
	{	return _T("MenuWnd");	};

void CMenuWnd::OnFinalMessage(HWND /*hWnd*/)
{
	delete this;	
};

void CMenuWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("itemselect")) {
		Close();
	}
	else if (msg.sType == _T("itemclick")) 
	{
		CDuiString strCtrlName = msg.pSender->GetName();
		if (m_menuItemMap.Find(strCtrlName.GetData()))
		{
			if (m_pOwner)
				m_pOwner->GetManager()->SendNotify(m_pOwner, strCtrlName, 0, 0, true);
		}
	}
}

HWND CMenuWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu /*= NULL*/)
{
	if (m_pShadowWnd == NULL) m_pShadowWnd = new CShadowWnd;
	m_pShadowWnd->Create(hwndParent, _T(""), WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS, WS_EX_LAYERED | WS_EX_TOOLWINDOW, x, y, cx, cy, NULL);

	dwExStyle |= WS_EX_TOOLWINDOW;
	return CWindowWnd::Create(hwndParent, pstrName, dwStyle, dwExStyle, x, y, cx, cy, hMenu);
}

void CMenuWnd::ShowWindow(bool bShow/* = true*/, bool bTakeFocus/* = true*/)
{
	if (m_pShadowWnd != NULL) m_pShadowWnd->ShowWindow(bShow, false);
	CWindowWnd::ShowWindow(bShow, bTakeFocus);
}

LRESULT CMenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pm.Init(m_hWnd);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(m_strXMLName.GetData()/*_T("rb_menu.xml")*/, (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	if (pRoot == NULL)
		return 0;

	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);
	//m_pm.SetRoundCorner(0, 0);
	ResizeClient(m_width, m_height);
	
	CListUI* pMenuList = static_cast<CListUI*>(m_pm.FindControl(m_strMenuCtrlName.GetData()/*_T("rbmenulist")*/));
	if (pMenuList)
	{
		pMenuList->SetItemFixedHeight(30);
		pMenuList->SetAttribute(_T("itemalign"), _T("center"));
		pMenuList->SetAttribute(_T("itemvalign"), _T("center"));
		pMenuList->SetAttribute(_T("itemfont"), _T("0"));

		int si = m_menuItemMap.GetSize();
		for (int i = 0; i < si; ++i)
		{
			CListLabelElementUI* pListLabelElement = new CListLabelElementUI();
			pMenuList->Add(pListLabelElement);

			LPCTSTR pszKey = m_menuItemMap[i];
			pListLabelElement->SetName(pszKey);
			LPCTSTR pszText = (LPCTSTR)m_menuItemMap.Find(pszKey);
			pListLabelElement->SetText(pszText);
		}
	}
	AdjustPostion();
	return 0;
}

LRESULT CMenuWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pShadowWnd != NULL) m_pShadowWnd->Close();
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ((HWND)wParam == m_hWnd) bHandled = TRUE;
	else if (m_pShadowWnd != NULL && (HWND)wParam == m_pShadowWnd->GetHWND())
	{
		CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		::SetFocus(m_hWnd);
		bHandled = TRUE;
	}
	else
	{
		Close();
		bHandled = FALSE;
	}
	return 0;
}

LRESULT CMenuWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_ESCAPE)
		Close();
	return 0;
}

LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!::IsIconic(*this))
	{
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		HRGN hRgn = ::CreateRectRgn(rcWnd.left + 8, rcWnd.top + 8, rcWnd.right - 8, rcWnd.bottom - 8);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
	if (m_pShadowWnd != NULL)
	{
		RECT rcWnd = { 0 };
		::GetWindowRect(m_hWnd, &rcWnd);
		::SetWindowPos(*m_pShadowWnd, m_hWnd, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left,
			rcWnd.bottom - rcWnd.top, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pShadowWnd != NULL)
	{
		RECT rcWnd = { 0 };
		::GetWindowRect(m_hWnd, &rcWnd);
		::SetWindowPos(*m_pShadowWnd, m_hWnd, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left,
			rcWnd.bottom - rcWnd.top, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg)
	{
	case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:       lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:    break;
	case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_MOVE:          lRes = OnMove(uMsg, wParam, lParam, bHandled); break;
	default:
		bHandled = FALSE;
	}
	if (bHandled) return lRes;
	if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}