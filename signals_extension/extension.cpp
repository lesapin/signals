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

#include "extension.h"
#include "signals.h"

// Global singleton
SignalForwards g_Extension;

// Local singleton
static SignalsManager Manager(forwards);

SMEXT_LINK(&g_Extension);

const sp_nativeinfo_t MyNatives[] =
{
	{"AddSignalHandler",	AddSignalHandlerToManager},
	{"RemoveSignalHandler",	RemoveSignalHandlerFromManager},
	{NULL,					NULL},
};

bool SignalForwards::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	sharesys->AddNatives(myself, MyNatives);
	return true;
}

void SignalForwards::SDK_OnUnload()
{
	// remember to release all forwards here
}

bool SignalForwards::QueryRunning(char* error, size_t maxlen)
{
	return true;
}

cell_t AddSignalHandlerToManager(IPluginContext* pContext, const cell_t* params)
{
	// Set the forward function
	auto Forward = forwards->CreateForwardEx(NULL, ET_Event, 0, NULL);
	Forward->AddFunction(pContext, static_cast<funcid_t>(params[2]));

	// Return true if the handler was added succesfully
	return Manager.AddHandler(static_cast<int>(params[1]), Forward);
}

cell_t RemoveSignalHandlerFromManager(IPluginContext* pContext, const cell_t* params)
{
	Manager.RemoveHandler(static_cast<int>(params[1]));

	return 1;
}