#pragma once

#include "SDK.h"
#include "FunctionRoute.h"

#include "valve_minmax_off.h"
#include <mutex>
#include <map>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
#define INVALID_SOCKET -1
#endif

class CTFTrue: public IServerPluginCallbacks
{
public:
	// IServerPluginCallbacks methods
	virtual bool			Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	virtual void			Unload( void );
	virtual void			Pause( void ) {}
	virtual void			UnPause( void ) {}
	virtual const char     *GetPluginDescription( void );
	virtual void			LevelInit( char const *pMapName ){}
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void ){}
	virtual void			ClientActive( edict_t *pEntity ) { }
	virtual void			ClientDisconnect( edict_t *pEntity );
	virtual void			ClientPutInServer( edict_t *pEntity, char const *playername ){}
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( edict_t *pEdict );
	virtual PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen ) {return PLUGIN_CONTINUE;}
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity, const CCommand &args );
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID ) {return PLUGIN_CONTINUE;}
	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue ){}
	virtual void			OnEdictAllocated( edict_t *edict ){}
	virtual void			OnEdictFreed( const edict_t *edict  ){}

	int GetCommandIndex() { return m_iClientCommandIndex; }

	void SetNextMap(const char *szNextMap);
	const char *GetNextMap() { return m_szNextMap; }

	enum eForceChangeMap
	{
		FORCE_NONE = 0,
		FORCE_RELOADMAP,
		FORCE_NEWMAP
	};

	void ForceChangeMap( eForceChangeMap forcechangemap, float flTime = 0.0f);
	eForceChangeMap GetForceChangeMap() { return m_ForceChangeMap ; }

	void UpdateGameDesc();

	static void Version_Callback( IConVar *var, const char *pOldValue, float flOldValue );
	static void GameDesc_Callback( IConVar *var, const char *pOldValue, float flOldValue );
	static void Freezecam_Callback( IConVar *var, const char *pOldValue, float flOldValue );

	edict_t *m_pEdictList;

	void ForwardCommand(ConCommand *pCmd, const CCommand &args);
private:
	bool m_bReloadedNeeded = false;
	int m_iLoadCount = 0;
	int m_iClientCommandIndex = 0;

	eForceChangeMap m_ForceChangeMap = FORCE_NONE;
	float m_flNextMapChange = 0.0f;
	char m_szNextMap[150] = {};

	static const char* __fastcall GetGameDescription(IServerGameDLL *gamedll EDX2);
	static void __fastcall ChangeLevel(IVEngineServer *pServer, EDX const char *s1, const char *s2);
	char m_szGameDesc[50] = {};

	CFunctionRoute GetGameDescriptionRoute;
	CFunctionRoute ChangeLevelRoute;

	// Command dispatch hook for server.so
	static void	DispatchCommandServer(ConCommand *pCmd, const CCommand &args);
	CFunctionRoute m_DispatchRouteServer;

	// Command dispatch hook for engine.so
	static void	DispatchCommandEngine(ConCommand *pCmd, const CCommand &args);
	CFunctionRoute m_DispatchRouteEngine;

	bool HandleSayCommand(ConCommand *pCmd, const CCommand &args);
};

extern CTFTrue g_Plugin;

extern ConVar tftrue_gamedesc;
extern ConVar tftrue_freezecam;
extern ConVar tftrue_version;

#ifdef _LINUX
#define GetFuncAddress(pAddress, szFunction) dlsym(pAddress, szFunction)
#else
#define GetFuncAddress(pAddress, szFunction) ::GetProcAddress((HMODULE)pAddress, szFunction)
#endif
