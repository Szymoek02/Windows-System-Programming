#include <iostream>
#include <Windows.h>

DWORD PID = NULL, TID = NULL;
HANDLE hProcess, hThread = NULL;
LPVOID rBuffer = NULL;

//unsigned char shellCode[] = "\x11\x11\x11\x11\x11\x11\x11\x11";

HMODULE hKernel32 = NULL;
wchar_t dllPath[MAX_PATH] = L"C:\\Users\\8350\\source\\repos\\testDLL\\x64\\Debug\\testDLL.dll";
size_t pathSize = sizeof(dllPath);

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("No arguments specified\n");
		return -1;
	}

	PID = std::atoi(argv[1]);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL)
	{
		printf("Couldn't get a handle to process (%ld), error: %ld\n", PID, GetLastError());
		return -1;
	}
	printf("Got a handle to the process\n\\---0x%p\n", hProcess);

	rBuffer = VirtualAllocEx(hProcess, NULL, pathSize, (MEM_COMMIT | MEM_RESERVE), PAGE_READWRITE);
	if (rBuffer == NULL)
	{
		printf("Couldn't create buffer, error: %ld\n", GetLastError());
		return -1;
	}


	WriteProcessMemory(hProcess, rBuffer, dllPath, pathSize, NULL);


	hKernel32 = GetModuleHandleW(L"Kernel32");
	if (hKernel32 == NULL)
	{
		printf("Couldn't get handle to kernel32, error: %ld\n", GetLastError());
		return -1;
	}
	printf("Got a handle to the kernel32\n\\---0x%p\n", hKernel32);

	LPTHREAD_START_ROUTINE startThis = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");

	hThread = CreateRemoteThread(hProcess, NULL, 0, startThis, rBuffer, 0, &TID);
	if (hThread == NULL)
	{
		printf("Couldn't get a handle to thread (%ld), error: %ld\n", TID, GetLastError());
		CloseHandle(hProcess);
		return -1;
	}
	printf("Waiting for thread to finish\n");
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hProcess);
	CloseHandle(hThread);

	/*PID = std::atoi(argv[1]);

	printf("Trying to open a handle to process: (%ld)\n", PID);

	hProcces = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcces == NULL)
	{
		printf("Couldn't get a handle to process (%ld), error: %ld\n", PID, GetLastError());
		return -1;
	}
	printf("Got a handle to the process\n\\---0x%p\n", hProcces);

	rBuffer = VirtualAllocEx(hProcces, NULL, sizeof(shellCode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	printf("Allocated %zu-bytes\n", sizeof(shellCode));

	if (rBuffer == 0)
	{
		return -1;
	}
	
	WriteProcessMemory(hProcces, rBuffer, shellCode, sizeof(shellCode), NULL);
	printf("Wrote memory to process %zu-bytes\n", sizeof(shellCode));

	hThread = CreateRemoteThreadEx(hProcces, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, 0, &TID);
	if (hThread == NULL)
	{
		printf("Couldn't get a handle to thread (%ld), error: %ld\n", TID, GetLastError());
		CloseHandle(hProcces);
		return -1;
	}
	printf("Got a handle to the thread (%ld)\n\\---0x%p\n", TID, hThread);
	printf("Waiting for handle\n");
	WaitForSingleObject(hThread, INFINITE);


	CloseHandle(hProcces);
	CloseHandle(hThread);*/
	printf("Finished\n");
	/*STARTUPINFOW si = {};
	* polymorphic malwar\Fx@1fe
	PROCESS_INFORMATION pi = {};
	if (!CreateProcessW(L"C:\\Windows\\System32\\notepad.exe", NULL, NULL, NULL, FALSE, BELOW_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		std::cout << "failed to create";
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);*/
	return 0;
}