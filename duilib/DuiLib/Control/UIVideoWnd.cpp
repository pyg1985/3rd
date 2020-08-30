#include "stdafx.h"
#include "UIVideoWnd.h"

namespace DuiLib
{
	class CVideoWnd : public CWindowWnd
	{
	public:
		CVideoWnd();

		void Init(CVideoWndUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		void PaintBkColor(HDC hDC, const RECT& rcPaint);
		void PaintBkImage(HDC hDC, const RECT& rcPaint);
		void PaintBorder(HDC hDC, const RECT& rcPaint);

	private:
		CVideoWndUI* m_pOwner;
	};

	CVideoWnd::CVideoWnd()
		: m_pOwner(NULL)
	{

	}

	void CVideoWnd::Init(CVideoWndUI* pOwner)
	{
		m_pOwner = pOwner;
		RECT rcPos = CalPos();
		UINT uStyle = WS_VISIBLE | WS_CHILD;
		Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
	}

	RECT CVideoWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		CControlUI* pParent = m_pOwner;
		RECT rcParent;
		while (pParent = pParent->GetParent()) {
			if (!pParent->IsVisible()) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
			rcParent = pParent->GetClientPos();
			if (!::IntersectRect(&rcPos, &rcPos, &rcParent)) {
				rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
				break;
			}
		}
		return rcPos;
	}

	LPCTSTR CVideoWnd::GetWindowClassName() const
	{
		return _T("VideoWnd");
	}

	LPCTSTR CVideoWnd::GetSuperClassName() const
	{
		return WC_STATIC;
	}

	void CVideoWnd::OnFinalMessage(HWND hWnd)
	{

	}

	LRESULT CVideoWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lr = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if (uMsg == WM_PAINT)
		{
			HDC hDstDC = GetDC(m_hWnd);
			RECT dstRec;
			GetClientRect(m_hWnd, &dstRec);
			PaintBkColor(hDstDC, dstRec);
			PaintBkImage(hDstDC, dstRec);
			PaintBorder(hDstDC, dstRec);
		}
		else if (uMsg == WM_KILLFOCUS || uMsg == WM_SETFOCUS) 
		{
			HDC hDstDC = GetDC(m_hWnd);
			RECT dstRec;
			GetClientRect(m_hWnd, &dstRec);
			PaintBorder(hDstDC, dstRec);
		}

		return lr;
	}

	void CVideoWnd::PaintBkColor(HDC hDC, const RECT& rcPaint)
	{
		//CRenderEngine::DrawColor(hDC, rcPaint, m_pOwner->GetBkColor());
		DWORD dwBackColor = m_pOwner->GetBkColor();
		DWORD dwBackColor2 = m_pOwner->GetBkColor2();
		DWORD dwBackColor3 = m_pOwner->GetBkColor3();
		RECT rcItem = m_pOwner->GetPos();
		if (dwBackColor != 0) {
			if (dwBackColor2 != 0) {
				if (dwBackColor3 != 0) {
					RECT rc = rcItem;
					rc.bottom = (rc.bottom + rc.top) / 2;
					CRenderEngine::DrawGradient(hDC, rc, m_pOwner->GetAdjustColor(dwBackColor), m_pOwner->GetAdjustColor(dwBackColor2), true, 8);
					rc.top = rc.bottom;
					rc.bottom = rcItem.bottom;
					CRenderEngine::DrawGradient(hDC, rc, m_pOwner->GetAdjustColor(dwBackColor2), m_pOwner->GetAdjustColor(dwBackColor3), true, 8);
				}
				else
					CRenderEngine::DrawGradient(hDC, rcItem, m_pOwner->GetAdjustColor(dwBackColor), m_pOwner->GetAdjustColor(dwBackColor2), true, 16);
			}
			else if (dwBackColor >= 0xFF000000) CRenderEngine::DrawColor(hDC, rcPaint, m_pOwner->GetAdjustColor(dwBackColor));
			else CRenderEngine::DrawColor(hDC, rcItem, m_pOwner->GetAdjustColor(dwBackColor));
		}
	}

	void CVideoWnd::PaintBkImage(HDC hDC, const RECT& rcPaint)
	{
		TDrawInfo& drawInfo = (TDrawInfo&)*m_pOwner->GetDrawInfo();
		CRenderEngine::DrawImage(hDC, m_pOwner->GetManager(), m_pOwner->GetPos(), rcPaint, drawInfo);
	}

	void CVideoWnd::PaintBorder(HDC hDC, const RECT& rcPaint)
	{
		RECT rcBorderSize = m_pOwner->GetBorderSize();
		DWORD dwBorderColor = m_pOwner->GetBorderColor();
		DWORD dwFocusBorderColor = m_pOwner->GetFocusBorderColor();
		SIZE cxyBorderRound = m_pOwner->GetBorderRound();
		RECT rcItem = rcPaint;//m_pOwner->GetPos();
		int nBorderStyle = m_pOwner->GetBorderStyle();

		if (rcBorderSize.left > 0 && (dwBorderColor != 0 || dwFocusBorderColor != 0)) {
			if (cxyBorderRound.cx > 0 || cxyBorderRound.cy > 0)//»­Ô²½Ç±ß¿ò
			{
				if (m_pOwner->IsFocused() && dwFocusBorderColor != 0)
					CRenderEngine::DrawRoundRect(hDC, rcItem, rcBorderSize.left, cxyBorderRound.cx, cxyBorderRound.cy, m_pOwner->GetAdjustColor(dwFocusBorderColor), nBorderStyle);
				else
					CRenderEngine::DrawRoundRect(hDC, rcItem, rcBorderSize.left, cxyBorderRound.cx, cxyBorderRound.cy, m_pOwner->GetAdjustColor(dwBorderColor), nBorderStyle);
			}
			else {
				if (rcBorderSize.right == rcBorderSize.left && rcBorderSize.top == rcBorderSize.left && rcBorderSize.bottom == rcBorderSize.left) {
					if (m_pOwner->IsFocused() && dwFocusBorderColor != 0)
						CRenderEngine::DrawRect(hDC, rcItem, rcBorderSize.left, m_pOwner->GetAdjustColor(dwFocusBorderColor), nBorderStyle);
					else
						CRenderEngine::DrawRect(hDC, rcItem, rcBorderSize.left, m_pOwner->GetAdjustColor(dwBorderColor), nBorderStyle);
				}
				else {
					RECT rcBorder;
					if (rcBorderSize.left > 0) {
						rcBorder = rcItem;
						rcBorder.left += rcBorderSize.left / 2;
						rcBorder.right = rcBorder.left;
						if (m_pOwner->IsFocused() && dwFocusBorderColor != 0)
							CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.left, m_pOwner->GetAdjustColor(dwFocusBorderColor), nBorderStyle);
						else
							CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.left, m_pOwner->GetAdjustColor(dwBorderColor), nBorderStyle);
					}
					if (rcBorderSize.top > 0) {
						rcBorder = rcItem;
						rcBorder.top += rcBorderSize.top / 2;
						rcBorder.bottom = rcBorder.top;
						rcBorder.left += rcBorderSize.left;
						rcBorder.right -= rcBorderSize.right;
						if (m_pOwner->IsFocused() && dwFocusBorderColor != 0)
							CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.top, m_pOwner->GetAdjustColor(dwFocusBorderColor), nBorderStyle);
						else
							CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.top, m_pOwner->GetAdjustColor(dwBorderColor), nBorderStyle);
					}
					if (rcBorderSize.right > 0) {
						rcBorder = rcItem;
						rcBorder.left = rcItem.right - rcBorderSize.right / 2;
						rcBorder.right = rcBorder.left;
						if (m_pOwner->IsFocused() && dwFocusBorderColor != 0)
							CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.right, m_pOwner->GetAdjustColor(dwFocusBorderColor), nBorderStyle);
						else
							CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.right, m_pOwner->GetAdjustColor(dwBorderColor), nBorderStyle);
					}
					if (rcBorderSize.bottom > 0) {
						rcBorder = rcItem;
						rcBorder.top = rcItem.bottom - rcBorderSize.bottom / 2;
						rcBorder.bottom = rcBorder.top;
						rcBorder.left += rcBorderSize.left;
						rcBorder.right -= rcBorderSize.right;
						if (m_pOwner->IsFocused() && dwFocusBorderColor != 0)
							CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.bottom, m_pOwner->GetAdjustColor(dwFocusBorderColor), nBorderStyle);
						else
							CRenderEngine::DrawLine(hDC, rcBorder, rcBorderSize.bottom, m_pOwner->GetAdjustColor(dwBorderColor), nBorderStyle);
					}
				}
			}
		}
	}

	CVideoWndUI::CVideoWndUI()
		: m_pVideoWnd(NULL)
	{
	}

	CVideoWndUI::~CVideoWndUI()
	{
		if (m_pVideoWnd)
		{
			delete m_pVideoWnd;
			m_pVideoWnd = NULL;
		}
	}

	void CVideoWndUI::DoInit()
	{
		m_pVideoWnd = new CVideoWnd();
		m_pVideoWnd->Init(this);
	}

	LPCTSTR CVideoWndUI::GetClass() const
	{
		return DUI_CTR_VIDEOWND;
	}

	LPVOID CVideoWndUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_VIDEOWND) == 0) return static_cast<CVideoWndUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	HWND CVideoWndUI::GetNativeWindow() const
	{
		HWND hWnd = NULL;
		if (m_pVideoWnd)
			hWnd = m_pVideoWnd->GetHWND();
		return hWnd;
	}

	void CVideoWndUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		if (m_pVideoWnd != NULL) {
		
			RECT rcPos = m_pVideoWnd->CalPos();
			if (::IsRectEmpty(&rcPos)) ::ShowWindow(m_pVideoWnd->GetHWND(), SW_HIDE);
			else {
				::SetWindowPos(m_pVideoWnd->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
			}
		}
	}

	void CVideoWndUI::Move(SIZE szOffset, bool bNeedInvalidate)
	{
		CControlUI::Move(szOffset, bNeedInvalidate);
		if (m_pVideoWnd != NULL) {
			RECT rcPos = m_pVideoWnd->CalPos();
			if (::IsRectEmpty(&rcPos)) ::ShowWindow(m_pVideoWnd->GetHWND(), SW_HIDE);
			else {
				::SetWindowPos(m_pVideoWnd->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
					rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
			}
		}
	}

	void CVideoWndUI::SetVisible(bool bVisible /*= true*/)
	{
		m_bVisible = bVisible;
		if (m_pVideoWnd)
		{
			if (bVisible)
				m_pVideoWnd->ShowWindow(true);
			else
				m_pVideoWnd->ShowWindow(false);
		}
	}

	void CVideoWndUI::DoEvent(TEventUI& event)
	{
		CControlUI::DoEvent(event);
		if (event.Type == UIEVENT_SETFOCUS)
		{
			m_bFocused = true;
			if(m_pVideoWnd)
				::SendMessage(m_pVideoWnd->GetHWND(), WM_SETFOCUS, 0, 0);
			//Invalidate();
		}
		else if (event.Type == UIEVENT_KILLFOCUS)
		{
			m_bFocused = false;
			if (m_pVideoWnd)
				::SendMessage(m_pVideoWnd->GetHWND(), WM_KILLFOCUS, 0, 0);
			//Invalidate();
		}
		else if (event.Type == UIEVENT_DBLCLICK)
		{
			GetManager()->SendNotify(this, DUI_MSGTYPE_DBCLICK, true);
		}
	}
}