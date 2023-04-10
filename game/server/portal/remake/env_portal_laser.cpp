/**
 * @file env_portal_laser.cpp
 * @brief Adds lasers from Portal 2
 * @date 2023-04-05
 */

#include "cbase.h"
#include "datamap.h"
#include "physicsshadowclone.h"
#include "prop_weightedcube.h"
#include "prop_laser_catcher.h"

class CPortalLaser : public CBaseAnimating
{
public:
	DECLARE_CLASS( CPortalLaser, CBaseAnimating );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPortalLaser( void );

	virtual void Spawn( void );
	virtual void Precache( void );

private:
	void InputToggle( inputdata_t &inputData );
	void InputTurnOn( inputdata_t &inputData );
	void InputTurnOff( inputdata_t &inputData );

	void LaserThink( void );

	CTraceFilterSkipTwoEntities m_filterBeams;

	bool m_bStartOff;

	CNetworkVar( bool, m_bActive);
	CNetworkVar( int, m_nSiteHalo );
	CNetworkVar( int, m_iAttachmentId );
	CNetworkVar( QAngle, m_vecCurrentAngles );
};

// Start of our data description for the class
BEGIN_DATADESC( CPortalLaser )
	
	// Save/restore our active state
	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),

	DEFINE_KEYFIELD( m_bStartOff, FIELD_BOOLEAN, "StartState" ),
	DEFINE_KEYFIELD( m_nSkin, FIELD_INTEGER, "skin" ),

	// Links our input name from Hammer to our input member function
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOn", InputTurnOn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOff", InputTurnOff ),

	DEFINE_THINKFUNC( LaserThink ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CPortalLaser, DT_PortalLaser)
	SendPropBool( SENDINFO( m_bActive )),
	SendPropInt( SENDINFO( m_nSiteHalo ) ),
	SendPropInt( SENDINFO( m_iAttachmentId ), 2 ),
	SendPropVector( SENDINFO( m_vecCurrentAngles ) ), 
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( env_portal_laser, CPortalLaser );

CPortalLaser::CPortalLaser( void )
	: m_filterBeams( NULL, NULL, COLLISION_GROUP_DEBRIS )
{
	m_filterBeams.SetPassEntity( this );
}

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CPortalLaser::Precache( void )
{
	PrecacheModel( STRING( GetModelName() ) );
	PrecacheModel("effects/redlaser1.vmt");
	m_nSiteHalo = PrecacheModel("sprites/light_glow03.vmt");

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPortalLaser::Spawn( void )
{
	Precache();

	m_vecCurrentAngles = GetAbsAngles();

	SetModel( STRING( GetModelName() ) );
	SetSolid( SOLID_VPHYSICS );
	CreateVPhysics();

	m_iAttachmentId = LookupAttachment("laser_attachment");
	if ( !m_iAttachmentId )
	{ // TODO: Figure out what the first string is.
		Warning("env_portal_laser \'%s\' : model named \'%s\' does not have attachment \'laser_attachment\'\n", "TODO", STRING(GetModelName()));
		return;
	}

	m_bActive = !m_bStartOff;

	SetFadeDistance(-1.0f, 0.0);

	SetThink( &CPortalLaser::LaserThink );
	SetNextThink( gpGlobals->curtime );
}

//-----------------------------------------------------------------------------
// Purpose: Toggle the laser on and off
//-----------------------------------------------------------------------------
void CPortalLaser::InputToggle( inputdata_t &inputData )
{
	m_bActive = !m_bActive;
}
void CPortalLaser::InputTurnOn( inputdata_t &inputData )
{
	m_bActive = true;
}
void CPortalLaser::InputTurnOff( inputdata_t &inputData )
{
	m_bActive = false;
}

//-----------------------------------------------------------------------------
// Purpose: Toggle the laser on and off
//-----------------------------------------------------------------------------
void CPortalLaser::LaserThink( void )
{
	if(m_bActive)
	{
		Ray_t rayDmg;
		Vector vForward;
		AngleVectors( m_vecCurrentAngles, &vForward, NULL, NULL );
		Vector vEndPoint = EyePosition() + vForward*8192;
		rayDmg.Init( EyePosition(), vEndPoint );
		rayDmg.m_IsRay = true;
		trace_t traceDmg;

		// This version reorients through portals
		CTraceFilterSimple subfilter( this, COLLISION_GROUP_NONE );
		CTraceFilterTranslateClones filter ( &subfilter );
		float flRequiredParameter = 2.0f;
		CProp_Portal* pFirstPortal = UTIL_Portal_FirstAlongRay( rayDmg, flRequiredParameter );
		UTIL_Portal_TraceRay_Bullets( pFirstPortal, rayDmg, MASK_VISIBLE_AND_NPCS, &filter, &traceDmg, false );

		if ( traceDmg.m_pEnt )
		{
			if ( traceDmg.m_pEnt->IsPlayer() )
			{
				CTakeDamageInfo dmgInfo;
				dmgInfo.SetDamage( 2 );
				dmgInfo.SetDamageType( DMG_ENERGYBEAM );
				traceDmg.m_pEnt->TakeDamage( dmgInfo );
			}
			else if( FClassnameIs( traceDmg.m_pEnt, "prop_weighted_cube" ) /* UTIL_IsReflectiveCube( traceDmg.m_pEnt ) */ )
			{
				//Set the cube to activate
				// TODO: Make lasers come from cubes
				CPropWeightedCube* pCube = assert_cast<CPropWeightedCube*>( traceDmg.m_pEnt );
				if( pCube )
				{
					pCube->SetActivated( true );
				}
			}
			else if( FClassnameIs( traceDmg.m_pEnt, "prop_laser_catcher" ))
			{
				CPropLaserCatcher* pCatcher = assert_cast<CPropLaserCatcher*>( traceDmg.m_pEnt );
				if( pCatcher )
				{
					pCatcher->SetActivated( true );
				}
			}
		}
	}

	SetNextThink( gpGlobals->curtime + 0.1f );
}
