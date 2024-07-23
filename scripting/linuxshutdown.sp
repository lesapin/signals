#pragma semicolon 1

#include <sourcemod>
#include <sdktools>
#include <signals>
#include <morecolors>

#pragma newdecls required

#define PL_VERSION "1.1.8"

public Plugin myinfo = 
{
    	name = "Linux Shutdown",
    	author = "bzdmn",
    	description = "Shutdown the server gracefully when a Linux signal is fired",
    	version = PL_VERSION,
    	url = "https://mge.me"
};

#define SHUTDOWNDELAY 60

bool mapIsLoaded = false;

public void OnPluginStart()
{
    	// Handle SIGTERM (Ctrl-C in terminal) gracefully.
	// https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html#TimeoutSec=
    	SetSignalCallback(TERM, GracefulShutdown);
}

public void OnMapStart()
{
	mapIsLoaded = true;
}

public void OnMapEnd()
{
	mapIsLoaded = false;
}

void SetSignalCallback(SIG signal, SignalCallbackType cb)
{
    	int err = CreateHandler(signal, cb);
    	if (err == view_as<int>(FuncCountError)) // Callback already exists probably because of a plugin reload. 
    	{
        	LogMessage("Resetting handler for signal %i", signal);

        	// Remove the previous handler and try again
        	RemoveHandler(signal);
        	err = CreateHandler(signal, cb);
    	}
    	else if (err == view_as<int>(SAHandlerError))
    	{
        	// Signal handler was set, not neccessarily by this extension but by the process.
        	// This error is like a confirmation that we really want to replace the handler.
        	LogError("A handler set by another process was replaced");

        	// Ignore the previous handler. Someone else should deal with it.
        	RemoveHandler(signal);
        	err = CreateHandler(signal, cb);
    	}

    	if (err != view_as<int>(NoError))
    	{
        	LogError("Critical error, code %i", err);
        	SetFailState("ERR: %i. Failed to attach callback for signal %i", err, signal);
    	}

    	LogMessage("Hooked signal %i", signal);
}

Action GracefulShutdown()
{
    	if (GetClientCount(true) == 0) // zero clients in-game
    	{
        	LogMessage("No clients in-game, shutting down instantly");
        	ServerCommand("exit");
    	}
    	else
    	{
       		// sv_shutdown shuts down the server after sv_shutdown_timeout_minutes,
       		// or after every player has left/gets kicked from the server.
		// https://github.com/ValveSoftware/Source-1-Games/issues/1726
    		ServerCommand("sv_shutdown");
    		////////////////////////////////// 
    		
		LogMessage("Server shutting down in ~%i seconds", SHUTDOWNDELAY);
    	}

    	ForceRoundTimer(SHUTDOWNDELAY);

    	CreateTimer(SHUTDOWNDELAY + 1.0, EndGame);
    	CreateTimer(SHUTDOWNDELAY + 10.0, KickClients);

    	MC_PrintToChatAll("{yellow}[SERVER] Shutting down in %i seconds for maintenance", SHUTDOWNDELAY);

    	return Plugin_Continue;
}

void ForceRoundTimer(int seconds)
{
	//char buf[4];
	//if (GetCurrentMap(buf, sizeof(buf)) > 0) // a map is running
	if (isMapLoaded)
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
	    
		int NewTimer = CreateEntityByName("team_round_timer");
		
		if (!IsValidEntity(NewTimer)) // Try to create a new timer entity
		{
		    	LogError("Could not create team_round_timer entity");
		}
		else
		{
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
	}
}

Action KickClients(Handle timer)
{
    	LogMessage("Server shutting down");

    	for (int client = 1; client < MaxClients; client++)
    	{
        	if (IsClientConnected(client) || IsClientAuthorized(client))
        	{
            		// Send a user-friendly shutdown message
            		KickClient(client, "Shutting down for maintenance");
        	}
    	}

	return Plugin_Continue;
}

Action EndGame(Handle timer)
{
    	int EndGameEnt = -1;
    	EndGameEnt = FindEntityByClassname(EndGameEnt, "game_end");

    	if (EndGameEnt < 1)
        	EndGameEnt = CreateEntityByName("game_end");

    	if (IsValidEntity(EndGameEnt))
    	{
        	AcceptEntityInput(EndGameEnt, "EndGame");
    	}
    	else
    	{
        	LogError("Could not create game_end entity");
    	}

    	return Plugin_Continue;
}
