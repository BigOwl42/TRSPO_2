#include <iostream>
#include<Windows.h>
#include<fileapi.h>
#include<errhandlingapi.h>
#include<detours.h> 
#include<libloaderapi.h>

int main() {
	//This is defenselles programm
	HANDLE newFile;
	int result;
	printf("Hello!");
	if (LoadLibrary(L"HookTool.dll") == NULL)
		printf("Error of load lib: %d", GetLastError());
	else printf("Successful load!");
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