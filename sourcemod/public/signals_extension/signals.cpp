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

#include "signals.h"

using namespace std;
using namespace SourceMod;

/**
 * @file   signals.cpp
 * @brief  SignalsManager tracks the SignalHandlers assigned to POSIX
           signals and takes care of adding/removing handlers safely.
 */

bool SignalsManager::AddHandler(int signalCode, SourceMod::IChangeableForward* forward)
{
    // Do signals stuff here

    // Create a new handler unless one already exists
    return Handlers.emplace(SignalHandler(signalCode, forward)).second;
}

void SignalsManager::RemoveHandler(int signalCode)
{

}