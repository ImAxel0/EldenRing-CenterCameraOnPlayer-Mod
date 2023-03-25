#include <Windows.h>

#include "ModUtils.h"

using namespace ModUtils;
bool centerCamera = true;

DWORD WINAPI MainThread(LPVOID lpParam)
{
	Log("Activating CenterCameraOnPlayer...");
	std::vector<uint16_t> pattern = { 0x66 ,0x0f ,0x7f ,0x07 ,0xf3 ,0x0f ,0x10 ,0xab };
	std::vector<uint16_t> originalBytes = { 0x66 ,0x0f ,0x7f ,0x07 };
	std::vector<uint8_t> newBytes = { 0x90, 0x90, 0x90, 0x90 };

	uintptr_t patchAddress = SigScan(pattern);
	if (patchAddress != 0)
	{
		Replace(patchAddress, originalBytes, newBytes);
		while (true) {
			if (GetAsyncKeyState(VK_CONTROL) & 1) {

				centerCamera = !centerCamera;

				if (centerCamera) {
					Replace(patchAddress, originalBytes, newBytes);
				}
				else if (!centerCamera) {
					Replace(patchAddress, { 0x90, 0x90, 0x90, 0x90 }, { 0x66 ,0x0f ,0x7f ,0x07 });
				}
			}
			Sleep(10);
		}
	}
	CloseLog();
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
		CreateThread(0, 0, &MainThread, 0, 0, NULL);
	}
	return 1;
}
