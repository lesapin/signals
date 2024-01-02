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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

/**
 *	@file		extension.h
 *	@brief		Main definitions file of the extension.
 */

#include "smsdk_ext.h"
#include "signals.h"

class SignalForwards : public SDKExtension
{
public:
	/**
	 *	@brief	Initialize Signals extension.
	 */
	virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late);

	/**
	 *	@brief	Cleanup and release all the forwards.
	 */
	virtual void SDK_OnUnload();

private:
	/**
	 *	@brief	Maintain a separate thread for polling signal events.
	 */
	std::unique_ptr<std::thread> SignalThread;
};

/**
 *	@brief	Native functions exposed to plugins
 */
cell_t CreateHandler(IPluginContext* pContext, const cell_t* params);

/**
 *	@brief	Native functions exposed to plugins
 */
cell_t RemoveHandler(IPluginContext* pContext, const cell_t* params);

/**
 *	@brief	SigAction specifies the signal handler in a sigaction struct where the SA_SIGINFO flag is set.
 *	Since only functions that are considered async-signal-safe should be called from within a signal
 *	handler, the safest thing to do here is to change the state of some (global) atomic variable.
 * 
 *	@param signal		Numeric value of the signal invoking the handler
 *	@param info			Structure containing further information about the signal
 *	@param ucontext		Pointer to a ucontext_t struct containing user-space stack information
 */
void SigAction(int signal, siginfo_t* info, void* ucontext);


#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
