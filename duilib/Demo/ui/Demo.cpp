// Demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "MainWnd.h"
#include "VRGameMan.h"
#include "GameSession.h"
#include "Verification.h"
#include "MsgBoxWnd.h"
#include "xipcvt.h"
#include "xfdk.h"
#include "moduleinfo.h"
#include "GatewayNet.h"

BOOL Authorization()
{
	if (!GetVRGameMan()->IsAuthEnable())
		return true;
	
	char szPath[MAX_PATH] = { 0 };
	MergeFileName(GetExeDir(), "pubkey.pem", szPath);
	FILE* pf = fopen(szPath, "r");
	if (pf == NULL)
		return false;

	INT64 si = GetFileSize(pf);
	if (si == 0)
	{
		fclose(pf);
		return false;
	}

	char* pszBuffer = (char*)malloc(si + 1);
	memset(pszBuffer, 0, si + 1);
	fread(pszBuffer, 1, si, pf);
	fclose(pf);

	std::string strKey = CreateCiphertext(GetVRGameMan()->GetDeviceId(), GetDWordIP("127.0.0.1"), pszBuffer);
	free(pszBuffer);

	SockInit();
	SOCKET sock = TCPNewClientByTime(GetDWordIP(GetVRGameMan()->GetAuthServerIp()), GetVRGameMan()->GetAuthServerPort(), 8000);
	if (!SockValid(sock))
		return false;

	AutoSock autoSock(sock);

	//����
	BYTE buf[1024] = { 0 };
	int nLen = PUB_BASIC_REQ_LEN + sizeof(SCheckAuthReq);
	SPubNetHeader* pNetHeader = (SPubNetHeader*)buf;

	//Э��ͷ
	pNetHeader->isCrypt = 0;
	pNetHeader->pduLen = PUB_REQ_HEAD_LEN + sizeof(SCheckAuthReq);
	pNetHeader->pktType = E_PUB_PACKET_REQ;
	pNetHeader->pktFlag = 1;
	pNetHeader->ver = PROTOCOL_VER;

	//������������ͷ
	SPubPayloadReq* pPubReq = (SPubPayloadReq*)(buf + PUB_NET_HEADER_LEN);
	pPubReq->cmdCode = VRGW_CHECK_AUTH;
	pPubReq->rawLen = PUB_REQ_HEAD_LEN + sizeof(SCheckAuthReq);

	//��������
	SCheckAuthReq* pAuthReq = (SCheckAuthReq*)(buf + PUB_BASIC_REQ_LEN);
	strcpy(pAuthReq->deviceId, GetVRGameMan()->GetDeviceId());
	memcpy(pAuthReq->authKey, strKey.c_str(), strKey.length());

	int n = SockWriteAllByTime(sock, buf, nLen, 3000);
	if (n != nLen)
		return false;

	//��������
	n = SockReadAllByTime(sock, buf, PUB_NET_HEADER_LEN, 10 * 1000);
	if (n != PUB_NET_HEADER_LEN)
		return false;

	pNetHeader = (SPubNetHeader*)buf;
	nLen = pNetHeader->pduLen;
	n = SockReadAllByTime(sock, buf, nLen, 10 * 1000);
	if (n != nLen)
		return false;

	SPubPayloadRes* pPubRes = (SPubPayloadRes*)buf;
	if (pPubRes->cmdCode != VRGW_CHECK_AUTH)
		return false;

	if (pPubRes->retCode != 0)
		return false;
	
	return true;
}

BOOL InitUI(HINSTANCE hInstance)
{
	CPaintManagerUI::SetInstance(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) 
		return false;

	MainWnd* pMainWnd = new MainWnd();
	if (pMainWnd == NULL)
		return false;

	pMainWnd->Create(NULL, _T("InfunVR"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0);
	pMainWnd->CenterWindow();
	pMainWnd->ShowWindow(true);

	BOOL bRet = Authorization();
	if (!bRet)
		MsgBox(pMainWnd->GetHWND(), "��Ȩʧ��, ������Ȩ�ļ�!");
	return bRet;
}

void RunMainLoop()
	{	CPaintManagerUI::MessageLoop();	}

void DestroyUI()
	{	::CoUninitialize();	}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	//��ʼ����Ϸ����
	InitVRGameMan();

	//��ʼ��UI
	if (!InitUI(hInstance))
		goto __end__;

	//��������
	StartGameServe();

	//ѭ��
	RunMainLoop();

__end__:

	//ֹͣ����
	StopGameServe();

	//����UI
	DestroyUI();

	//������Ϸ����
	DestroyVRGameMan();

	//����
	return 0;
}