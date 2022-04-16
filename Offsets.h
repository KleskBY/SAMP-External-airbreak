#pragma once
#include <Windows.h>

namespace Offsets
{
	const DWORD ViewMatrix = 0x76FF90;
	const DWORD PlayerOrigin = 0x76FF74;
	const DWORD IsGameWindowActive = 0x4D621C; //gta_sa.exe+4D621C
	const DWORD IsCursorVisable = 0x562B744;// libcef.dll + 562B744
	const DWORD LocalPlayer = 0x0026BC58; 
	const DWORD Money = 0x77CE50;

	const DWORD LocalPalyerPtr = 0x76F5F0;
		const DWORD Health = 0x0540;
		const DWORD SpeedY = 0x004C;
}