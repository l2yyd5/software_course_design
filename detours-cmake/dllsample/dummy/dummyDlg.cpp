
// dummyDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "dummy.h"
#include "dummyDlg.h"
#include "afxdialogex.h"
#include <WS2tcpip.h>
#include <winsock2.h>


#include <iostream>
#include <cstring>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

size_t strlength(const char*& c) {
	size_t tmp = 0;
	while (static_cast<int>(*(c + tmp)) >= 0x20 && static_cast<int>(*(c + tmp)) <= 0x7f) {
		tmp++;
	}
	return tmp;
}

void GetC(const char* c, char*& b)
{
	std::string tmp(c);
	size_t cSize = strlength(c);
	tmp = tmp.substr(0, cSize);
	memcpy(b, tmp.c_str(), cSize);
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CdummyDlg 对话框



CdummyDlg::CdummyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DUMMY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CdummyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, output);
}

BEGIN_MESSAGE_MAP(CdummyDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CdummyDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CdummyDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CdummyDlg::OnBnClickedButton3)
	ON_EN_CHANGE(IDC_EDIT2, &CdummyDlg::OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON4, &CdummyDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CdummyDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CdummyDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CdummyDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CdummyDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CdummyDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CdummyDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CdummyDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CdummyDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CdummyDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CdummyDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CdummyDlg::OnBnClickedButton15)
END_MESSAGE_MAP()


// CdummyDlg 消息处理程序

BOOL CdummyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CdummyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CdummyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CdummyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CdummyDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	::MessageBoxW(nullptr, L"MessageBoxW", L"Test", MB_OK);
}


void CdummyDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	::MessageBoxA(nullptr, "MessageBoxA", "Test", 0);
}


void CdummyDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start HeapCreate!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, PAGE_SIZE * 10, PAGE_SIZE * 100);
	if (hHeap == nullptr) {

		SetDlgItemText(IDC_EDIT2, "HeapCreate failed");
		return;
	}
	int* pArr = static_cast<int*>(HeapAlloc(hHeap, 0, sizeof(int) * 30));
	if (pArr == nullptr) {

		SetDlgItemText(IDC_EDIT2, "HeapCreate failed");
		return;
	}
	for (int i = 0; i < 30; ++i)
	{
		pArr[i] = i + 1;
	}

	tmpOutput += "Successfully created!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	HeapFree(hHeap, 0, pArr);
	tmpOutput += "Successfully free!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	HeapDestroy(hHeap);
	tmpOutput += "Successfully Destroy!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	return;
}

void CdummyDlg::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CdummyDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start CreateFile!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	HANDLE hOpenFile = static_cast<HANDLE>(CreateFile("a.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, NULL,
	                                                  nullptr));
	if (hOpenFile == INVALID_HANDLE_VALUE) {
		hOpenFile = nullptr;
		SetDlgItemText(IDC_EDIT2, "CreateFile failed");
		return;
	}

	tmpOutput += "Successfully created!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	tmpOutput += "Start WriteFile!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	bool ss = WriteFile(hOpenFile, "testtest", static_cast<size_t>(strlen("testtest")), nullptr, nullptr);
	if (ss) {
		tmpOutput += "Successfully Write!\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	}


	FlushFileBuffers(hOpenFile);
	CloseHandle(hOpenFile);
}


void CdummyDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start ReadFile!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	HANDLE hOpenFile = static_cast<HANDLE>(CreateFile("a.txt", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, NULL, nullptr));
	if (hOpenFile == INVALID_HANDLE_VALUE)
	{
		hOpenFile = nullptr;
		SetDlgItemText(IDC_EDIT2, "ReadFile failed");
		return;
	}

	tmpOutput += "Successfully the open file!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	int fileSize = GetFileSize(hOpenFile, nullptr);
	char* pBuf = new char[fileSize];
	bool ss = ReadFile(hOpenFile, pBuf, fileSize, nullptr, nullptr);
	if (ss) {
		tmpOutput += "Successfully Read: ";
		char* pBuffer = new char[fileSize];
		GetC(pBuf, pBuffer);
		tmpOutput += pBuffer;
		tmpOutput += "\r\n";

		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	}
	delete[]pBuf;
	CloseHandle(hOpenFile);
}


void CdummyDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey = nullptr;
	std::string tmpOutput("Start Create Reg Key!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	size_t Rvl1 = RegCreateKeyEx(HKEY_CURRENT_USER, "testRegKey", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, nullptr);
	if (Rvl1 == ERROR_SUCCESS) {
		tmpOutput += "RegKey create successfully\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	}
	else {
		SetDlgItemText(IDC_EDIT2, "RegKey create failed");
		return;
	}


	char value[255] = "testlzyyds";
	size_t vLen = strlen(value);

	size_t Rvl2 = RegSetValueEx(hKey, "lzlz", 0, REG_SZ, (CONST BYTE*)value, sizeof(wchar_t) * vLen);
	if (Rvl2 == ERROR_SUCCESS) {
		tmpOutput += "RegValue set successfully\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	}
	else {
		SetDlgItemText(IDC_EDIT2, "RegValue set failed");
		return;
	}

	RegCloseKey(hKey);
}


void CdummyDlg::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey = nullptr;
	std::string tmpOutput("Start Open Reg Key!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	size_t Rvl1 = RegOpenKeyEx(HKEY_CURRENT_USER, "testRegKey", 0, KEY_ALL_ACCESS, &hKey);

	if (Rvl1 == ERROR_SUCCESS) {
		tmpOutput += "RegKey open successfully\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	}
	else {
		SetDlgItemText(IDC_EDIT2, "RegKey open failed");
		return;
	}


	size_t Rvl2 = RegDeleteValue(hKey, "lzlz");
	if (Rvl2 == ERROR_SUCCESS) {
		tmpOutput += "RegValue delete successfully\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	}
	else {
		SetDlgItemText(IDC_EDIT2, "RegValue delete failed");
		return;
	}


	RegCloseKey(hKey);

	size_t Rvl3 = RegDeleteKey(HKEY_CURRENT_USER, "testRegKey");
	if (Rvl3 == ERROR_SUCCESS) {
		tmpOutput += "RegKey delete successfully\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	}
	else {
		SetDlgItemText(IDC_EDIT2, "RegKey delete failed");
		return;
	}
}


void CdummyDlg::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	WSADATA wsaData;
	std::string tmpOutput("Start Socket!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return;
	}
	//创建套接字
	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET)
	{
		SetDlgItemText(IDC_EDIT2, "Build socket failed");
		return;
	}

	tmpOutput += "Build socket successfully\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	//绑定套接字
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
	sockAddr.sin_family = PF_INET;  //使用IPv4地址
	inet_pton(AF_INET, "127.0.0.1", &sockAddr.sin_addr.s_addr);  //具体的IP地址
	sockAddr.sin_port = htons(56666);  //端口
	if (bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR))) {
		SetDlgItemText(IDC_EDIT2, "Bind failed");
		return;
	}

	tmpOutput += "Bind successfully\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	//进入监听状态
	if (listen(servSock, 20) == SOCKET_ERROR) {
		SetDlgItemText(IDC_EDIT2, "Listen failed");
		return;
	}

	tmpOutput += "Listen successfully\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	char recvData[100];
	memset(recvData, 0, sizeof(recvData));
	SOCKET clntSock = accept(servSock, static_cast<SOCKADDR*>(&clntAddr), &nSize);
	if (clntSock == INVALID_SOCKET)
	{
		SetDlgItemText(IDC_EDIT2, "Accept failed");
		return;
	}

	tmpOutput += "Accept successfully\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	int ret = recv(clntSock, recvData, 99, 0);

	if (ret > 0 && ret < 100)
	{
		recvData[ret] = 0x00;
		tmpOutput += "Recv successfully, recv data:\r\n";
		tmpOutput += recvData;
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	}

	closesocket(clntSock);
	closesocket(servSock);
	WSACleanup();
}


void CdummyDlg::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start Socket!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return;
	}
	SOCKET clntSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clntSock == INVALID_SOCKET)
	{
		SetDlgItemText(IDC_EDIT2, "Build socket failed");
		return;
	}

	tmpOutput += "Build socket successfully\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
	sockAddr.sin_family = PF_INET;  //使用IPv4地址
	inet_pton(AF_INET, "127.0.0.1", &sockAddr.sin_addr.s_addr);  //具体的IP地址
	sockAddr.sin_port = htons(56667);  //端口
	if (connect(clntSock, (sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{  //连接失败 
		SetDlgItemText(IDC_EDIT2, "Connect failed");
		closesocket(clntSock);
		return;
	}

	tmpOutput += "Connect successfully\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	std::string sendData("lzyyds");
	send(clntSock, sendData.c_str(), sendData.length(), 0);

	tmpOutput += "Send successfully\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	closesocket(clntSock);

	WSACleanup();
}


void CdummyDlg::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start HeapCreate!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, PAGE_SIZE * 10, PAGE_SIZE * 100);
	if (hHeap == nullptr) {
		SetDlgItemText(IDC_EDIT2, "HeapCreate failed");
		return;
	}
	int* pArr = static_cast<int*>(HeapAlloc(hHeap, 0, sizeof(int) * 30));
	if(pArr == nullptr) {
		return;
	}

	memset(pArr, 0, sizeof(pArr));
	if (pArr == nullptr) {

		SetDlgItemText(IDC_EDIT2, "HeapCreate failed");
		return;
	}
	for (int i = 0; i < 30; ++i)
	{
		pArr[i] = i + 1;
	}

	tmpOutput += "Successfully created!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	HeapFree(hHeap, 0, pArr);
	tmpOutput += "Successfully free!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());


	try {
		HeapFree(hHeap, 0, pArr);
		tmpOutput += "Double free!\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	}
	catch (...) {
		SetDlgItemText(IDC_EDIT2, "Double free error!\r\n");
	}

	HeapDestroy(hHeap);
	tmpOutput += "Successfully Destroy!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	return;
}


void CdummyDlg::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start Modify Executable File!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	HANDLE hOpenFile = static_cast<HANDLE>(CreateFile("a.exe", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, NULL,
	                                                  nullptr));

	tmpOutput += "Modified!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	CloseHandle(hOpenFile);
}


void CdummyDlg::OnBnClickedButton12()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start Self-replication!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	LPCSTR targetPath = "./test/test.exe";
	TCHAR tcBuf[_MAX_PATH * 2];
	GetModuleFileName(nullptr, tcBuf, sizeof(tcBuf));
	CopyFile(tcBuf, targetPath, FALSE);

	tmpOutput += "Copy successfully!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
}


void CdummyDlg::OnBnClickedButton13()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start open Another folder!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	HANDLE hOpenFile = static_cast<HANDLE>(CreateFile("../../test/test.exe", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr,
	                                                  CREATE_ALWAYS, NULL, nullptr));

	tmpOutput += "Open successfully!\r\n";
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	CloseHandle(hOpenFile);
}


void CdummyDlg::OnBnClickedButton14()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start open Startup Reg!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	HKEY hKey = nullptr;
	size_t rVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE/Microsoft/Windows/CurrentVersion/Run", 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hKey);

	if (rVal == ERROR_SUCCESS) {
		tmpOutput += "Open successfully!\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	}
	else {
		tmpOutput += "Open failed!\r\n";
		SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
	}
	RegCloseKey(hKey);
}


void CdummyDlg::OnBnClickedButton15()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string tmpOutput("Start Memory Copy!\r\n");
	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());

	char temp[100] = "";
	memcpy(temp, "hello\n", 6);
	memcpy(temp, "hello\n", 6);
	memcpy(temp, "hello\n", 6);

	memmove(temp, "world\n", 6);

	tmpOutput += "Memory Copy successfully!\r\n";

	SetDlgItemText(IDC_EDIT2, tmpOutput.c_str());
}
