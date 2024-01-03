/**
 * Signal Forwards Extension
 * Copyright (C) 2024 bezdmn
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *	@file		extension.cpp
 *	@brief		Main implementation file of the extension.
 */

#include "extension.h"

#define DEBUG
#define MAX_SIGNALS 64

#ifdef DEBUG
    #define DBGPRINT(...) rootconsole->ConsolePrint(__VA_ARGS__)
#else
    #define DBGPRINT(...) 0
#endif

using namespace std;

SignalForwards g_Extension;    /**< Global singleton for extension's main interface	*/

SMEXT_LINK(&g_Extension);

const sp_nativeinfo_t SignalNatives[] =
{
    {"CreateHandler",   CreateHandler},
    {"RemoveHandler",   RemoveHandler},
    {NULL,              NULL},
};

IChangeableForward* g_Handlers[MAX_SIGNALS];    /* Could also use a map or a set */

cell_t CreateHandler(IPluginContext* pContext, const cell_t* params)
{
    int Signal = static_cast<int>(params[1]);
    
    if (int NumFuncs = g_Handlers[Signal]->GetFunctionCount())
    {
        rootconsole->ConsolePrint("ERROR: Forward for signal %i already has %i function(s) assigned", Signal, NumFuncs);
        return FuncCountError;
    }

    if (!g_Handlers[Signal]->AddFunction(pContext, static_cast<funcid_t>(params[2])))
    {
        rootconsole->ConsolePrint("ERROR: Failed to add callback function for signal %i", Signal);
        return CallbackError;
    }

    int ret = SetSAHandler(Signal);
    
    if (ret != NoError)
    {
        RemoveFunctionsFromForward(Signal, pContext);
    }

    return ret;
}

cell_t RemoveHandler(IPluginContext* pContext, const cell_t* params)
{
    int Signal = static_cast<int>(params[1]);

    if (g_Handlers[Signal] != nullptr)
    { 
        if (ResetSAHandler(Signal) != NoError)
        {
            // ...
        }

        RemoveFunctionsFromForward(Signal, pContext);
    }

    return 1;
}

void SigAction(int signal, siginfo_t* info, void* ucontext)
{
    // The thread's signal mask ... are restored as part of this procedure. Reset the sa_handler here. 
    // sigaction() itself is reentrant and therefore safe to call.
    SetSAHandler(signal);

    // Execute the forward. I'm currently convinced it's reentrant.
    g_Handlers[signal]->Execute(); 
}

bool SignalForwards::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
    for (int i = 0; i < MAX_SIGNALS; i++)
    {
        g_Handlers[i] = forwards->CreateForwardEx(NULL, ET_Event, 0, NULL);

        if (g_Handlers[i] == nullptr)
        {
            rootconsole->ConsolePrint("ERROR: Failed to create a forward for signal %i", i);
            return false;
        }
    }

    sharesys->AddNatives(myself, SignalNatives);

    DBGPRINT("Signals extension loaded");
    return true;
}

void SignalForwards::SDK_OnUnload()
{
    for (int i = 0; i < MAX_SIGNALS; i++)
    {
        if (g_Handlers != nullptr)
        {
            forwards->ReleaseForward(g_Handlers[i]); 
            ResetSAHandler(i);
        }
    }
}

/* Helper functions*/

int SetSAHandler(int signal)
{
    struct sigaction SigactionNew {}, SigactionOld{};

    // sigset_t represents the set of signals that should be blocked when this signals 
    // handler is executed. By default, only the signal itself will be blocked. 
    sigemptyset(&SigactionNew.sa_mask);

    sigaddset(&SigactionNew.sa_mask, signal);

    // Use the sa_sigaction() handler instead of the outdated signal().
    SigactionNew.sa_flags = SA_SIGINFO;

    // Set the sigaction handler.
    SigactionNew.sa_sigaction = SigAction;

    if (sigaction(signal, &SigactionNew, &SigactionOld) != 0)
    {
        rootconsole->ConsolePrint("ERROR: Failed to add sigaction handler for signal %i", signal);
        return SigactionError;
    }

    if (SigactionOld.sa_handler == SIG_IGN)
    {
        DBGPRINT("old sa_handler: SIG_IGN ");
    }
    else if (SigactionOld.sa_handler == SIG_DFL)
    {
        DBGPRINT("old sa_handler: SIG_DFL ");
    }
    else // Replacing a sigaction handler is crash prone so just bail out.
    {
        rootconsole->ConsolePrint("ERROR: old sigaction was a handler ");
        return SAHandlerError;
    }

    return NoError;
}

int ResetSAHandler(int signal)
{
    // Reset the signals disposition to OS default and clear the forward handle.

    struct sigaction SigactionDefault {};

    // Could store previous actions into an array and restore from there

    sigemptyset(&SigactionDefault.sa_mask);

    sigaddset(&SigactionDefault.sa_mask, signal);

    SigactionDefault.sa_flags = 0;

    // Set the default handler.
    SigactionDefault.sa_handler = SIG_DFL;

    if (sigaction(signal, &SigactionDefault, nullptr) != 0)
    {
        rootconsole->ConsolePrint("ERROR: Failed to reset sa_handler for signal %i", signal);
        return SigactionError;
    }

    return NoError;
}

void RemoveFunctionsFromForward(int signal, IPluginContext* pContext)
{
    // Remove all (should be only 1) functions from the forward.
    int NumRemoved = g_Handlers[signal]->RemoveFunctionsOfPlugin(plsys->FindPluginByContext(pContext->GetContext()));

    DBGPRINT("removed %i function(s) from g_Handlers[%i]", NumRemoved, signal);
}