#include <Windows.h>

#include "ModUtils.h"
#include "ini.h"

using namespace ModUtils;
using namespace mINI;
bool centerCamera = true;
bool isPermanent = true;;
DWORD toggleKey{ 0x71 };

void ReadConfig()
{
	INIFile config(GetModuleFolderPath() + "\\config.ini");
	INIStructure ini;

	if (config.read(ini))
	{
		toggleKey = std::stoi(ini["Toggle key"].get("key value"));
		isPermanent = std::stoi(ini["Can be toggled"].get("value"));
	}
	else
	{
		ini["Toggle key"]["key value"] = "113";
		ini["Can be toggled"]["value"] = "1";
	}
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
	ReadConfig();

	Log("Activating CenterCameraOnPlayer...");
	std::vector<uint16_t> pattern = { 0x66 ,0x0f ,0x7f ,0x07 ,0xf3 ,0x0f ,0x10 ,0xab };
	std::vector<uint16_t> originalBytes = { 0x66 ,0x0f ,0x7f ,0x07 };
	std::vector<uint8_t> newBytes = { 0x90, 0x90, 0x90, 0x90 };

	uintptr_t patchAddress = SigScan(pattern);
	if (patchAddress != 0)
	{
		Replace(patchAddress, originalBytes, newBytes);

		if (isPermanent) {
			while (true) {
				if (GetAsyncKeyState(toggleKey) & 1) {

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
