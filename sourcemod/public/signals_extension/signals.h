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
#ifndef _INCLUDE_SOURCEMOD_EXTENSION_SIGNALS_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_SIGNALS_H_

/**
 *  @file       signals.h
 *  @brief      Defines a SignalsManager class for managing signal handlers.
 */

#include <thread>
#include <chrono>
#include <memory>
#include <atomic>
#include "signal_handler.h"

/* Maximum number of signal handlers that can be stored */
#define MAX_SIGNALS 128
/* SIGKILL is used as the default event value */
#define DEFAULT_SIGNAL 9

class SignalsManager 
{
public:
    /**
     *  @brief  SignalsManager holds a private array of pointers to Signal Handler structs and is
     *  responsible for managing their lifetime from creation to deletion. 
     *
     *  SignalsManager will periodically check if there are new signal invocations through a 
     *  public atomic integer (SRCDS is multithreaded) that is also marked as volatile due to 
     *  the asynchronous nature of POSIX reliable signals.
     *
     *  @param interval     How often (in ms) the SignalEvent value gets checked for new signal invocations.
     */
    SignalsManager(int interval = 5000) 
                    : PollInterval(interval) 
    {};

    /**
     *  @brief  Store a signal handler that was created elsewhere. If a handler for 
     *  the same signal already exists, replace the stored one with the new
     *  sigaction but preserve the original old state.
     * 
     *  @param signalCode   Numeric value of the signal.
     *  @param handler      Raw pointer to a handler struct.
     *  @return             False if a previous handler was replaced by the new one,
     *                      true otherwise.
     */
    bool AddHandler(int signalCode, SignalHandler* handler);

    /**
     *  @brief  Remove a signal handler and its associated forward if one was created
     *  by a plugin. Restore the previous state of the signal handler.
     *
     *  @param signalCode   Signal to reset.
     */
    void ResetHandler(int signalCode);

    /**
     *  @brief  A publically accessible atomic variable that sigaction handler(s)
     *  should modify to make SignalsManager aware of new signal invocations. 
     *  Initialized to SIGKILL which cannot be masked, blocked, modified etc. 
     */
    std::atomic<int> SignalEvent{ DEFAULT_SIGNAL };

    /**
     *  @brief  Polls the SignalEvent variable and sleeps for a PollInterval period between 
     *  each run. On SignalEvent change, executes the specified SignalHandler, resets the 
     *  variable and continues polling. This function should be called from a separate thread. 
     * 
     *  Since signal handlers can't be async-safe about mutexes or conditional variables and let
     *  them notify us when a signal was received and a variable was changed, polling the atomic 
     *  variable periodically is probably the best solution for our purposes
     */
    void Poll();

private:

    int PollInterval;

    std::unique_ptr<SignalHandler> Handlers[ MAX_SIGNALS ];
};

#endif // _INCLUDE_SOURCEMOD_EXTENSION_SIGNALS_H_