#pragma semicolon 1

#include <signals>
#include <sdktools>

#pragma newdecls required
#define PLUGIN_VERSION "1.0.0"

#define SHUTDOWNDELAY 60

public Plugin myinfo = 
{
    name = "Linux signals example",
    author = "bezdmn",
    description = "Sets callbacks to some Linux signals",
    version = PLUGIN_VERSION,
    url = ""
};

public void OnPluginStart()
{
    RegServerCmd("add_signal", SignalAdd);
    RegServerCmd("rem_signal", SignalRemove);

    CreateTimer(5.0, SetSignalCallbacks);

}

// Replace any signal handler. Do error checking.
public Action SignalAdd(int args)
{
    char Signal[4];
    GetCmdArg(1, Signal, sizeof(Signal));

    int err = CreateHandler(StringToInt(Signal), MyCallback);

    switch(err)
    {
        case view_as<int>(SigactionError):
        {
            LogError("sigaction() failed for signal %i.", err);
            SetFailState("SigactionError");
        }
        case  view_as<int>(SAHandlerError):
        {
            LogMessage("A previous handler existed for signal %i but was replaced.", err);
        }
        case view_as<int>(ForwardError):
        {
            LogError("CreateForwardEx() failed for signal %i.", err);
            SetFailState("ForwardError");
        }
        case view_as<int>(FuncCountError):
        {
            LogMessage("A callback is already set for signal %i. No handler created.", err);
        }
        case view_as<int>(CallbackError):
        {
            LogError("Couldn't attach a function to forward for signal %i", err);
            SetFailState("CallbackError");
        }
        case view_as<int>(Error):
        {
            SetFailState("Unexpected error occurred");
        }
        default: 
        {

        }
    }

    return Plugin_Continue;
}

public Action SignalRemove(int args)
{
    char signal[4];
    GetCmdArg(1, signal, sizeof(signal));
    RemoveHandler(StringToInt(signal));
    return Plugin_Continue;
}

Action SetSignalCallbacks(Handle timer)
{ 
    // Handle SIGINT (Ctrl-C in terminal) gracefully.
    if (CreateHandler(SIGINT, GracefulShutdown) != view_as<int>(NoError))
    {
        PrintToServer("Failed to attach callback for SIGINT. \
                       a handler might already exist for this signal.");
    }
    
    // ... but leave a way to shutdown the server instantly. 
    if (CreateHandler(SIGTERM, InstantShutdown) != view_as<int>(NoError))
    {
        PrintToServer("Failed to attach callback for SIGTERM. \
                       a handler might already exist for this signal.");
    }

    // Start and stop profiling.
    if (CreateHandler(SIGUSR1, StartVProf) != view_as<int>(NoError) ||
        CreateHandler(SIGUSR2, StopVProf) != view_as<int>(NoError))
    {
        PrintToServer("Failed to attach callbacks for SIGUSR: \
                       a handler might already exist for this signal.");
    }

    // Fix jittering issues on long-running maps by reloading the map.
    // SIGWINCH is ignored by default so we can repurpose it. 
    if (CreateHandler(SIGWINCH, ReloadMap) != view_as<int>(NoError))
    {
        PrintToServer("Failed to attach callback for SIGWINCH. \
                       a handler might already exist for this signal.");
    }

    return Plugin_Continue;
}

/****** CALLBACK FUNCTIONS ******/

Action MyCallback()
{
    PrintToServer("Callback");
    return Plugin_Continue;
}

Action GracefulShutdown()
{
    ForceRoundTimer(SHUTDOWNDELAY);

    CreateTimer(SHUTDOWNDELAY + 1.0, GameEnd);
    CreateTimer(SHUTDOWNDELAY + 10.0, ShutdownServer);

    PrintToChatAll("[SERVER] Shutting down in %i seconds for maintenance", SHUTDOWNDELAY);
    PrintToServer("Server shutdown in ~%i seconds", SHUTDOWNDELAY);

    return Plugin_Continue;
}

Action InstantShutdown()
{
    for (int client = 1; client < MaxClients; client++)
    {
        if (IsClientConnected(client))
        {
            // Send a user-friendly shutdown message
            KickClient(client, "Shutting down for maintenance");
        }
    }

    ServerCommand("exit");

    return Plugin_Continue;
}

Action StartVProf()
{
    ServerCommand("vprof_reset");

    ServerCommand("vprof_on");
    LogMessage("Started VProfiler");

    return Plugin_Continue;
}

Action StopVProf()
{
    char Previous[128];
    Handle ConLog = FindConVar("con_logfile");
    
    if (ConLog == null)
    {
        LogError("Failed to dump vprof log");
    }
    else
    {
        // Logfile gets dumped in the server root folder
        GetConVarString(ConLog, Previous, sizeof(Previous));
        SetConVarString(ConLog, "vprof.txt", false, false);

        //ServerCommand("con_logfile vprof.txt");
        ServerCommand("vprof_generate_report"); 
        ServerCommand("vprof_generate_report_hierarchy");
        ServerCommand("vprof_generate_report_map_load");

        //ServerCommand("con_logfile %s", Previous);
        SetConVarString(ConLog, Previous, false, false);
    }

    ServerCommand("vprof_off");
    LogMessage("Stopped VProfiler");

    delete ConLog;

    return Plugin_Continue;
}

Action ReloadMap()
{
    ForceRoundTimer(SHUTDOWNDELAY);

    CreateTimer(SHUTDOWNDELAY + 1.0, GameEnd);
    CreateTimer(SHUTDOWNDELAY + 10.0, ChangeLevel);

    PrintToChatAll("[SERVER] Reloading the map in %i seconds for maintenance", SHUTDOWNDELAY);

    return Plugin_Continue;
}

/****** HELPER FUNCTIONS ******/

void ForceRoundTimer(int seconds)
{
    int NewTimer = CreateEntityByName("team_round_timer");

    if (IsValidEntity(NewTimer))
    {
        int TimerEnt = -1,
            TimerEntKothRed = -1,
            TimerEntKothBlu = -1;

        TimerEnt = FindEntityByClassname(TimerEnt, "team_round_timer");
        TimerEntKothRed = FindEntityByClassname(TimerEntKothRed, "zz_red_koth_timer");
        TimerEntKothBlu = FindEntityByClassname(TimerEntKothBlu, "zz_blue_koth_timer");

        if (TimerEnt >= 1) // Delete all previous round timers
        {
            RemoveEntity(TimerEnt);    
        }
        else if (TimerEntKothBlu >= 1 || TimerEntKothRed >= 1)
        {
            RemoveEntity(TimerEntKothBlu);
            RemoveEntity(TimerEntKothRed);
        }
    
        HookSingleEntityOutput(NewTimer, "OnFinished", EndGame, true);
    }
    else
    {
        SetFailState("Unable to create round timer");
    }
   
    // setup the new round timer
    /*
    DispatchKeyValueInt(NewTimer, "m_nTimerInitialLength", seconds);
    DispatchKeyValueInt(NewTimer, "m_nTimerMaxLength", seconds);
    DispatchKeyValueInt(NewTimer, "m_nSetupTimeLength", 0);
    DispatchKeyValueInt(NewTimer, "m_bAutoCountdown", 1);
    DispatchKeyValueInt(NewTimer, "m_bShowInHUD", 1);
    DispatchKeyValueInt(NewTimer, "m_bShowTimeRemaining", 1);
    DispatchKeyValueInt(NewTimer, "m_bIsDisabled", 0);
    DispatchKeyValueInt(NewTimer, "m_bStartPaused", 0);
    */

    DispatchSpawn(NewTimer);

    SetVariantInt(seconds);
    AcceptEntityInput(NewTimer, "SetTime");
    SetVariantInt(seconds);
    AcceptEntityInput(NewTimer, "SetMaxTime");
    SetVariantInt(0);
    AcceptEntityInput(NewTimer, "SetSetupTime");
    SetVariantInt(1);
    AcceptEntityInput(NewTimer, "ShowInHud");
    SetVariantInt(1);
    AcceptEntityInput(NewTimer, "AutoCountdown");

    AcceptEntityInput(NewTimer, "Enable");
}

Action GameEnd(Handle timer)
{
    int EndGameEnt = -1;
    EndGameEnt = FindEntityByClassname(EndGameEnt, "game_end");

    if (EndGameEnt < 1)
        EndGameEnt = CreateEntityByName("game_end");

    if (IsValidEntity(EndGameEnt))
    {
        AcceptEntityInput(EndGameEnt, "EndGame");
    }
    else // just shutdown instantly
    {
        LogError("Couldn't create game_end entity. Shutting down");
        ServerCommand("exit");
    }

    return Plugin_Continue;
}

// TODO
Action EndGame(const char[] output, int caller, int activator, float delay)
{
    PrintToServer("EndGame entity output");
    return Plugin_Continue;
}

Action ShutdownServer(Handle timer)
{
    return InstantShutdown(); // compiler warnings
}

Action ChangeLevel(Handle timer)
{
    char CurrentMap[64];
    GetCurrentMap(CurrentMap, sizeof(CurrentMap));
    ForceChangeLevel(CurrentMap, "Map reload for maintenance");
    return Plugin_Continue;
}

