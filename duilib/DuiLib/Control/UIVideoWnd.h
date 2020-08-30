#ifndef __UIVIDEOWND_H__
#define __UIVIDEOWND_H__

namespace DuiLib
{
	class CVideoWnd;
	class DUILIB_API CVideoWndUI : public CControlUI
	{
		friend class CVideoWnd;
	public:
		CVideoWndUI();
		~CVideoWndUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoInit();
		void SetPos(RECT rc, bool bNeedInvalidate);
		void Move(SIZE szOffset, bool bNeedInvalidate);
		void SetVisible(bool bVisible = true);
		void DoEvent(TEventUI& event);
		HWND GetNativeWindow() const;

	private:
		CVideoWnd* m_pVideoWnd;
	};
}

#endif // __UIVIDEOWND_H__