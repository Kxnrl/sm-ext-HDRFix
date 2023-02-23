#include "extension.h"

#include <iplayerinfo.h>
#include <inetchannel.h>

#if defined _WIN32 || defined _WIN64
#define sleep_ms(x) Sleep(x);
#else
#define sleep_ms(x) usleep(x * 1000);
#endif

#define DEBUG

HDRFix g_Extension;
IPlayerInfoManager* g_pPlayerInfoManager = nullptr;
CGlobalVars* g_pGlobals = nullptr;

SMEXT_LINK(&g_Extension);

SH_DECL_HOOK2_void(IVEngineServer, ChangeLevel, SH_NOATTRIB, 0, const char*, const char*);

void HDRFix::OnChangeLevel(const char* s1, const char* s2)
{
#ifdef DEBUG
    smutils->LogMessage(myself, "OnChangeLevel -> %.2f :: %" PRIu64 ".", g_pGlobals->curtime, time(nullptr));
#endif

    for (auto i = 1; i <= g_pGlobals->maxClients; i++)
    {
        auto* const pEdict = gamehelpers->EdictOfIndex(i);

        if (pEdict == nullptr || pEdict->IsFree())
        {
            continue;
        }

        auto const pNetChan = static_cast<INetChannel*>(engine->GetPlayerNetInfo(i));
        if (pNetChan == nullptr)
        {
            continue;
        }

        auto* const  pInfo = g_pPlayerInfoManager->GetPlayerInfo(pEdict);

        if (pInfo == nullptr || !pInfo->IsConnected() || pInfo->IsFakeClient())
        {
            continue;
        }

        engine->ClientCommand(pEdict, "retry;");
        pNetChan->Transmit();

#ifdef DEBUG
        smutils->LogError(myself, "Force %s<%s> retry", pInfo->GetName(), pInfo->GetNetworkIDString());
#endif
    }


    sleep_ms(100)

#ifdef DEBUG
        smutils->LogMessage(myself, "OnChangeLevel -> %.2f :: %" PRIu64 ".", g_pGlobals->curtime, time(nullptr));
#endif
}

bool HDRFix::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
    sharesys->RegisterLibrary(myself, "HDRFix");
    return true;
}

void HDRFix::SDK_OnUnload()
{

}

bool HDRFix::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    g_pGlobals = ismm->GetCGlobals();

    GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
    GET_V_IFACE_CURRENT(GetServerFactory, g_pPlayerInfoManager, IPlayerInfoManager, INTERFACEVERSION_PLAYERINFOMANAGER);

    SH_ADD_HOOK(IVEngineServer, ChangeLevel, engine, SH_MEMBER(this, &HDRFix::OnChangeLevel), false);

    return true;
}

bool HDRFix::SDK_OnMetamodUnload(char* error, size_t maxlength)
{
    SH_REMOVE_HOOK(IVEngineServer, ChangeLevel, engine, SH_MEMBER(this, &HDRFix::OnChangeLevel), false);
    return true;
}
