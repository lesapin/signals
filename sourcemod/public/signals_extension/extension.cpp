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

#include <cassert>
#include "extension.h"

using namespace std;

static SignalsManager*	Manager;		/**< SignalsManager is made a local object				*/
SignalForwards			g_Extension;	/**< Global singleton for extension's main interface	*/

SMEXT_LINK(&g_Extension);

const sp_nativeinfo_t SignalNatives[] =
{
	{"CreateHandler",	CreateHandler},
	{"RemoveHandler",	RemoveHandler},
	{NULL,				NULL},
};

cell_t CreateHandler(IPluginContext* pContext, const cell_t* params)
{
	// A plugin has passed us a signal, create a handler for it.
	int Signal = static_cast<int>(params[1]);

	// sigset_t represents the set of signals that should be blocked when this signals 
	// handler is executed. By default, only the signal itself will be blocked. 
	sigset_t ToBlock;

	sigemptyset(&ToBlock); 
	sigaddset(&ToBlock, Signal);

	// A bitmask of flags that modify the behavior of the signal. In this case
	// it is made to use the sa_sigaction() handler instead of the outdated signal().
	int SigFlags = SA_SIGINFO;

	struct sigaction SigactionNew, SigactionOld;

	SigactionNew.sa_mask		= ToBlock;
	SigactionNew.sa_sigaction	= SigAction;
	SigactionNew.sa_flags		= SigFlags;

	// Call sigaction and let the system setup the rest.
	if (sigaction(Signal, &SigactionNew, &SigactionOld) != 0)
	{
		rootconsole->ConsolePrint("ERROR: Failed to add sigaction handler");
		return false;
	}

	auto Forward = forwards->CreateForwardEx(NULL, ET_Event, 0, NULL);

	rootconsole->ConsolePrint("sigevent: %i", Manager->SignalEvent.load());

	// A callback that our handler will call when the signal is received.
	if (!Forward->AddFunction(pContext, static_cast<funcid_t>(params[2])))
	{
		rootconsole->ConsolePrint("ERROR: Failed to add callback function");
		forwards->ReleaseForward(Forward);
		return false;
	};

	// Do something if the old signal handler was anything other than SIG_IGN | SIG_DFL
	if (SigactionOld.sa_handler == SIG_IGN)
	{
		rootconsole->ConsolePrint("old sa_handler: SIG_IGN ");
	}
	else if (SigactionOld.sa_handler == SIG_DFL)
	{
		rootconsole->ConsolePrint("old sa_handler: SIG_DFL ");
	}
	else
	{
		rootconsole->ConsolePrint("old sigaction was a handler ");
		//TODO
	}

	// Pack everything created here in a SignalHandler struct.
	auto Handler = new SignalHandler(Signal, Forward, SigactionNew, SigactionOld);

	return Manager->AddHandler(Signal, Handler);
}

cell_t RemoveHandler(IPluginContext* pContext, const cell_t* params)
{
	Manager->ResetHandler(static_cast<int>(params[1]));
	return 1;
}

void SigAction(int signal, siginfo_t* info, void* ucontext)
{
	int current = Manager->SignalEvent.load();

	//	Make sure no signals are pending. The previous signal invocation has 
	//	to be handled first if the current SignalEvent isn't on default value.
	if (current == DEFAULT_SIGNAL)										
	{
		Manager->SignalEvent.store(signal);					/* Atomically replace the current value */
	}
	else
	{
		//int ret = Manager->SignalEvent.exchange(signal);	/* Store the signal somewhere and process later */
		return;
	}

	// Do something useful with siginfo here...
}

bool SignalForwards::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	Manager = new SignalsManager(); 

	assert(Manager->SignalEvent.is_lock_free());	/* Verify the lock-free property */

	sharesys->AddNatives(myself, SignalNatives);

	// Start polling for signal events
	SignalThread = make_unique<thread>(&SignalsManager::Poll, Manager);

	return true;
}

void SignalForwards::SDK_OnUnload()
{
	// Calling delete on Manager causes the unique_ptr Handlers array to 
	// go out of scope, invoking the destructors for each SignalHandler where 
	// we take care of releasing all the forwards that were assigned here. 
	delete Manager;

	// Synchronize the signal polling thread.
	SignalThread->join();
}