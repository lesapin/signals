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
 *  @brief  Default numeric values for some signals in x86/ARM and most other systems.
 */
enum SignalCode
{
    SIGHUP = 1,
    SIGINT,
    SIGQUIT,
    SIGILL,
    SIGTRAP,
    SIGABRT,
    SIGBUS,
    SIGFPE,
    SIGKILL,
    SIGUSR1,
    SIGSEGv,
    SIGUSR2,
    SIGPIPE,
    SIGALRM,
    SIGTERM,
    SIGSTKFLT,
    SIGCHLD,
    SIGCONT,
    SIGSTOP,
    SIGTSTP,
    SIGTTIN,
    SIGTTOU,
    SIGURG,
    SIGXCPU,
    SIGXFSZ,
    SIGVTALRM,
    SIGPROF,
    SIGWINCH,
    SIGIO,
    SIGPWR,
    SIGSYS
}

/**
 *  @brief  Error codes that might be returned by CreateHandler.
 */
enum SignalError
{
    NoError = 0,
    Error,
    SigactionError,
    SAHandlerError,
    ForwardError,
    FuncCountError, // Not fatal, always check for this to be sure that no callbacks have
                    // been previously set for the signal by your/anyone else's plugin. 
                    // Happens often when plugins get unloaded and later try to reinitalize
                    // callbacks.
    CallbackError
}

/**
 *  @brief  Callback handler function type
 */
typedef SignalCallbackType = function Action () 

/**
 *  @brief  Add a callback as a signal handler.
 *
 *  @param signal        Numeric/enum value of the signal.
 *  @param callback      Function that gets called when the signal is received.
 *  @return              An error code indicating what went wrong.
 */
native int CreateHandler(SignalCode signal, SignalCallbackType cb);

/**
 *  @brief  Remove the handler associated with the signal.
 *
 *  @param signal       Numeric/enum value of the signal.
 */
native void RemoveHandler(SignalCode signal);




