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
 *  @file       signals.cpp
 *  @brief      Implements a SignalsManager class for managing signal handlers.
 */

#include <iostream>
#include "signals.h"

using namespace std;
using namespace SourceMod;

bool SignalsManager::AddHandler(int signalCode, SignalHandler* handler)
{
    if (Handlers[signalCode] != nullptr)
    {
        // Keep it simple for now
    }

    Handlers[signalCode] = unique_ptr<SignalHandler>(handler);

    return true;
}

void SignalsManager::ResetHandler(int signalCode)
{
    if (Handlers[signalCode] != nullptr)
    {
        // Keep it simple for now
        Handlers[signalCode]->Reset();          /* Resets the signal handler */
        Handlers[signalCode].reset( nullptr );  /* Resets the handler object */
    }
}

void SignalsManager::Poll()
{
    // Expected value inside the SignalEvent atomic
    int expected = DEFAULT_SIGNAL;

    // Using compare_exchange_weak() inside a loop is preferrable to
    // compare_exchange_strong() for better performance and because some 
    // misses are acceptable here.
    while (true)
    {
        cout << "Polling.." << endl;

        // If $SignalEvent != expected, replaces expected with the contained value. 
        if (!SignalEvent.compare_exchange_weak(expected, DEFAULT_SIGNAL))
        {
            cout << "Signal event encountered. signal code: " << expected << endl;

            if (Handlers[expected] != nullptr)
            {
                // Trying out some simple exceptions handling.
                try 
                {
                    cout << "Calling forward" << endl;
                    int ret = Handlers[expected]->ExecForward();
                    if (ret != SP_ERROR_NONE) throw(ret);
                }
                catch (int& e)
                {
                    // deal with bad callbacks..
                    continue;
                }
            }
            else
            {
                // do something else..
            }

            expected = DEFAULT_SIGNAL;

            // Reset the SignalEvent once we are done.
            SignalEvent.store(DEFAULT_SIGNAL);
        }

        this_thread::sleep_for(chrono::milliseconds(PollInterval));
    }
}