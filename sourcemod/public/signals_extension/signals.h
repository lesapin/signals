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
#ifndef _INCLUDE_SOURCEMOD_EXTENSION_SIGNALS_MANAGER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_SIGNALS_MANAGER_H_

/**
 * @file   signals.h
 * @brief  SignalsManager tracks the SignalHandlers assigned to POSIX
           signals and takes care of adding/removing handlers safely.
 */

#include <signal.h>
#include <IForwardSys.h>
#include <set>

/**
 *  SignalHandler encapsulates all relevant information about a POSIX signal,
 *  including a pointer to the forward associated with the signal.
 */
struct SignalHandler
{
public:
    SignalHandler(int signalCode, SourceMod::IChangeableForward* signalForward)
        : code(signalCode), forward(signalForward)
    {};

    /**
     *  Get the forward pointer.
     */
    SourceMod::IChangeableForward* Get() { return forward; };

    int code;

private:
    SourceMod::IChangeableForward* forward = nullptr;
};

/**
 *  Sort SignalHandlers in the Handlers set by their signal code.
 */
struct SignalCmp
{
    bool operator()(const SignalHandler& lhs, const SignalHandler& rhs) const
    {
        return lhs.code < rhs.code;
    };
};

/**
 *  SignalsManager takes a pointer to IForwardManager defined by SourceMod
 *  and connects plugin-supplied callbacks with POSIX signal handlers.
 */
class SignalsManager 
{
public:
    SignalsManager(SourceMod::IForwardManager* FM) 
                    : ForwardManager(FM) 
    {};

    /**
     *  Create a new signal handler (if possible), associate a
     *  callback with its private forward and add it to Handlers.
     */
    bool AddHandler(int signalCode, SourceMod::IChangeableForward* forward);

    /**
     *  Release the signal handlers forward, remove it from
     *  the Handlers set and then delete the object.
     */
    void RemoveHandler(int signalCode);

private:
    SourceMod::IForwardManager* ForwardManager;

    /**
     *  The set of Handlers being managed, with signal codes 
     *  used as comparators.
     */
    std::set<SignalHandler, SignalCmp> Handlers;
};

#endif // _INCLUDE_SOURCEMOD_EXTENSION_SIGNALS_MANAGER_H_