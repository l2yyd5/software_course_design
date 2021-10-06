#include <stdio.h>
#include <Windows.h>
#include <psapi.h>
#include <tchar.h>
#include <detours.h>

#define DLL_NAME _T("dllsample.dll")

typedef void(*ptrChangeProInfo)(STARTUPINFO* , PROCESS_INFORMATION* );
typedef void(*ptrChangeModuleInfo)(char*);
typedef void(*ptrGetNum)(int&);
typedef void(*ptrClearSeg)();
typedef void(*ptrClearModule)();
HMODULE hMod = LoadLibrary("E:\\homework\\detours-cmake-master\\build\\bin\\DEBUG\\dllsample.dll");
ptrChangeProInfo ChangeProInfo = (ptrChangeProInfo)GetProcAddress(hMod, "ChangeProInfo");
ptrChangeModuleInfo ChangeModule = (ptrChangeModuleInfo)GetProcAddress(hMod, "ChangeModule");
ptrClearSeg ClearSeg = (ptrClearSeg)GetProcAddress(hMod, "ClearSeg");
ptrClearModule ClearModule = (ptrClearModule)GetProcAddress(hMod, "ClearModule");
ptrGetNum GetNum = (ptrGetNum)GetProcAddress(hMod, "GetNum");

//-------------------------------------------------------------------------
int main(int argc, char** argv)
{
    char szExeFileBaseName[MAX_PATH];
    char szExeFileName[MAX_PATH];
    char tmpFileName[MAX_PATH + 1];
    char exe[MAX_PATH] = "dummy.exe";
    memset(szExeFileBaseName, 0, sizeof(szExeFileBaseName));
    memset(szExeFileName, 0, sizeof(szExeFileName));
    memset(tmpFileName, 0, sizeof(tmpFileName));
    memset(exe, 0, sizeof(exe));

    GetModuleFileName(nullptr, szExeFileName, MAX_PATH);
    strcpy(tmpFileName, szExeFileName);

    char* pbuf = nullptr;
    char* szLine = strtok_s(tmpFileName, "\\", &pbuf);
    while (nullptr != szLine)
    {
        strcpy_s(szExeFileBaseName, szLine);
        szLine = strtok_s(nullptr, "\\", &pbuf);
    }

    _tprintf("%s\n", szExeFileName);
    _tprintf("%s\n", szExeFileBaseName);

    if (argc == 0) {
        _tprintf("argv error!\n");
        return 0;
    }
    if (argc == 1) {
        if (strcmp(szExeFileName, argv[0]) == 0 || (strcmp(szExeFileBaseName, argv[0]) == 0)) {
            _tprintf("argv error!\n");
            return 0;
        }
        memcpy(exe, argv[0], strlen(argv[0]));
    }
    if (argc == 2) {
        memcpy(exe, argv[1], strlen(argv[1]));
    }

    _tprintf("%s\n", exe);
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;

    TCHAR BinDir[MAX_PATH];
    GetModuleFileName(nullptr, BinDir, _countof(BinDir));
    *_tcsrchr(BinDir, '\\') = '\0';
    SetCurrentDirectory(BinDir);

    BOOL ok = DetourCreateProcessWithDll(
        exe, 
        nullptr, 
        nullptr, 
        nullptr, 
        FALSE, 
        0, 
        nullptr, 
        nullptr, 
        &si, 
        &pi, 
        DLL_NAME, 
        nullptr);
    int moduleNum = 0;
    if (ok) {
        ClearModule();
        ClearSeg();
        Sleep(500);
        ChangeProInfo(&si, &pi);
        HMODULE hMod[100];
        DWORD cbNeeded;
        char moduleName[256];
        if (EnumProcessModulesEx(pi.hProcess, hMod, sizeof(hMod), &cbNeeded, LIST_MODULES_64BIT)) {
            moduleNum = cbNeeded / sizeof(HMODULE);
            for (int i = 0; i < moduleNum; i++) {
                memset(moduleName, 0, sizeof(moduleName));
                GetModuleBaseNameA(pi.hProcess, hMod[i], moduleName, sizeof(moduleName));
                ChangeModule(moduleName);
            }
        }
        Sleep(2000);
    }
    int num = 0;
    GetNum(num);
    _tprintf("%d\n", num);

    _tprintf("%d\n", moduleNum);

    _tprintf("%d\n", GetLastError());

    _tprintf("Withdll: running the dummy program with " DLL_NAME "...%s\n", ok ? _T("ok!") : _T("failed!"));

    return 0;
}