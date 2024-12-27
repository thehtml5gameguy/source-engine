//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engineer's Teleporter
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_OBJ_TELEPORTER_H
#define TF_OBJ_TELEPORTER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

// ------------------------------------------------------------------------ //
// Base Teleporter object
// ------------------------------------------------------------------------ //
class CObjectTeleporter : public CBaseObject
{
	DECLARE_CLASS( CObjectTeleporter, CBaseObject );

public:
	DECLARE_SERVERCLASS();

	CObjectTeleporter();

	virtual void	Spawn();
	virtual void	Precache();
	virtual bool	StartBuilding( CBaseEntity *pBuilder );
	virtual void	OnGoActive( void );
	virtual int		DrawDebugTextOverlays(void) ;
	virtual bool	IsPlacementPosValid( void );
	virtual void	SetModel( const char *pModel );

	virtual void	FinishedBuilding( void );

	void SetState( int state );
	virtual void	DeterminePlaybackRate( void );

	void TeleporterThink( void );
	void TeleporterTouch( CBaseEntity *pOther );

	virtual void TeleporterSend( CTFPlayer *pPlayer ) { Assert(0); }
	virtual void TeleporterReceive( CTFPlayer *pPlayer, float flDelay ) { Assert(0); }

	CObjectTeleporter *GetMatchingTeleporter( void );
	CObjectTeleporter *FindMatch( void );	// Find the teleport partner to this object

	bool IsReady( void );					// is this teleporter connected and functional? (ie: not sapped, disabled, upgrading, unconnected, etc)
	bool IsMatchingTeleporterReady( void );
	bool IsSendingPlayer( CTFPlayer *player );	// returns true if we are in the process of teleporting the given player

	virtual bool IsEntrance( void ) = 0;
	virtual bool IsExit( void ) = 0;

	int GetState( void ) { return m_iState; }	// state of the object ( building, charging, ready etc )

	void SetTeleportingPlayer( CTFPlayer *pPlayer )
	{
		m_hTeleportingPlayer = pPlayer;
	}

	//TF_MOD_BOT changes
	CTFPlayer *GetTeleportingPlayer()
	{
		return m_hTeleportingPlayer.Get();
	}

protected:
	CNetworkVar( int, m_iState );
	CNetworkVar( float, m_flRechargeTime );
	CNetworkVar( int, m_iTimesUsed );
	CNetworkVar( float, m_flYawToExit );

	CHandle<CObjectTeleporter> m_hMatchingTeleporter;

	float m_flLastStateChangeTime;

	float m_flMyNextThink;	// replace me

	CHandle<CTFPlayer> m_hTeleportingPlayer;

	float m_flNextEnemyTouchHint;

	// Direction Arrow, shows roughly what direction the exit is from the entrance
	void ShowDirectionArrow( bool bShow );

	bool m_bShowDirectionArrow;
	int m_iDirectionBodygroup;
	int m_iBlurBodygroup;

private:
	DECLARE_DATADESC();
};

class CObjectTeleporter_Entrance : public CObjectTeleporter
{
public:
	DECLARE_CLASS( CObjectTeleporter_Entrance, CObjectTeleporter );

	CObjectTeleporter_Entrance();

	virtual void Spawn();
	virtual void TeleporterSend( CTFPlayer *pPlayer );
	virtual bool IsEntrance( void ) { return true; };
	virtual bool IsExit( void )  { return false; };;
};

class CObjectTeleporter_Exit : public CObjectTeleporter
{
public:
	DECLARE_CLASS( CObjectTeleporter_Exit, CObjectTeleporter );

	CObjectTeleporter_Exit();

	virtual void Spawn();
	virtual void TeleporterReceive( CTFPlayer *pPlayer, float flDelay );
	virtual bool IsEntrance( void ) { return false; };
	virtual bool IsExit( void )  { return true; };;
};

#endif // TF_OBJ_TELEPORTER_H
