/**
 * @file prop_laser_catcher.cpp
 * @brief Adds laser catchers from Portal 2
 * @date 2023-04-05
 */

#include "prop_laser_catcher.h"
#include "const.h"
#include "particle_parse.h"

LINK_ENTITY_TO_CLASS( prop_laser_catcher, CPropLaserCatcher );

BEGIN_DATADESC( CPropLaserCatcher )

	DEFINE_FIELD( m_bActivated, FIELD_BOOLEAN ),

	DEFINE_THINKFUNC( CatcherThink ),

	DEFINE_KEYFIELD( m_bRusted, FIELD_BOOLEAN, "SkinType" ),

	DEFINE_OUTPUT( m_OnPowered, "OnPowered" ),
	DEFINE_OUTPUT( m_OnUnpowered, "OnUnpowered" ),

END_DATADESC()

CPropLaserCatcher::CPropLaserCatcher()
{
}

/**
 * @brief Precache assets used by the entity
 */
void CPropLaserCatcher::Precache( void )
{
	PrecacheModel( STRING( GetModelName() ) );
	PrecacheParticleSystem("laser_relay_powered");
	PrecacheScriptSound("prop_laser_catcher.poweron");
	PrecacheScriptSound("prop_laser_catcher.poweroff");
	PrecacheScriptSound("prop_laser_catcher.powerloop");

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
	SetMoveType( MOVETYPE_VPHYSICS );

	m_IdleSequence = LookupSequence("idle");
	m_PowerOnSequence = LookupSequence("spin");
	m_iTargetAttachment = LookupAttachment("laser_target");

	ResetSequence(m_PowerOnSequence);

	//SetSequence(  );

	SetThink( &CPropLaserCatcher::CatcherThink );
	SetNextThink( gpGlobals->curtime );
}

void CPropLaserCatcher::SetActivated( bool bActivate )
{
	if( m_bActivated != bActivate )
	{
		m_bActivated = bActivate;
		SetCatcherSkin();

		if( bActivate )
		{
			m_OnPowered.FireOutput( this, this );

			DispatchParticleEffect("laser_relay_powered",PATTACH_POINT_FOLLOW, this, "particle_emitter");

			EmitSound("prop_laser_catcher.poweron");
			EmitSound("prop_laser_catcher.powerloop");
			StopSound("prop_laser_catcher.poweroff");

			SetSequence( m_PowerOnSequence );
			SetPlaybackRate( 1.0f );
			UseClientSideAnimation();
		}
		else
		{
			m_OnUnpowered.FireOutput( this, this );

			StopParticleEffects(this);

			EmitSound("prop_laser_catcher.poweroff");
			StopSound("prop_laser_catcher.powerloop");

			SetSequence( m_IdleSequence );
			SetPlaybackRate( 1.0f );
			UseClientSideAnimation();
		}
	}
}

/**
 * @brief Updates the laser catcher
 */
void CPropLaserCatcher::CatcherThink( void )
{
	SetNextThink( gpGlobals->curtime + 0.2f );
}