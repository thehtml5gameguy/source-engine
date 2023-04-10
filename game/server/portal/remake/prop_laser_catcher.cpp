/**
 * @file prop_laser_catcher.cpp
 * @brief Adds laser catchers from Portal 2
 * @date 2023-04-05
 */

#include "prop_laser_catcher.h"

LINK_ENTITY_TO_CLASS( prop_laser_catcher, CPropLaserCatcher );

BEGIN_DATADESC( CPropLaserCatcher )

	DEFINE_FIELD( m_bActivated, FIELD_BOOLEAN ),

	DEFINE_THINKFUNC( CatcherThink ),

	DEFINE_KEYFIELD( m_bRusted, FIELD_BOOLEAN, "SkinType" ),

	DEFINE_OUTPUT( m_OnPowered, "OnPowered" ),
	DEFINE_OUTPUT( m_OnUnpowered, "OnUnpowered" ),

END_DATADESC()

CPropLaserCatcher::CPropLaserCatcher()
	: m_bAlreadyActivated( false )
{
}

/**
 * @brief Precache assets used by the entity
 */
void CPropLaserCatcher::Precache( void )
{
	PrecacheModel( STRING( GetModelName() ) );

	BaseClass::Precache();
}

void CPropLaserCatcher::SetCatcherSkin( void )
{
	if(m_bRusted) // TODO: Make a skin enum
	{
		if(m_bActivated)
			SetSkin(3);
		else
			SetSkin(2);
	}
	else 
	{
		if(m_bActivated)
			SetSkin(1);
		else
			SetSkin(0);
	}
}

void CPropLaserCatcher::SetSkin( int skinNum )
{
	m_nSkin = skinNum;
}

/**
 * @brief Sets up the entity's initial state
 */
void CPropLaserCatcher::Spawn( void )
{
	Precache();

	SetModel( STRING( GetModelName() ) );
	SetSolid( SOLID_VPHYSICS );
	CreateVPhysics();
	SetSequence( LookupSequence("idle") );

	SetThink( &CPropLaserCatcher::CatcherThink );
	SetNextThink( gpGlobals->curtime );
}

void CPropLaserCatcher::SetActivated( bool bActivate )
{
	m_bActivated = bActivate;
}

/**
 * @brief Updates the laser catcher
 */
void CPropLaserCatcher::CatcherThink( void )
{
	if( m_bActivated )
	{
		m_OnPowered.FireOutput( this, this );
		SetCatcherSkin();
		SetSequence( LookupSequence("spin") );
		m_bActivated = false; // This is a dumb way of making the catcher power off.
	}
	else
	{
	 	m_OnUnpowered.FireOutput( this, this );
		SetCatcherSkin();
		SetSequence( LookupSequence("idle") );
	}
	SetNextThink( gpGlobals->curtime + 0.2f );
}