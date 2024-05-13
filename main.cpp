#include <Windows.h>
#include <iostream>
#include <winternl.h>

#ifdef _WIN64
	PPEB pPEB = (PPEB)__readgsqword(0x60);
#elif _WIN32
	PPEB pPEB = (PPEB)__readfsdword(0x30);
#endif
#define NEW_STREAM L":astream"

using namespace std;
extern "C" { PTEB getTEB(); }
extern "C" { DWORD CustomError(); }

BOOL checkDebug()
{
	return pPEB->BeingDebugged;
}

int selfDelete()
{
	cout << "Deleting..." << endl;
	HANDLE                hFile = INVALID_HANDLE_VALUE;
	const wchar_t* NEWSTREAM = NEW_STREAM;
	size_t                RenameSize = sizeof(FILE_RENAME_INFO) + sizeof(NEWSTREAM);
	PFILE_RENAME_INFO     PFRI = NULL;
	wchar_t               Path[MAX_PATH * 2] = { 0 };
	FILE_DISPOSITION_INFO SetDelete = { 0 };

	// ----- [ ALLOCATE BUFFER FOR PFILE_RENAME_INFO ] -----
	PFRI = (PFILE_RENAME_INFO)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, RenameSize);
	if (PFRI == NULL)
	{
		cout << "HeapAlloc failed\n" << GetLastError();
		return 1;
	}

	ZeroMemory(Path, MAX_PATH * 2);
	ZeroMemory(&SetDelete, sizeof(SetDelete));

	// ----- [ MARK FOR DELETION ] -----
	SetDelete.DeleteFileW = TRUE;

	// ----- [ SET NEW DATA STREAM BUFFER AND SIZE IN FILE_RENAME_INFO ] -----
	PFRI->FileNameLength = sizeof(NEWSTREAM);
	RtlCopyMemory(PFRI->FileName, NEWSTREAM, sizeof(NEWSTREAM));

	// ----- [ GET FILE NAME ] -----
	if (GetModuleFileNameW(NULL, Path, MAX_PATH * 2) == 0)
	{
		cout << "GetModuleFileNameW failed\n" << GetLastError();
		return 1;
	}

	// ----- [ GET FILE HANDLE ] -----
	hFile = CreateFileW(Path, (DELETE | SYNCHRONIZE), FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		cout << "1. CreateFileW failed\n" << GetLastError();
		return 1;
	}
	// ----- [ RENAME ] -----
	if (!SetFileInformationByHandle(hFile, FileRenameInfo, PFRI, RenameSize))
	{
		cout << "1. SetFileInformationByHandle failed \n" << GetLastError();
		return 1;
	}
	CloseHandle(hFile);

	// ----- [ DELETION II ] -----
	hFile = CreateFileW(Path, DELETE | SYNCHRONIZE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		cout << "2. CreateFileW failed\n" << GetLastError();
		return 1;
	}
	if (!SetFileInformationByHandle(hFile, FileDispositionInfo, &SetDelete, sizeof(SetDelete)))
	{
		cout << "2. SetFileInformationByHandle failed \n" << GetLastError();
		return 1;
	}
	CloseHandle(hFile);

	HeapFree(GetProcessHeap(), 0, PFRI);
	return 0;
}

int main(int argc, char* argv[])
{
	if (!checkDebug())
	{
		MessageBoxW(NULL, L"Aaaa", L"Bbbb", MB_ICONWARNING | MB_YESNO);
		return 0;
	}
	selfDelete();

	/*printf("Getting the TEB...\n");
	PTEB pTEB = getTEB();
	printf("TEB: 0x%p\n", pTEB);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 333);
	if (hProcess == NULL)
	{
		printf("Custom code error: 0x%lx\n", CustomError());
		printf("System code error: 0x%lx\n", GetLastError());
		return -1;
	}*/
	return 0;
}
