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

#pragma once
#ifndef _INCLUDE_SOURCEMOD_EXTENSION_SIGNAL_HANDLER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_SIGNAL_HANDLER_H_

/**
 *  @file       signal_handler.h
 *  @brief      Defines and implements a container class used in signal transactions.
 */

#include <signal.h>
#include <IForwardSys.h>
#include "smsdk_ext.h"

struct SignalHandler
{
public:
    /**
     *  @brief  SignalHandler encapsulates all relevant information about a POSIX signal,
     *  including a pointer to the forward associated with the signal and the signal 
     *  handlers previous state.
     *
     *  @param signal       Numeric value of the signal associated with the forward.
     *  @param forward      SourceMod forward function that acts as a signal "handler".
     *  @param saNew        Struct sigaction contains all information about the new signal handler.
     *  @param saOld        Struct sigaction contains all information about the previous signal.
     *                      Useful for re-establishing the previous state of the signal handler if
     *                      the new signal handler gets removed by a plugin at a later point.
     */
    SignalHandler(int signal, SourceMod::IChangeableForward* signalForward, struct sigaction saNew, struct sigaction saOld)
                    : Code(signal), Forward(signalForward), SigactionNew(saNew), SigactionOld(saOld) 
    {};

    /**
     *  @brief  SourceMod does not automatically clean up forwards for us.
     */
    ~SignalHandler()
    {
        forwards->ReleaseForward(Forward);
    };

    /**
     *  @brief  Execute the stored forward, causing a callback to trigger in any SourceMod plugin 
     *  that is listening to this handlers signal.
     * 
     *  @return     Zero if the subcall executed without errors, otherwise an error code.
     */
    int ExecForward() { return Forward->Execute(); };

    /**
     *  @brief  Reset this signal to its original handler.
     */
    void Reset() 
    {
        return;//TODO
    };

private:

    int Code;

    SourceMod::IChangeableForward* Forward;

    struct sigaction    SigactionNew, 
                        SigactionOld;
};

#endif // _INCLUDE_SOURCEMOD_EXTENSION_SIGNAL_HANDLER_H_
