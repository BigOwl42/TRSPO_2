#undef UNICODE
#include <cstdio>
#include <windows.h>
#include <detours.h>


typedef HANDLE(WINAPI* fnCreateFile)(
	LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);
fnCreateFile realCreateFile = CreateFileA;



HANDLE myCreateFile(LPCSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile)
{
	HANDLE handle = NULL;
	printf("Mua-ha-ha! You programm is hack!\n");
	realCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	return handle;
}


int myAngryHook() {
	DWORD    dwDetoursErr = NULL;
	if ((dwDetoursErr = DetourTransactionBegin()) != NO_ERROR) {
		printf("[!] DetourTransactionBegin error : %d \n", dwDetoursErr);
		return FALSE;
	}

	if ((dwDetoursErr = DetourUpdateThread(GetCurrentThread())) != NO_ERROR) {
		printf("[!] DetourUpdateThread error : %d \n", dwDetoursErr);
		return FALSE;
	}

	// ������ MyMessageBoxA ������ g_pMessageBoxA, ������� �������� MessageBoxA
	if ((dwDetoursErr = DetourAttach(&(PVOID&)realCreateFile, myCreateFile)) != NO_ERROR) {
		printf("[!] DetourAttach error : %d \n", dwDetoursErr);
		return FALSE;
	}

	// ����������� ��������� ���� ���������� ����� `DetourTransactionCommit` - ������������� ����������
	if ((dwDetoursErr = DetourTransactionCommit()) != NO_ERROR) {
		printf("[!] DetourTransactionCommit error : %d \n", dwDetoursErr);
		return FALSE;
	}


	return 0;
}






INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
	{
		myAngryHook();
	}
	break;
	/*
	 case DLL_PROCESS_DETACH:
		 DetourTransactionBegin();
		 DetourUpdateThread(GetCurrentThread());
		 DetourDetach(&(PVOID&)FuncToDetour, MyFunc);
		 DetourTransactionCommit();
		 break;
	 case DLL_THREAD_ATTACH:
	 case DLL_THREAD_DETACH:
		 break;
	 }
	 */
	return TRUE;
	}
}

