//Создать удалённый процесс
//Загрузив в  него библиотеку

/******************************************************************************
Module: InjLib.cpp
Notices: Copyright (c) 2008 Jeffrey Richter & Christophe Nasarre
******************************************************************************/
#include <windowsx.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <malloc.h> // For alloca
#include <TlHelp32.h>
#include <StrSafe.h>
#include <errhandlingapi.h>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
#ifdef UNICODE
#define InjectLib InjectLibW
#define EjectLib EjectLibW
#else
#define InjectLib InjectLibA
#define EjectLib EjectLibA
#endif // !UNICODE
///////////////////////////////////////////////////////////////////////////////
using namespace std;

BOOL WINAPI InjectLibW(DWORD dwProcessId, PCWSTR pszLibFile) {
	printf("Hello from inject\n");
	printf("1. Error code:  %d\n",GetLastError());
	BOOL bOk = FALSE; // Assume that the function fails
	HANDLE hProcess = NULL, hThread = NULL;
	PWSTR pszLibFileRemote = NULL;
	printf("2. Error code:  %d\n", GetLastError());
	printf("dwProcessId is: %d\n", dwProcessId);
	__try {
		// Get a handle for the target process.
		hProcess = OpenProcess(
			PROCESS_ALL_ACCESS,
			FALSE, dwProcessId);
		printf("3. Error code:  %d\n", GetLastError());
		if (hProcess == NULL) {
			printf("Error 1!!!!!"); __leave;
		}
		// Calculate the number of bytes needed for the DLL's pathname
		int cch = 1 + lstrlenW(pszLibFile);
		int cb = cch * sizeof(wchar_t);
		// Allocate space in the remote process for the pathname
		pszLibFileRemote = (PWSTR) VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
		if (pszLibFileRemote == NULL) { printf("Error 1!!!!!"); __leave; }
		// Copy the DLL's pathname to the remote process' address space
		if (!WriteProcessMemory(hProcess, pszLibFileRemote,
			(PVOID)pszLibFile, cb, NULL)) {
			printf("Error 1!!!!!");  __leave;
		}
		// Get the real address of LoadLibraryW in Kernel32.dll
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
		if (pfnThreadRtn == NULL) { printf("Error 1!!!!!"); __leave; }
			// Create a remote thread that calls LoadLibraryW(DLLPathname)
			hThread = CreateRemoteThread(hProcess, NULL, 0,
				pfnThreadRtn, pszLibFileRemote, 0, NULL);
			if (hThread == NULL) {
				printf("Error 1!!!!!"); __leave;
			}
		// Wait for the remote thread to terminate
		WaitForSingleObject(hThread, INFINITE);
		bOk = TRUE; // Everything executed successfully
	}
	__finally { // Now, we can clean everything up
		// Free the remote memory that contained the DLL's pathname
		if (pszLibFileRemote != NULL)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);
		if (hThread != NULL)
			CloseHandle(hThread);
		if (hProcess != NULL)
			CloseHandle(hProcess);
	}
	return(bOk);
}
///////////////////////////////////////////////////////////////////////////////
BOOL WINAPI InjectLibA(DWORD dwProcessId, PCSTR pszLibFile) {
	// Allocate a (stack) buffer for the Unicode version of the pathname
	SIZE_T cchSize = lstrlenA(pszLibFile) + 1;
	PWSTR pszLibFileW = (PWSTR)
		_alloca(cchSize * sizeof(wchar_t));
	// Convert the ANSI pathname to its Unicode equivalent
	StringCchPrintfW(pszLibFileW, cchSize, L"%S", pszLibFile);
	// Call the Unicode version of the function to actually do the work.
	return(InjectLibW(dwProcessId, pszLibFileW));
}
///////////////////////////////////////////////////////////////////////////////
BOOL WINAPI EjectLibW(DWORD dwProcessId, PCWSTR pszLibFile) {
		BOOL bOk = FALSE; // Assume that the function fails
	HANDLE hthSnapshot = NULL;
	HANDLE hProcess = NULL, hThread = NULL;
	__try {
		// Grab a new snapshot of the process
		hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
		if (hthSnapshot == INVALID_HANDLE_VALUE) __leave;
		// Get the HMODULE of the desired library
		MODULEENTRY32W me = { sizeof(me) };
		BOOL bFound = FALSE;
		BOOL bMoreMods = Module32FirstW(hthSnapshot, &me);
		for (; bMoreMods; bMoreMods = Module32NextW(hthSnapshot, &me)) {
			bFound = (_wcsicmp(me.szModule, pszLibFile) == 0) ||
				(_wcsicmp(me.szExePath, pszLibFile) == 0);
			if (bFound) break;
		}
		if (!bFound) __leave;
		// Get a handle for the target process.
		hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION |
			PROCESS_CREATE_THREAD |
			PROCESS_VM_OPERATION, // For CreateRemoteThread
			FALSE, dwProcessId);
		if (hProcess == NULL) __leave;
		// Get the real address of FreeLibrary in Kernel32.dll
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "FreeLibrary");
		if (pfnThreadRtn == NULL) __leave;
		// Create a remote thread that calls FreeLibrary()
		hThread = CreateRemoteThread(hProcess, NULL, 0,
			pfnThreadRtn, me.modBaseAddr, 0, NULL);
		if (hThread == NULL) __leave;
		// Wait for the remote thread to terminate
		WaitForSingleObject(hThread, INFINITE);
		bOk = TRUE; // Everything executed successfully
	}
	__finally { // Now we can clean everything up
		if (hthSnapshot != NULL)
			CloseHandle(hthSnapshot);
		if (hThread != NULL)
			CloseHandle(hThread);
		if (hProcess != NULL)
			CloseHandle(hProcess);
	}
	return(bOk);
}
///////////////////////////////////////////////////////////////////////////////
BOOL WINAPI EjectLibA(DWORD dwProcessId, PCSTR pszLibFile) {
	// Allocate a (stack) buffer for the Unicode version of the pathname
	SIZE_T cchSize = lstrlenA(pszLibFile) + 1;
	PWSTR pszLibFileW = (PWSTR)
		_alloca(cchSize * sizeof(wchar_t));
	// Convert the ANSI pathname to its Unicode equivalent
	StringCchPrintfW(pszLibFileW, cchSize, L"%S", pszLibFile);
	// Call the Unicode version of the function to actually do the work.
	return(EjectLibW(dwProcessId, pszLibFileW));
}







unsigned long PIDByName(const char* name)
{
	HANDLE pHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (pHandle == INVALID_HANDLE_VALUE)
	{
		printf("Failed to create snapshot\n");
		return 0;
	}

	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(pHandle, &ProcessEntry))
	{
		printf("Failed to get the first process\n");
		CloseHandle(pHandle);
		return 0;
	}
	int exeflag = -1;
	int cmp;
	do
	{
		cmp = 1;
		// Ensure null-termination for safety
		exeflag = -1;
		ProcessEntry.szExeFile[sizeof(ProcessEntry.szExeFile)/sizeof(WCHAR) - 1] = '\0';
		printf("Name is:   ");
		for (int i = 0; i < sizeof(ProcessEntry.szExeFile) / sizeof(WCHAR)-1&&exeflag!=0; i++) {
			if (ProcessEntry.szExeFile[i] == '.') exeflag=4;
			printf("%c", ProcessEntry.szExeFile[i]);
			if (exeflag) exeflag--;
			if (ProcessEntry.szExeFile[i] != name[i]) { cmp = 0;  break; }
		}
		printf(".\n");

		printf("ID is %d\n", ProcessEntry.th32ProcessID);
		if (cmp) {
			cout << "We found it!!" << endl;
			return ProcessEntry.th32ProcessID;
		}

	} while (Process32Next(pHandle, &ProcessEntry));

	CloseHandle(pHandle);
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
int main() {
	int result;
	int pid;
	printf("Hello!\n");
	cin >> pid;
	TCHAR szLibFile[MAX_PATH];
	GetModuleFileName(NULL, szLibFile, _countof(szLibFile));
	PTSTR pFilename = _tcsrchr(szLibFile, TEXT('\\')) + 1;
	_tcscpy_s(pFilename, _countof(szLibFile) - (pFilename - szLibFile),
		TEXT("HookTool.dll"));
	pid = PIDByName("notepad.exe");
	cout << "pid of notepad is: " << pid << endl;
	result = InjectLib((DWORD) pid, szLibFile);
	printf( "Success inject!!%d     %d\n", result, GetLastError());
	return 0;
}
///////////////////////////////////////////////////////////////////////////////





//////////////////////////////// End of File //////////////////////////////////