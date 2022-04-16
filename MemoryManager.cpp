#include "MemoryManager.h"
HANDLE MemoryManager::hProc;
MemoryManager::MemoryManager()
{
    this->hProc = NULL;
    this->pID = NULL;
    try
    {
        while (!attachProc())
        {
            attachProc();
            Sleep(100);
            std::cout << "1" << std::endl;
        }
        this->eDll = getModule(pID, "gta_sa.exe"); //"malinovka_game.exe"
		this->cDll = getModule(pID, "samp.dll");
    }
    catch (...)
    {
        MessageBoxA(NULL, "Cant access memory", 0, MB_ICONSTOP | MB_OK);
    }
}

MemoryManager::~MemoryManager()
{
    CloseHandle(this->hProc);
}
