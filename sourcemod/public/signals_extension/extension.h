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

#include "smsdk_ext.h"

class SignalForwards : public SDKExtension
{
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late);
	virtual void SDK_OnUnload();
	virtual bool QueryRunning(char *error, size_t maxlen);
};

//	Native functions exposed to plugins
cell_t AddSignalHandlerToManager(IPluginContext* pContext, const cell_t* params);
cell_t RemoveSignalHandlerFromManager(IPluginContext* pContext, const cell_t* params);

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
