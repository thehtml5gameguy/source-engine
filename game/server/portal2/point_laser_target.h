//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//
#pragma once

#include "teimpact.h"
#include "cbase.h"

class CPortalLaserTarget : public CTEImpact
{
public:
	DECLARE_CLASS( CPortalLaserTarget, CBaseEntity );
	DECLARE_DATADESC();

	CPortalLaserTarget( void );

	virtual void Spawn( void );
	
	void DisableThink( void );

private:
	COutputEvent					m_OnPowered;
	COutputEvent					m_OnUnpowered;

	bool							m_bPowered;
	bool							m_bTerminalPoint;

	void* /* CCatcher* */			m_pCatcher;
	string_t						m_ModelName;
};