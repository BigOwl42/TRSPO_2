#include <iostream>
#include<Windows.h>
#include<fileapi.h>
#include<errhandlingapi.h>
#include<detours.h> 


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
		printf("[!] DetourTransactionBegin ����������� ������� � ����� : %d \n", dwDetoursErr);
		return FALSE;
	}

	if ((dwDetoursErr = DetourUpdateThread(GetCurrentThread())) != NO_ERROR) {
		printf("[!] DetourUpdateThread ����������� ������� � ����� : %d \n", dwDetoursErr);
		return FALSE;
	}

	// ������ MyMessageBoxA ������ g_pMessageBoxA, ������� �������� MessageBoxA
	if ((dwDetoursErr = DetourAttach(&(PVOID&) realCreateFile, myCreateFile)) != NO_ERROR) {
		printf("[!] DetourAttach ����������� ������� � ����� : %d \n", dwDetoursErr);
		return FALSE;
	}

	// ����������� ��������� ���� ���������� ����� `DetourTransactionCommit` - ������������� ����������
	if ((dwDetoursErr = DetourTransactionCommit()) != NO_ERROR) {
		printf("[!] DetourTransactionCommit ����������� ������� � ����� : %d \n", dwDetoursErr);
		return FALSE;
	}


	return 0;
}


int main() {
	//This is defenselles programm
	HANDLE newFile;
	int result;
	printf("Hello!");
	myAngryHook();
	newFile = CreateFileA ("NewFile",
		GENERIC_WRITE,
		NULL,
		NULL,
		CREATE_NEW,
		NULL,
		NULL);
	result = GetLastError();
	if (result)
		printf("Error: %d\n", result);
	else printf("Success open!");
	system("PAUSE");
}