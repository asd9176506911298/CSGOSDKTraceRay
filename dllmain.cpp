// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "csgosdk.h"
#include "csgo.h"
#include "csgotrace.h"
#include "csgoVector.h"
#include <Windows.h>
#include <iostream>

using namespace hazedumper;

tTraceRay TraceRay;
tCreateInterface CreateInterface;

DWORD WINAPI HackThread(HMODULE hModule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    uintptr_t clientdll = (uintptr_t)GetModuleHandle("client.dll");
    uintptr_t enginedll = (uintptr_t)GetModuleHandle("engine.dll");

    CreateInterface = (tCreateInterface)GetProcAddress((HMODULE)enginedll, "CreateInterface");
    IEngineTrace* EngineTrace = (IEngineTrace*)GetInterface(CreateInterface, "EngineTraceClient004");

    

    while (!GetAsyncKeyState(VK_END))
    {
        Ent* pLocal = *(Ent**)(clientdll + signatures::dwLocalPlayer);
        EntList* entList = (EntList*)(clientdll + signatures::dwEntityList);

        if(GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            for (auto currEnt : entList->entListObjs)
            {
                if (currEnt.ent && currEnt.ent->clientId != pLocal->clientId)
                {
                    Vector eyepos = pLocal->origin + pLocal->m_vecViewOffset;
                    Vector targeteyepos = currEnt.ent->origin + currEnt.ent->m_vecViewOffset;

                    CGameTrace trace;
                    Ray_t ray;
                    CTraceFilter tracefilter;
                    tracefilter.pSkip = (void*)pLocal;

                    ray.Init(eyepos, targeteyepos);

                    EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &tracefilter, &trace);
                    //EngineTrace->TraceRay(ray, MASK_NPCWORLDSTATIC | CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX, &tracefilter, &trace);


                    if (currEnt.ent == trace.m_pEnt)
                    {
                        std::cout << "Ent 0x" << std::hex << currEnt.ent << " Visible\n";
                    }
                    else
                    {
                        std::cout << "Ent 0x" << std::hex << currEnt.ent << " InVisible\n";
                    }
                }
                
            } 
        }
        Sleep(50);
        
    }

    if (f) fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE hdl = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0);
        if (hdl) CloseHandle(hdl);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

