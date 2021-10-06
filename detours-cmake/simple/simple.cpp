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
    for (int i = 0; i < temp.length(); i++) {
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
        return;
    }
    else {
        Real_memcpy(&moduleNames[moduleNum++], modulename, 256);
    }
    mtx.unlock();
}

extern "C" __declspec(dllexport)void ChangeSeg(mmapInfo * str) {
    mtx.lock();
    if (numbercount >= 1000) {
        return;
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

extern "C" __declspec(dllexport)void Mine_memcpy(void*,
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

extern "C" __declspec(dllexport)HANDLE Mine_CreateFile(LPCSTR,
    DWORD,
    DWORD,
    LPSECURITY_ATTRIBUTES,
    DWORD,
    DWORD,
    HANDLE);

extern "C" __declspec(dllexport)BOOL Mine_WriteFile(HANDLE,
    LPCVOID,
    DWORD,
    LPDWORD,
    LPOVERLAPPED);

extern "C" __declspec(dllexport)BOOL Mine_ReadFile(HANDLE,
    LPVOID,
    DWORD,
    LPDWORD,
    LPOVERLAPPED);

extern "C" __declspec(dllexport)LSTATUS Mine_RegCreateKeyEx(HKEY,
    LPCSTR,
    DWORD,
    LPSTR,
    DWORD,
    REGSAM,
    const LPSECURITY_ATTRIBUTES,
    PHKEY,
    LPDWORD);

extern "C" __declspec(dllexport)LSTATUS Mine_RegOpenKeyEx(HKEY,
    LPCSTR,
    DWORD,
    REGSAM,
    PHKEY);

extern "C" __declspec(dllexport)LSTATUS Mine_RegSetValueEx(HKEY,
    LPCSTR,
    DWORD,
    DWORD,
    const BYTE*,
    DWORD);

extern "C" __declspec(dllexport)LSTATUS Mine_RegDeleteValue(HKEY,
    LPCSTR);

extern "C" __declspec(dllexport)LSTATUS Mine_RegDeleteKey(HKEY,
    LPCSTR);

extern "C" __declspec(dllexport)LSTATUS Mine_RegCloseKey(HKEY);

extern "C" __declspec(dllexport)SOCKET Mine_socket(int,
    int,
    int);

extern "C" __declspec(dllexport)int Mine_bind(SOCKET,
    const sockaddr*,
    int);

extern "C" __declspec(dllexport)int Mine_send(SOCKET,
    const char*,
    int,
    int);

extern "C" __declspec(dllexport)int Mine_connect(SOCKET,
    const sockaddr*,
    int);

extern "C" __declspec(dllexport)int Mine_recv(SOCKET,
    char*,
    int,
    int);

extern "C" __declspec(dllexport)BOOL Mine_CopyFile(LPCSTR,
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
        PVOID* ppbFailedPointer = NULL;
        LONG error = DetourTransactionCommitEx(&ppbFailedPointer);

        printf("traceapi.dll: Detach transaction failed to commit. Error %ld (%p/%p)",
            error, ppbFailedPointer, *ppbFailedPointer);
        return error;
    }
    return 0;
}

mmapInfo sharedInfo;

//type define
const int constMESSAGEBOXW = 1;
const int constMESSAGEBOXA = 2;
const int constHEAPCREATE = 3;
const int constHEAPDESTORY = 4;
const int constHEAPFREE = 5;
const int constCREATEFILE = 6;
const int constWRITEFILE = 7;
const int constREADFILE = 8;
const int constREGCREATEKEYEX = 9;
const int constREGOPENKEYEX = 10;
const int constREGSETVALUEEX = 11;
const int constREGDELETEVALUE = 12;
const int constREGDELETEKEY = 13;
const int constREGCLOSEKEY = 14;
const int constSOCKET = 15;
const int constBIND = 16;
const int constSEND = 17;
const int constCONNECT = 18;
const int constRECV = 19;
const int constMEMCPY = 20;
const int constCopyFile = 21;

static bool entered = false;

extern "C" __declspec(dllexport)void Mine_memcpy(void* _Dst,
    const void* _Src,
    size_t _Size)
{
    if (entered) {
        Real_memcpy(_Dst, _Src, _Size);
        return;
    }
    entered = true;
    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constMEMCPY;
    sharedInfo.argNums = 5;

    sharedInfo.argNames[0] = toArray(string("_Dst"));
    sharedInfo.argNames[1] = toArray(string("_Src"));
    sharedInfo.argNames[2] = toArray(string("_Size"));
    sharedInfo.argNames[3] = toArray(string("Dst"));
    sharedInfo.argNames[4] = toArray(string("Src"));

    sharedInfo.argValues[0] = toArray(type2str(&_Dst, 16));
    sharedInfo.argValues[1] = toArray(type2str(&_Src, 16));
    sharedInfo.argValues[2] = toArray(type2str(_Size, 16));
    sharedInfo.argValues[3] = toArray(string((char*)_Dst, _Size));
    sharedInfo.argValues[4] = toArray(string((char*)_Src, _Size));

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
    sharedInfo.type = constMESSAGEBOXW;
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
    sharedInfo.type = constMESSAGEBOXA;
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
    sharedInfo.type = constHEAPCREATE;
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
    sharedInfo.type = constHEAPDESTORY;
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
    sharedInfo.type = constHEAPFREE;
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

extern "C" __declspec(dllexport)HANDLE Mine_CreateFile(LPCSTR lpFileName,
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
        sharedInfo.type = constCREATEFILE;;
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

extern "C" __declspec(dllexport)BOOL Mine_WriteFile(HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped)
{
    entered = true;

    if (GetFileType(hFile) == FILE_TYPE_DISK) {
        GetLocalTime(&sharedInfo.st);
        sharedInfo.type = constWRITEFILE;;
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

extern "C" __declspec(dllexport)BOOL Mine_ReadFile(HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped)
{
    entered = true;

    if (GetFileType(hFile) == FILE_TYPE_DISK) {
        GetLocalTime(&sharedInfo.st);
        sharedInfo.type = constREADFILE;
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

extern "C" __declspec(dllexport)LSTATUS Mine_RegCreateKeyEx(HKEY hKey,
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
    sharedInfo.type = constREGCREATEKEYEX;
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

extern "C" __declspec(dllexport)LSTATUS Mine_RegOpenKeyEx(HKEY hKey,
    LPCSTR lpSubKey,
    DWORD ulOptions,
    REGSAM samDesired,
    PHKEY phkResult)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constREGOPENKEYEX;
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

extern "C" __declspec(dllexport)LSTATUS Mine_RegSetValueEx(HKEY hKey,
    LPCSTR lpValueName,
    DWORD Reserved,
    DWORD dwType,
    const BYTE * lpData,
    DWORD cbData)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constREGSETVALUEEX;
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

extern "C" __declspec(dllexport)LSTATUS Mine_RegDeleteValue(HKEY hKey,
    LPCSTR lpValueName)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constREGDELETEVALUE;
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

extern "C" __declspec(dllexport)LSTATUS Mine_RegDeleteKey(HKEY hKey,
    LPCSTR lpSubKey)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constREGDELETEKEY;
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


extern "C" __declspec(dllexport)LSTATUS Mine_RegCloseKey(HKEY hKey)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constREGCLOSEKEY;
    sharedInfo.argNums = 1;

    sharedInfo.argNames[0] = toArray(string("hKey"));

    sharedInfo.argValues[0] = toArray(type2str(hKey, 16));

    ChangeSeg(&sharedInfo);
    memset(&sharedInfo, 0, sizeof(mmapInfo));
    entered = false;

    return Real_RegCloseKey(hKey);
}

extern "C" __declspec(dllexport)SOCKET Mine_socket(int af,
    int type,
    int protocol)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constSOCKET;
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

extern "C" __declspec(dllexport)int Mine_bind(SOCKET s,
    const sockaddr * name,
    int namelen)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constBIND;
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

extern "C" __declspec(dllexport)int Mine_send(SOCKET s,
    const char* buf,
    int len,
    int flags)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constSEND;
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

extern "C" __declspec(dllexport)int Mine_connect(SOCKET s,
    const sockaddr * name,
    int namelen)
{
    entered = true;

    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constCONNECT;
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

extern "C" __declspec(dllexport)int Mine_recv(SOCKET s,
    char* buf,
    int len,
    int flags)
{
    entered = true;

    int ret = Real_recv(s, buf, len, flags);
    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constRECV;
    sharedInfo.argNums = 5;

    sharedInfo.argNames[0] = toArray(string("s"));
    sharedInfo.argNames[1] = toArray(string("buf"));
    sharedInfo.argNames[2] = toArray(string("len"));
    sharedInfo.argNames[3] = toArray(string("flags"));
    sharedInfo.argNames[4] = toArray(string("buffer"));

    sharedInfo.argValues[0] = toArray(type2str(s, 16));
    sharedInfo.argValues[1] = toArray(type2str(buf, ret));
    sharedInfo.argValues[2] = toArray(type2str(len, 8));
    sharedInfo.argValues[3] = toArray(type2str(flags, 8));
    sharedInfo.argValues[4] = toArray(string(buf, ret));

    ChangeSeg(&sharedInfo);
    memset(&sharedInfo, 0, sizeof(mmapInfo));

    entered = false;

    return ret;
}

extern "C" __declspec(dllexport)BOOL Mine_CopyFile(LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName,
    BOOL bFailIfExists)
{
    entered = true;

    int ret = Real_CopyFile(lpExistingFileName, lpNewFileName, bFailIfExists);
    GetLocalTime(&sharedInfo.st);
    sharedInfo.type = constCopyFile;
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

void key() {
    HKEY hKey = nullptr;
    std::string tmpOutput("Start Create Reg Key!\r\n");

    size_t Rvl1 = RegCreateKeyEx(HKEY_CURRENT_USER, "testRegKey", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    if (Rvl1 != ERROR_SUCCESS)
        return;


    char value[255] = "testlzyyds";
    size_t vLen = strlen(value);

    size_t Rvl2 = RegSetValueEx(hKey, "lzlz", 0, REG_SZ, (CONST BYTE*)value, sizeof(wchar_t) * vLen);
    if (Rvl2 != ERROR_SUCCESS)
        return;

    RegCloseKey(hKey);
}

void deletekey() {
    HKEY hKey = nullptr;

    size_t Rvl1 = RegOpenKeyEx(HKEY_CURRENT_USER, "testRegKey", 0, KEY_ALL_ACCESS, &hKey);

    if (Rvl1 != ERROR_SUCCESS)
        return;


    size_t Rvl2 = RegDeleteValue(hKey, "lzlz");
    if (Rvl2 != ERROR_SUCCESS)
        return;

    RegCloseKey(hKey);

    size_t Rvl3 = RegDeleteKey(HKEY_CURRENT_USER, "testRegKey");
    if (Rvl3 != ERROR_SUCCESS)
        return;
}

void so() {
    // TODO: 在此添加控件通知处理程序代码
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return;
    }
    //创建套接字
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSock == INVALID_SOCKET)
    {
        return;
    }

    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    //inet_pton(AF_INET, "127.0.0.1", &sockAddr.sin_addr.s_addr);  //具体的IP地址
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(56666);  //端口
    if (bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR))) {
        return;
    }

    //进入监听状态
    if (listen(servSock, 20) == SOCKET_ERROR) {
        return;
    }

    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    char recvData[101];
    //memset(recvData, 0, sizeof(recvData));
    SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
    if (clntSock == INVALID_SOCKET)
    {
        return;
    }

    int ret = recv(clntSock, recvData, 100, 0);

    if (ret > 0 && ret < 100)
    {
        recvData[ret] = 0x00;
    }

    closesocket(clntSock);
    closesocket(servSock);
    WSACleanup();
}

#define PAGE_SIZE	4096
void menu();
void heap_create_destroy();
void create_and_write_file();
void read_file();
void reg_create_and_set_value();
void reg_open_and_del_value();
void heap_repeated_release();
void change_executable_program();
void self_replication();
void open_another_folder();
void modify_startup_registry();
void socket_recv_data();
void socket_send_data();
void copytest();

int main() {
    AttachDetours();
    //MessageBoxA(GetActiveWindow(), "new MessageBoxA", "test", MB_OK);
    //key();
    //deletekey();
    so();

    int choice;
    int mark = 0;
    while (false)
    {
        menu();
        scanf_s("%d", &choice);
        switch (choice)
        {
        case 1:
            MessageBoxA(NULL, "I'm your father!", "wsxk", MB_OK);
            break;
        case 2:
            MessageBoxW(NULL, L"test", L"wsxk", MB_OK);
            break;
        case 3:
            heap_create_destroy();
            break;
        case 4:
            create_and_write_file();
            break;
        case 5:
            read_file();
            break;
        case 6:
            reg_create_and_set_value();
            break;
        case 7:
            reg_open_and_del_value();
            break;
        case 8:
            heap_repeated_release();
            break;
        case 9:
            change_executable_program();
            break;
        case 10:
            self_replication();
            break;
        case 11:
            open_another_folder();
            break;
        case 12:
            modify_startup_registry();
            break;
        case 13:
            socket_recv_data();
            break;
        case 14:
            socket_send_data();
            break;
        case 15:
            copytest();
            break;
        case 16:
            mark = 1;
            break;
        default:
            break;
        }
        if (mark)
            break;
    }
    printf("See you!\n");

    DetachDetours();

    return 0;
}

void menu() {
    printf("--------------------------------please enter your choice--------------------------------\n");
    printf("---operations---:\n");
    printf("1.MessageBoxA      2.MessageBoxW            3.HeapCreateAndDestory   4.CreateAndWriteFile\n");
    printf("5.ReadFileString   6.RegCreateAndSetValue   7.RegOpenAndDelValue     8.HeapRepeatedRelease\n");
    printf("9.ChangeExecutableProgram   10.SelfReplication   11.OpenAnotherFolder\n");
    printf("12.ModifyStartupRegistry 13.SocketRecvData 14.SocketSendData\n");
    printf("15: exit\n");
}

void heap_create_destroy() {
    HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, PAGE_SIZE * 10, PAGE_SIZE * 100);
    if (!hHeap) {
        printf("create error!\n");
        return;
    }
    int* heap = (int*)HeapAlloc(hHeap, 0, sizeof(int) * 10);
    if (!heap) {
        printf("alloc error!\n");
        return;
    }
    for (int i = 0; i < 10; i++) {
        heap[i] = i;
    }
    for (int i = 0; i < 10; i++) {
        printf("%d ", heap[i]);
    }
    printf("\n");
    HeapFree(hHeap, 0, heap);
    HeapDestroy(hHeap);
}

void create_and_write_file() {
    HANDLE hOpenFile = (HANDLE)CreateFile("wsxk.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
    if (hOpenFile == INVALID_HANDLE_VALUE) {
        printf("create error!\n");
        return;
    }
    printf("just for for convenience,we put string 'wsxk' into file\n");
    char write_string[5] = "wsxk";
    int flag;
    flag = WriteFile(hOpenFile, write_string, sizeof(write_string), NULL, NULL);
    FlushFileBuffers(hOpenFile);
    CloseHandle(hOpenFile);
    return;
}

void read_file() {
    HANDLE hOpenFile = (HANDLE)CreateFile("wsxk.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
    if (hOpenFile == INVALID_HANDLE_VALUE)
    {
        hOpenFile = NULL;
        printf("Can not open the file\n");
        return;
    }
    int file_size = GetFileSize(hOpenFile, NULL);
    char* file_buffer = (char*)malloc(((long long)(file_size)+1) * sizeof(char));
    if (file_buffer == nullptr) {
        return;
    }
    int flag = ReadFile(hOpenFile, file_buffer, file_size, NULL, NULL);
    file_buffer[file_size] = 0;
    if (flag) {
        printf("%s\n", file_buffer);
    }
    free(file_buffer);
    CloseHandle(hOpenFile);
}

void reg_create_and_set_value() {
    HKEY hKey = NULL;
    size_t lRet = RegCreateKeyEx(HKEY_CURRENT_USER, "wsxk", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    if (lRet != ERROR_SUCCESS) {
        printf("create reg error!\n");
        return;
    }
    char value[255] = "https://github.com/wsxk";
    size_t size = strlen(value);
    lRet = RegSetValueEx(hKey, "wxk", 0, REG_SZ, (CONST BYTE*)value, sizeof(TCHAR) * size);
    if (lRet == ERROR_SUCCESS)
    {
        printf("set value successfully!\n");
        return;
    }
    else {
        printf("failed to set value!\n");
    }
    RegCloseKey(hKey);
}

void reg_open_and_del_value() {
    HKEY hKey = NULL;
    size_t lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "wsxk", 0, KEY_ALL_ACCESS, &hKey);
    if (lRet == ERROR_SUCCESS) {
        printf("open successfully!\n");
    }
    else {
        printf("open failed\n");
        return;
    }
    lRet = RegDeleteValue(hKey, "wxk");
    if (lRet == ERROR_SUCCESS) {
        printf("delete success!\n");
    }
    else {
        printf("delete fail!\n");
    }
    RegCloseKey(hKey);
    return;
}

void heap_repeated_release() {
    HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, PAGE_SIZE * 10, PAGE_SIZE * 100);
    if (!hHeap) {
        printf("create error!\n");
        return;
    }
    int* heap = (int*)HeapAlloc(hHeap, 0, sizeof(int) * 10);
    if (heap == nullptr) {
        return;
    }
    memset(heap, 0, sizeof(heap));
    if (!heap) {
        printf("alloc error!\n");
        return;
    }
    for (int i = 0; i < 10; i++) {
        heap[i] = i;
    }
    for (int i = 0; i < 10; i++) {
        printf("%d ", heap[i]);
    }
    printf("\n");
    printf("first free!\n");
    HeapFree(hHeap, 0, heap);
    printf("second free!\n");
    HeapFree(hHeap, 0, heap);
    HeapDestroy(hHeap);
    return;
}

void change_executable_program() {
    HANDLE hOpenFile = (HANDLE)CreateFile("fun.exe", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
    CloseHandle(hOpenFile);
}

void self_replication() {
    //HANDLE hOpenFile = (HANDLE)CreateFile("test_program.exe", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
    //CloseHandle(hOpenFile);
    LPCSTR targetPath = "./test/test.exe";
    TCHAR tcBuf[_MAX_PATH * 2];
    GetModuleFileName(NULL, tcBuf, sizeof(tcBuf));
    CopyFile(tcBuf, targetPath, FALSE);
}

void open_another_folder() {
    HANDLE hOpenFile = (HANDLE)CreateFile("testFolder\\wxk.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
    if (hOpenFile == INVALID_HANDLE_VALUE) {
        printf("create error!\n");
        return;
    }
    printf("just for for convenience,we put string 'wsxk' into file\n");
    char write_string[5] = "wsxk";
    int flag;
    flag = WriteFile(hOpenFile, write_string, sizeof(write_string), NULL, NULL);
    FlushFileBuffers(hOpenFile);
    CloseHandle(hOpenFile);
}

void modify_startup_registry() {
    HKEY hKey = NULL;
    size_t lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey);
    if (lRet == ERROR_SUCCESS) {
        printf("open successfully!\n");
    }
    else {
        printf("open failed\n");
    }
    char value[255] = "C:\\Users\\wsxk\\Desktop\\software_course_design\\injector\\x64\\Debug\\test_program.exe";
    size_t size = strlen(value);
    lRet = RegSetValueEx(hKey, "wxk", 0, REG_SZ, (CONST BYTE*)value, sizeof(TCHAR) * size);
    if (lRet == ERROR_SUCCESS)
    {
        printf("set value successfully!\n");
        return;
    }
    else {
        printf("failed to set value!\n");
    }
    RegCloseKey(hKey);
}

void socket_recv_data() {
    //开局必备
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //向服务器发起请求
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(6666);
    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //Sleep(500);
    //接收服务器传回的数据
    char szBuffer[MAXBYTE] = { 0 };
    recv(sock, szBuffer, MAXBYTE, NULL);
    //输出接收到的数据
    printf("Message form server: %s\n", szBuffer);
    //关闭套接字
    closesocket(sock);
    //终止使用 DLL
    WSACleanup();
}

void socket_send_data() {
    //使用socket之前必须调用WSAStartup
    WSADATA wsaData;//2.2版本
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("start wsa error!\n");
        return;
    }
    //创建套接字 服务端
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //绑定套接字
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    sockAddr.sin_port = htons(6666);  //端口
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //进入监听状态
    listen(servSock, 20);
    //接收客户端请求 连接请求
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
    //向客户端发送数据
    char str[32] = "Hello my son!";
    send(clntSock, str, strlen(str) + sizeof(char), (size_t)NULL);
    printf("send successfully!\n");
    //关闭套接字
    closesocket(clntSock);
    closesocket(servSock);
    //终止 DLL 的使用
    WSACleanup();
}

void copytest() 
{
    char temp[100] = "";
    memcpy(temp, "hello\n", 6);
    memcpy(temp, "hello\n", 6);
    memcpy(temp, "hello\n", 6);

    memmove(temp, "world\n", 6);
}