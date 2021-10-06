#include <stdio.h>
#include <Windows.h>

#include <array>
#include <iostream>
#include <strstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <detours.h>

#define ATTACH(x)       DetAttach(&(PVOID&)Real_##x, Mine_##x,#x)
#define DETACH(x)       DetDetach(&(PVOID&)Real_##x, Mine_##x,#x)

VOID DetAttach(PVOID* ppvReal, PVOID pvMine, PCHAR psz)
{
	PVOID pvReal = nullptr;
	if (ppvReal == nullptr) {
		ppvReal = &pvReal;
	}

	DetourAttach(ppvReal, pvMine);
}

VOID DetDetach(PVOID* ppvReal, PVOID pvMine, PCHAR psz)
{
	DetourDetach(ppvReal, pvMine);
	(void)psz;
}

using namespace std;

struct mmapInfo {
	unsigned short type;
	unsigned short argNums;
	SYSTEMTIME st;
	array<array<char, 100>, 10> argNames{};
	array<array<char, 100>, 10> argValues{};
};

static void* (__cdecl* Real_memcpy)(void* _Dst,
	const void* _Src,
	size_t _Size)
	= memcpy;

template<class src_type>
string type2str(src_type src, int n = 0, int longs = 100) {
	stringstream ss;
	ss << setw(n) << setfill('0') << src;
	string ret;
	ss >> ret;
	if (ret.length() >= 100) {
		return ret.substr(0, 99);
	}
	return ret;
}

template<class src_type>
string type2str(src_type* src, int n = 0) {
	if (src == nullptr) {
		return string("0", n);
	}
	stringstream ss;
	ss << setw(n) << setfill('0') << src;
	string ret;
	ss >> ret;
	if (ret.length() >= 100) {
		return ret.substr(0, 99);
	}
	return ret;
}

string type2str(char* src, int n = 0) {
	if (src == nullptr) {
		return string("0", n);
	}
	stringstream ss;
	ss << setw(n) << setfill('0') << *src;
	string ret;
	ss >> ret;
	if (ret.length() >= 100) {
		return ret.substr(0, 99);
	}
	return ret;
}

array<char, 100> toArray(string temp) {
	array<char, 100> arr{};
	int length = min(100, temp.length());
	for (int i = 0; i < length; i++) {
		arr[i] = temp[i];
	}
	return arr;
}

array<char, 256> toArray2(string temp) {
	array<char, 256> arr{};
	for (int i = 0; i < temp.length(); i++) {
		arr[i] = temp[i];
	}
	return arr;
}

#pragma data_seg("DataShare")
STARTUPINFO sui = { sizeof(STARTUPINFO) };
PROCESS_INFORMATION pri = {};
int moduleNum = 0;
array<array<char, 256>, 100> moduleNames{};
mmapInfo seg[1000] = {};
int numbercount = 0;
mutex mtx;
#pragma data_seg()
#pragma comment(linker, "/section:DataShare,RWS")

extern "C" __declspec(dllexport)void ChangeProInfo(STARTUPINFO * ss, PROCESS_INFORMATION * pp) {
	mtx.lock();
	Real_memcpy(&sui, ss, sizeof(STARTUPINFO));
	Real_memcpy(&pri, pp, sizeof(PROCESS_INFORMATION));
	mtx.unlock();
}

extern "C" __declspec(dllexport)void ChangeModule(char* modulename) {
	mtx.lock();
	if (moduleNum >= 100) {
		moduleNum--;
	}
	else {
		Real_memcpy(&moduleNames[moduleNum++], modulename, 256);
	}
	mtx.unlock();
}

extern "C" __declspec(dllexport)void ChangeSeg(mmapInfo * str) {
	mtx.lock();
	if (numbercount >= 1000) {
		numbercount--;
	}
	else {
		Real_memcpy(&seg[numbercount++], str, sizeof(mmapInfo));
	}
	mtx.unlock();
}

extern "C" __declspec(dllexport)void GetSeg(mmapInfo * str, int i) {
	mtx.lock();
	Real_memcpy(str, &seg[i], sizeof(mmapInfo));
	mtx.unlock();
}

extern "C" __declspec(dllexport)void GetModule(array<char, 256> *str, int i) {
	mtx.lock();
	Real_memcpy(str, &moduleNames[i], 256);
	mtx.unlock();
}

extern "C" __declspec(dllexport)void GetProInfo(STARTUPINFO * ss, PROCESS_INFORMATION * pp) {
	mtx.lock();
	Real_memcpy(ss, &sui, sizeof(STARTUPINFO));
	Real_memcpy(pp, &pri, sizeof(PROCESS_INFORMATION));
	mtx.unlock();
}

extern "C" __declspec(dllexport)void GetNum(int& num) {
	mtx.lock();
	num = numbercount;
	mtx.unlock();
}

extern "C" __declspec(dllexport)void GetModuleNum(int& num) {
	mtx.lock();
	num = moduleNum;
	mtx.unlock();
}

extern "C" __declspec(dllexport)void ClearSeg() {
	mtx.lock();
	memset(&seg, 0, sizeof(seg));
	numbercount = 0;
	mtx.unlock();
}

extern "C" __declspec(dllexport)void ClearModule() {
	mtx.lock();
	memset(&moduleNames, 0, sizeof(moduleNames));
	moduleNum = 0;
	mtx.unlock();
}


static int(WINAPI* Real_MessageBoxW)(HWND hWnd,
	LPCWSTR lpText,
	LPCWSTR lpCaption,
	UINT uType)
	= MessageBoxW;

static int(WINAPI* Real_MessageBoxA)(HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType)
	= MessageBoxA;

static HANDLE(WINAPI* Real_HeapCreate)(DWORD flOptions,
	SIZE_T dwInitialSize,
	SIZE_T dwMaximumSize)
	= HeapCreate;

static BOOL(WINAPI* Real_HeapDestroy)(HANDLE hHeap)
= HeapDestroy;

static BOOL(WINAPI* Real_HeapFree)(HANDLE hHeap,
	DWORD dwFlags,
	LPVOID lpMem)
	= HeapFree;

static HANDLE(WINAPI* Real_CreateFile)(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
	= CreateFile;

static BOOL(WINAPI* Real_WriteFile)(HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
	= WriteFile;

static BOOL(WINAPI* Real_ReadFile)(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
	= ReadFile;

static LSTATUS(WINAPI* Real_RegCreateKeyEx)(HKEY hKey,
	LPCSTR lpSubKey,
	DWORD Reserved,
	LPSTR lpClass,
	DWORD dwOptions,
	REGSAM samDesired,
	const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY phkResult,
	LPDWORD lpdwDisposition)
	= RegCreateKeyEx;

static LSTATUS(WINAPI* Real_RegOpenKeyEx)(HKEY hKey,
	LPCSTR lpSubKey,
	DWORD ulOptions,
	REGSAM samDesired,
	PHKEY phkResult)
	= RegOpenKeyEx;

static LSTATUS(WINAPI* Real_RegSetValueEx)(HKEY hKey,
	LPCSTR lpValueName,
	DWORD Reserved,
	DWORD dwType,
	const BYTE* lpData,
	DWORD cbData)
	= RegSetValueEx;

static LSTATUS(WINAPI* Real_RegDeleteValue)(HKEY hKey,
	LPCSTR lpValueName)
	= RegDeleteValue;

static LSTATUS(WINAPI* Real_RegDeleteKey)(HKEY hKey,
	LPCSTR lpSubKey)
	= RegDeleteKey;

static LSTATUS(WINAPI* Real_RegCloseKey)(HKEY hKey)
= RegCloseKey;

static SOCKET(WINAPI* Real_socket)(int af,
	int type,
	int protocol)
	= socket;

static int (WINAPI* Real_bind)(SOCKET s,
	const sockaddr* name,
	int namelen)
	= bind;

static int (WINAPI* Real_send)(SOCKET     s,
	const char* buf,
	int len,
	int flags)
	= send;

static int (WINAPI* Real_connect)(
	SOCKET s,
	const sockaddr* name,
	int namelen)
	= connect;

static int (WINAPI* Real_recv)(SOCKET s,
	char* buf,
	int len,
	int flags)
	= recv;

static BOOL(WINAPI* Real_CopyFile)(LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	BOOL bFailIfExists)
	= CopyFile;

extern "C" __declspec(dllexport)void WINAPI Mine_memcpy(void*,
	const void*,
	size_t);

extern "C" __declspec(dllexport)int WINAPI Mine_MessageBoxA(HWND,
	LPCSTR,
	LPCSTR,
	UINT);

extern "C" __declspec(dllexport)int WINAPI Mine_MessageBoxW(HWND,
	LPCWSTR,
	LPCWSTR,
	UINT);

extern "C" __declspec(dllexport)HANDLE WINAPI Mine_HeapCreate(DWORD,
	SIZE_T,
	SIZE_T);

extern "C" __declspec(dllexport)BOOL WINAPI Mine_HeapDestroy(HANDLE hHeap);

extern "C" __declspec(dllexport)BOOL WINAPI Mine_HeapFree(HANDLE,
	DWORD,
	LPVOID);

extern "C" __declspec(dllexport)HANDLE WINAPI Mine_CreateFile(LPCSTR,
	DWORD,
	DWORD,
	LPSECURITY_ATTRIBUTES,
	DWORD,
	DWORD,
	HANDLE);

extern "C" __declspec(dllexport)BOOL WINAPI Mine_WriteFile(HANDLE,
	LPCVOID,
	DWORD,
	LPDWORD,
	LPOVERLAPPED);

extern "C" __declspec(dllexport)BOOL WINAPI Mine_ReadFile(HANDLE,
	LPVOID,
	DWORD,
	LPDWORD,
	LPOVERLAPPED);

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegCreateKeyEx(HKEY,
	LPCSTR,
	DWORD,
	LPSTR,
	DWORD,
	REGSAM,
	const LPSECURITY_ATTRIBUTES,
	PHKEY,
	LPDWORD);

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegOpenKeyEx(HKEY,
	LPCSTR,
	DWORD,
	REGSAM,
	PHKEY);

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegSetValueEx(HKEY,
	LPCSTR,
	DWORD,
	DWORD,
	const BYTE*,
	DWORD);

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegDeleteValue(HKEY,
	LPCSTR);

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegDeleteKey(HKEY,
	LPCSTR);

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegCloseKey(HKEY);

extern "C" __declspec(dllexport)SOCKET WINAPI Mine_socket(int,
	int,
	int);

extern "C" __declspec(dllexport)int WINAPI Mine_bind(SOCKET,
	const sockaddr*,
	int);

extern "C" __declspec(dllexport)int WINAPI Mine_send(SOCKET,
	const char*,
	int,
	int);

extern "C" __declspec(dllexport)int WINAPI Mine_connect(SOCKET,
	const sockaddr*,
	int);

extern "C" __declspec(dllexport)int WINAPI Mine_recv(SOCKET,
	char*,
	int,
	int);

extern "C" __declspec(dllexport)BOOL WINAPI Mine_CopyFile(LPCSTR,
	LPCSTR,
	BOOL);

LONG AttachDetours(VOID)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourSetIgnoreTooSmall(TRUE);

	ATTACH(MessageBoxA);
	ATTACH(MessageBoxW);
	ATTACH(HeapCreate);
	ATTACH(HeapDestroy);
	ATTACH(HeapFree);
	ATTACH(CreateFile);
	ATTACH(WriteFile);
	ATTACH(ReadFile);
	ATTACH(RegCreateKeyEx);
	ATTACH(RegOpenKeyEx);
	ATTACH(RegSetValueEx);
	ATTACH(RegDeleteValue);
	ATTACH(RegDeleteKey);
	ATTACH(RegCloseKey);
	ATTACH(socket);
	ATTACH(bind);
	ATTACH(send);
	ATTACH(connect);
	ATTACH(recv);
	ATTACH(memcpy);
	ATTACH(CopyFile);

	PVOID* ppbFailedPointer = nullptr;
	LONG error = DetourTransactionCommitEx(&ppbFailedPointer);
	if (error != 0) {
		printf("traceapi.dll: Attach transaction failed to commit. Error %ld (%p/%p)",
			error, ppbFailedPointer, *ppbFailedPointer);
		return error;
	}
	return 0;
}

LONG DetachDetours(VOID)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourSetIgnoreTooSmall(TRUE);

	DETACH(MessageBoxA);
	DETACH(MessageBoxW);
	DETACH(HeapCreate);
	DETACH(HeapDestroy);
	DETACH(HeapFree);
	DETACH(CreateFile);
	DETACH(WriteFile);
	DETACH(ReadFile);
	DETACH(RegCreateKeyEx);
	DETACH(RegOpenKeyEx);
	DETACH(RegSetValueEx);
	DETACH(RegDeleteValue);
	DETACH(RegDeleteKey);
	DETACH(RegCloseKey);
	DETACH(socket);
	DETACH(bind);
	DETACH(send);
	DETACH(connect);
	DETACH(recv);
	DETACH(memcpy);
	DETACH(CopyFile);

	if (DetourTransactionCommit() != 0) {
		PVOID* ppbFailedPointer = nullptr;
		LONG error = DetourTransactionCommitEx(&ppbFailedPointer);

		printf("traceapi.dll: Detach transaction failed to commit. Error %ld (%p/%p)",
			error, ppbFailedPointer, *ppbFailedPointer);
		return error;
	}
	return 0;
}

mmapInfo sharedInfo;

//type define
constexpr int CMESSAGEBOXW = 1;
constexpr int CMESSAGEBOXA = 2;
constexpr int CHEAPCREATE = 3;
constexpr int CHEAPDESTORY = 4;
constexpr int CHEAPFREE = 5;
constexpr int CCREATEFILE = 6;
constexpr int CWRITEFILE = 7;
constexpr int CREADFILE = 8;
constexpr int CREGCREATEKEYEX = 9;
constexpr int CREGOPENKEYEX = 10;
constexpr int CREGSETVALUEEX = 11;
constexpr int CREGDELETEVALUE = 12;
constexpr int CREGDELETEKEY = 13;
constexpr int CREGCLOSEKEY = 14;
constexpr int CSOCKET = 15;
constexpr int CBIND = 16;
constexpr int CSEND = 17;
constexpr int CCONNECT = 18;
constexpr int CRECV = 19;
constexpr int CMEMCPY = 20;
constexpr int CCopyFile = 21;

static bool entered = false;

extern "C" __declspec(dllexport)void WINAPI Mine_memcpy(void* _Dst,
	const void* _Src,
	size_t _Size)
{
	if (entered) {
		Real_memcpy(_Dst, _Src, _Size);
		return;
	}
	entered = true;
	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CMEMCPY;
	sharedInfo.argNums = 5;

	sharedInfo.argNames[0] = toArray(string("_Dst"));
	sharedInfo.argNames[1] = toArray(string("_Src"));
	sharedInfo.argNames[2] = toArray(string("_Size"));
	sharedInfo.argNames[3] = toArray(string("Dst"));
	sharedInfo.argNames[4] = toArray(string("Src"));

	int nLen = min(_Size, 99);

	sharedInfo.argValues[0] = toArray(type2str(&_Dst, 16));
	sharedInfo.argValues[1] = toArray(type2str(&_Src, 16));
	sharedInfo.argValues[2] = toArray(type2str(_Size, 16));
	sharedInfo.argValues[3] = toArray(string((char*)_Dst, nLen));
	sharedInfo.argValues[4] = toArray(string((char*)_Src, nLen));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	Real_memcpy(_Dst, _Src, _Size);
	entered = false;

	return;
}

extern "C" __declspec(dllexport)int WINAPI Mine_MessageBoxW(HWND hWnd,
	LPCWSTR lpText,
	LPCWSTR lpCaption,
	UINT uType)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CMESSAGEBOXW;
	sharedInfo.argNums = 4;
	sharedInfo.argNames[0] = toArray(string("hWnd"));
	sharedInfo.argNames[1] = toArray(string("lpText"));
	sharedInfo.argNames[2] = toArray(string("lpCaption"));
	sharedInfo.argNames[3] = toArray(string("uType"));

	sharedInfo.argValues[0] = toArray(type2str(hWnd, 16));
	sharedInfo.argValues[1] = toArray(type2str(lpText));
	sharedInfo.argValues[2] = toArray(type2str(lpCaption));
	sharedInfo.argValues[3] = toArray(type2str(uType, 8));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_MessageBoxW(nullptr, L"new MessageBoxW", L"Hooked", MB_OK);
}

extern "C" __declspec(dllexport)int WINAPI Mine_MessageBoxA(HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CMESSAGEBOXA;
	sharedInfo.argNums = 4;

	sharedInfo.argNames[0] = toArray(string("hWnd"));
	sharedInfo.argNames[1] = toArray(string("lpText"));
	sharedInfo.argNames[2] = toArray(string("lpCaption"));
	sharedInfo.argNames[3] = toArray(string("uType"));

	sharedInfo.argValues[0] = toArray(type2str(hWnd, 16));
	sharedInfo.argValues[1] = toArray(type2str(lpText));
	sharedInfo.argValues[2] = toArray(type2str(lpCaption));
	sharedInfo.argValues[3] = toArray(type2str(uType, 8));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_MessageBoxA(nullptr, "new MessageBoxA", "Hooked", MB_OK);
}

extern "C" __declspec(dllexport)HANDLE WINAPI Mine_HeapCreate(DWORD flOptions,
	SIZE_T dwInitialSize,
	SIZE_T dwMaximumSize)
{
	entered = true;

	HANDLE hFile = Real_HeapCreate(flOptions, dwInitialSize, dwMaximumSize);
	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CHEAPCREATE;
	sharedInfo.argNums = 4;

	sharedInfo.argNames[0] = toArray(string("flOptions"));
	sharedInfo.argNames[1] = toArray(string("dwInitialSize"));
	sharedInfo.argNames[2] = toArray(string("dwMaximumSize"));
	sharedInfo.argNames[3] = toArray(string("hFile"));

	sharedInfo.argValues[0] = toArray(type2str(flOptions, 8));
	sharedInfo.argValues[1] = toArray(type2str(dwInitialSize, 8));
	sharedInfo.argValues[2] = toArray(type2str(dwMaximumSize, 8));
	sharedInfo.argValues[3] = toArray(type2str(hFile, 16));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return hFile;
}

extern "C" __declspec(dllexport)BOOL WINAPI Mine_HeapDestroy(HANDLE hHeap)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CHEAPDESTORY;
	sharedInfo.argNums = 1;

	sharedInfo.argNames[0] = toArray(string("hHeap"));

	sharedInfo.argValues[0] = toArray(type2str(hHeap, 16));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_HeapDestroy(hHeap);
}

extern "C" __declspec(dllexport)BOOL WINAPI Mine_HeapFree(HANDLE hHeap,
	DWORD dwFlags,
	LPVOID lpMem)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CHEAPFREE;
	sharedInfo.argNums = 3;

	sharedInfo.argNames[0] = toArray(string("hHeap"));
	sharedInfo.argNames[1] = toArray(string("dwFlags"));
	sharedInfo.argNames[2] = toArray(string("lpMem"));

	sharedInfo.argValues[0] = toArray(type2str(hHeap, 16));
	sharedInfo.argValues[1] = toArray(type2str(dwFlags, 8));
	sharedInfo.argValues[2] = toArray(type2str(lpMem, 16));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_HeapFree(hHeap, dwFlags, lpMem);
}

extern "C" __declspec(dllexport)HANDLE WINAPI Mine_CreateFile(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	entered = true;

	HANDLE hFile = Real_CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (GetFileType(hFile) == FILE_TYPE_DISK) {
		GetLocalTime(&sharedInfo.st);
		sharedInfo.type = CCREATEFILE;
		sharedInfo.argNums = 7;
		
		sharedInfo.argNames[0] = toArray(string("lpFileName"));
		sharedInfo.argNames[1] = toArray(string("dwDesiredAccess"));
		sharedInfo.argNames[2] = toArray(string("dwShareMode"));
		sharedInfo.argNames[3] = toArray(string("lpSecurityAttributes"));
		sharedInfo.argNames[4] = toArray(string("dwCreationDisposition"));
		sharedInfo.argNames[5] = toArray(string("dwFlagsAndAttributes"));
		sharedInfo.argNames[6] = toArray(string("hTemplateFile"));

		sharedInfo.argValues[0] = toArray(type2str(lpFileName));
		sharedInfo.argValues[1] = toArray(type2str(dwDesiredAccess, 8));
		sharedInfo.argValues[2] = toArray(type2str(dwShareMode, 8));
		sharedInfo.argValues[3] = toArray(type2str(lpSecurityAttributes, 16));
		sharedInfo.argValues[4] = toArray(type2str(dwCreationDisposition, 8));
		sharedInfo.argValues[5] = toArray(type2str(dwFlagsAndAttributes, 8));
		sharedInfo.argValues[6] = toArray(type2str(hTemplateFile, 16));

		ChangeSeg(&sharedInfo);
		memset(&sharedInfo, 0, sizeof(mmapInfo));
	}

	entered = false;

	return hFile;
}

extern "C" __declspec(dllexport)BOOL WINAPI Mine_WriteFile(HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	entered = true;

	if (GetFileType(hFile) == FILE_TYPE_DISK) {
		GetLocalTime(&sharedInfo.st);
		sharedInfo.type = CWRITEFILE;
		sharedInfo.argNums = 6;

		sharedInfo.argNames[0] = toArray(string("hFile"));
		sharedInfo.argNames[1] = toArray(string("lpBuffer"));
		sharedInfo.argNames[2] = toArray(string("nNumberOfBytesToWrite"));
		sharedInfo.argNames[3] = toArray(string("lpNumberOfBytesWritten"));
		sharedInfo.argNames[4] = toArray(string("lpOverlapped"));
		sharedInfo.argNames[5] = toArray(string("buffer"));

		sharedInfo.argValues[0] = toArray(type2str(hFile, 16));
		sharedInfo.argValues[1] = toArray(type2str(lpBuffer, 16));
		sharedInfo.argValues[2] = toArray(type2str(nNumberOfBytesToWrite, 8));
		sharedInfo.argValues[3] = toArray(type2str(lpNumberOfBytesWritten, 16));
		sharedInfo.argValues[4] = toArray(type2str(lpOverlapped, 16));
		sharedInfo.argValues[5] = toArray(string((char*)lpBuffer, nNumberOfBytesToWrite));

		ChangeSeg(&sharedInfo);
		memset(&sharedInfo, 0, sizeof(mmapInfo));
	}

	entered = false;

	return Real_WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

extern "C" __declspec(dllexport)BOOL WINAPI Mine_ReadFile(HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	entered = true;

	if (GetFileType(hFile) == FILE_TYPE_DISK) {
		GetLocalTime(&sharedInfo.st);
		sharedInfo.type = CREADFILE;
		sharedInfo.argNums = 6;

		sharedInfo.argNames[0] = toArray(string("hFile"));
		sharedInfo.argNames[1] = toArray(string("lpBuffer"));
		sharedInfo.argNames[2] = toArray(string("nNumberOfBytesToRead"));
		sharedInfo.argNames[3] = toArray(string("lpNumberOfBytesRead"));
		sharedInfo.argNames[4] = toArray(string("lpOverlapped"));
		sharedInfo.argNames[5] = toArray(string("buffer"));

		sharedInfo.argValues[0] = toArray(type2str(hFile, 16));
		sharedInfo.argValues[1] = toArray(type2str(lpBuffer, 16));
		sharedInfo.argValues[2] = toArray(type2str(nNumberOfBytesToRead, 8));
		sharedInfo.argValues[3] = toArray(type2str(lpNumberOfBytesRead, 16));
		sharedInfo.argValues[4] = toArray(type2str(lpOverlapped, 16));
		sharedInfo.argValues[5] = toArray(string((char*)lpBuffer, nNumberOfBytesToRead));

		ChangeSeg(&sharedInfo);
		memset(&sharedInfo, 0, sizeof(mmapInfo));
	}

	entered = false;

	return Real_ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegCreateKeyEx(HKEY hKey,
	LPCSTR lpSubKey,
	DWORD Reserved,
	LPSTR lpClass,
	DWORD dwOptions,
	REGSAM samDesired,
	const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY phkResult,
	LPDWORD lpdwDisposition)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CREGCREATEKEYEX;
	sharedInfo.argNums = 9;

	sharedInfo.argNames[0] = toArray(string("hKey"));
	sharedInfo.argNames[1] = toArray(string("lpSubKey"));
	sharedInfo.argNames[2] = toArray(string("Reserved"));
	sharedInfo.argNames[3] = toArray(string("lpClass"));
	sharedInfo.argNames[4] = toArray(string("dwOptions"));
	sharedInfo.argNames[5] = toArray(string("samDesired"));
	sharedInfo.argNames[6] = toArray(string("lpSecurityAttributes"));
	sharedInfo.argNames[7] = toArray(string("phkResult"));
	sharedInfo.argNames[8] = toArray(string("lpdwDisposition"));

	sharedInfo.argValues[0] = toArray(type2str(hKey, 16));
	sharedInfo.argValues[1] = toArray(type2str(lpSubKey));
	sharedInfo.argValues[2] = toArray(type2str(Reserved, 8));
	sharedInfo.argValues[3] = toArray(type2str(lpClass, 16));
	sharedInfo.argValues[4] = toArray(type2str(dwOptions, 8));
	sharedInfo.argValues[5] = toArray(type2str(samDesired, 16));
	sharedInfo.argValues[6] = toArray(type2str(lpSecurityAttributes, 16));
	sharedInfo.argValues[7] = toArray(type2str(phkResult, 16));
	sharedInfo.argValues[8] = toArray(type2str(lpdwDisposition, 16));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_RegCreateKeyEx(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegOpenKeyEx(HKEY hKey,
	LPCSTR lpSubKey,
	DWORD ulOptions,
	REGSAM samDesired,
	PHKEY phkResult)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CREGOPENKEYEX;
	sharedInfo.argNums = 5;

	sharedInfo.argNames[0] = toArray(string("hKey"));
	sharedInfo.argNames[1] = toArray(string("lpSubKey"));
	sharedInfo.argNames[2] = toArray(string("ulOptions"));
	sharedInfo.argNames[3] = toArray(string("samDesired"));
	sharedInfo.argNames[4] = toArray(string("phkResult"));

	sharedInfo.argValues[0] = toArray(type2str(hKey, 16));
	sharedInfo.argValues[1] = toArray(type2str(lpSubKey));
	sharedInfo.argValues[2] = toArray(type2str(ulOptions, 8));
	sharedInfo.argValues[3] = toArray(type2str(samDesired, 16));
	sharedInfo.argValues[4] = toArray(type2str(phkResult, 16));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_RegOpenKeyEx(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegSetValueEx(HKEY hKey,
	LPCSTR lpValueName,
	DWORD Reserved,
	DWORD dwType,
	const BYTE * lpData,
	DWORD cbData)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CREGSETVALUEEX;
	sharedInfo.argNums = 6;

	sharedInfo.argNames[0] = toArray(string("hKey"));
	sharedInfo.argNames[1] = toArray(string("lpValueName"));
	sharedInfo.argNames[2] = toArray(string("Reserved"));
	sharedInfo.argNames[3] = toArray(string("dwType"));
	sharedInfo.argNames[4] = toArray(string("lpData"));
	sharedInfo.argNames[5] = toArray(string("cbData"));

	sharedInfo.argValues[0] = toArray(type2str(hKey, 16));
	sharedInfo.argValues[1] = toArray(type2str(lpValueName));
	sharedInfo.argValues[2] = toArray(type2str(Reserved, 8));
	sharedInfo.argValues[3] = toArray(type2str(dwType, 8));
	sharedInfo.argValues[4] = toArray(type2str(lpData));
	sharedInfo.argValues[5] = toArray(type2str(cbData, 8));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_RegSetValueEx(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegDeleteValue(HKEY hKey,
	LPCSTR lpValueName)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CREGDELETEVALUE;
	sharedInfo.argNums = 2;

	sharedInfo.argNames[0] = toArray(string("hKey"));
	sharedInfo.argNames[1] = toArray(string("lpValueName"));

	sharedInfo.argValues[0] = toArray(type2str(hKey, 16));
	sharedInfo.argValues[1] = toArray(type2str(lpValueName));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_RegDeleteValue(hKey, lpValueName);
}

extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegDeleteKey(HKEY hKey,
	LPCSTR lpSubKey)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CREGDELETEKEY;
	sharedInfo.argNums = 2;

	sharedInfo.argNames[0] = toArray(string("hKey"));
	sharedInfo.argNames[1] = toArray(string("lpSubKey"));

	sharedInfo.argValues[0] = toArray(type2str(hKey, 16));
	sharedInfo.argValues[1] = toArray(type2str(lpSubKey));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_RegDeleteKey(hKey, lpSubKey);
}


extern "C" __declspec(dllexport)LSTATUS WINAPI Mine_RegCloseKey(HKEY hKey)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CREGCLOSEKEY;
	sharedInfo.argNums = 1;

	sharedInfo.argNames[0] = toArray(string("hKey"));

	sharedInfo.argValues[0] = toArray(type2str(hKey, 16));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));
	entered = false;

	return Real_RegCloseKey(hKey);
}

extern "C" __declspec(dllexport)SOCKET WINAPI Mine_socket(int af,
	int type,
	int protocol)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CSOCKET;
	sharedInfo.argNums = 3;

	sharedInfo.argNames[0] = toArray(string("af"));
	sharedInfo.argNames[1] = toArray(string("type"));
	sharedInfo.argNames[2] = toArray(string("protocol"));

	sharedInfo.argValues[0] = toArray(type2str(af, 8));
	sharedInfo.argValues[1] = toArray(type2str(type, 8));
	sharedInfo.argValues[2] = toArray(type2str(protocol, 8));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_socket(af, type, protocol);
}

extern "C" __declspec(dllexport)int WINAPI Mine_bind(SOCKET s,
	const sockaddr * name,
	int namelen)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CBIND;
	sharedInfo.argNums = 5;

	sharedInfo.argNames[0] = toArray(string("s"));
	sharedInfo.argNames[1] = toArray(string("name"));
	sharedInfo.argNames[2] = toArray(string("namelen"));
	sharedInfo.argNames[3] = toArray(string("IP"));
	sharedInfo.argNames[4] = toArray(string("port"));

	sockaddr_in* sockets = (sockaddr_in*)name;

	sharedInfo.argValues[0] = toArray(type2str(s, 16));
	sharedInfo.argValues[1] = toArray(type2str(name, 16));
	sharedInfo.argValues[2] = toArray(type2str(namelen, 8));
	sharedInfo.argValues[3] = toArray(type2str(inet_ntoa(sockets->sin_addr)));
	sharedInfo.argValues[4] = toArray(type2str(ntohs(sockets->sin_port)));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_bind(s, name, namelen);
}

extern "C" __declspec(dllexport)int WINAPI Mine_send(SOCKET s,
	const char* buf,
	int len,
	int flags)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CSEND;
	sharedInfo.argNums = 5;

	sharedInfo.argNames[0] = toArray(string("s"));
	sharedInfo.argNames[1] = toArray(string("buf"));
	sharedInfo.argNames[2] = toArray(string("len"));
	sharedInfo.argNames[3] = toArray(string("flags"));
	sharedInfo.argNames[4] = toArray(string("buffer"));

	sharedInfo.argValues[0] = toArray(type2str(s, 16));
	sharedInfo.argValues[1] = toArray(type2str(buf));
	sharedInfo.argValues[2] = toArray(type2str(len, 8));
	sharedInfo.argValues[3] = toArray(type2str(flags, 8));
	sharedInfo.argValues[4] = toArray(string(buf));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_send(s, buf, len, flags);
}

extern "C" __declspec(dllexport)int WINAPI Mine_connect(SOCKET s,
	const sockaddr * name,
	int namelen)
{
	entered = true;

	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CCONNECT;
	sharedInfo.argNums = 5;

	sharedInfo.argNames[0] = toArray(string("s"));
	sharedInfo.argNames[1] = toArray(string("name"));
	sharedInfo.argNames[2] = toArray(string("namelen"));
	sharedInfo.argNames[3] = toArray(string("IP"));
	sharedInfo.argNames[4] = toArray(string("port"));

	sockaddr_in* sockets = (sockaddr_in*)name;

	sharedInfo.argValues[0] = toArray(type2str(s, 16));
	sharedInfo.argValues[1] = toArray(type2str(name, 16));
	sharedInfo.argValues[2] = toArray(type2str(namelen, 8));
	sharedInfo.argValues[3] = toArray(type2str(inet_ntoa(sockets->sin_addr)));
	sharedInfo.argValues[4] = toArray(type2str(ntohs(sockets->sin_port)));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return Real_connect(s, name, namelen);
}

extern "C" __declspec(dllexport)int WINAPI Mine_recv(SOCKET s,
	char* buf,
	int len,
	int flags)
{
	entered = true;

	int ret = Real_recv(s, buf, len, flags);
	Sleep(10);
	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CRECV;
	sharedInfo.argNums = 5;

	sharedInfo.argNames[0] = toArray(string("s"));
	sharedInfo.argNames[1] = toArray(string("buf"));
	sharedInfo.argNames[2] = toArray(string("len"));
	sharedInfo.argNames[3] = toArray(string("flags"));
	sharedInfo.argNames[4] = toArray(string("buffer"));

	sharedInfo.argValues[0] = toArray(type2str(s, 16));
	sharedInfo.argValues[1] = toArray(type2str(buf));
	sharedInfo.argValues[2] = toArray(type2str(len, 8));
	sharedInfo.argValues[3] = toArray(type2str(flags, 8));
	sharedInfo.argValues[4] = toArray(string(buf, ret));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return ret;
}

extern "C" __declspec(dllexport)BOOL WINAPI Mine_CopyFile(LPCSTR lpExistingFileName,
	LPCSTR lpNewFileName,
	BOOL bFailIfExists)
{
	entered = true;

	int ret = Real_CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);
	GetLocalTime(&sharedInfo.st);
	sharedInfo.type = CCopyFile;
	sharedInfo.argNums = 3;

	sharedInfo.argNames[0] = toArray(string("lpExistingFileName"));
	sharedInfo.argNames[1] = toArray(string("lpNewFileName"));
	sharedInfo.argNames[2] = toArray(string("bFailIfExists"));

	sharedInfo.argValues[0] = toArray(type2str(lpExistingFileName));
	sharedInfo.argValues[1] = toArray(type2str(lpNewFileName));
	sharedInfo.argValues[2] = toArray(type2str(bFailIfExists, 1));

	ChangeSeg(&sharedInfo);
	memset(&sharedInfo, 0, sizeof(mmapInfo));

	entered = false;

	return ret;
}

BOOL WINAPI DllMain(HMODULE hModule,
	DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		AttachDetours();
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		DetachDetours();

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}
	return TRUE;
}