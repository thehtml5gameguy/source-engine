/**
 * @file env_portal_laser.cpp
 * @brief Adds lasers from Portal 2
 * @date 2023-04-05
 */

#include "cbase.h"
#include "datamap.h"
#include "dt_common.h"
#include "dt_send.h"
#include "mathlib/vector.h"
#include "networkvar.h"
#include "physicsshadowclone.h"
#include "prop_weightedcube.h"
#include "prop_laser_catcher.h"
#include "util.h"
#include <cstddef>

ConVar sv_portal_laser_high_precision_update ( "portal_laser_high_precision_update", "0.03f", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar sv_portal_laser_normal_update ( "portal_laser_normal_update", "0.05f", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );

class CPortalLaser : public CBaseAnimating
{
public:
	DECLARE_CLASS( CPortalLaser, CBaseAnimating );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPortalLaser( void );

	virtual void Spawn( void );
	virtual void Precache( void );

	void TurnOn( void );
	void TurnOff( void );

private:
	void InputToggle( inputdata_t &inputData );
	void InputTurnOn( inputdata_t &inputData );
	void InputTurnOff( inputdata_t &inputData );

	void StrikeThink( void );

	CPortalLaser* m_pParentLaser;
	CPortalLaser* m_pChildLaser;

	Vector m_vecNearestSoundSource[33];
	CBaseEntity* m_pSoundProxy[33];
	CSoundPatch* m_pAmbientSound[33];
	//CInfoPlacementHelper* m_pPlacementHelper;
	int m_iLaserAttachment;

	bool m_bStartOff;
	bool m_bFromReflectedCube;
	bool m_bGlowInitialized;
	bool m_bAutoAimEnabled;
	bool m_bNoPlacementHelper;

	CNetworkHandle( CBaseEntity, m_hReflector);
	CNetworkVector( m_vStartPoint );
	CNetworkVector( m_vEndPoint );
	CNetworkVar( bool, m_bLaserOn );
	CNetworkVar( bool, m_bIsLethal );
	CNetworkVar( bool, m_bIsAutoAiming );
	CNetworkVar( bool, m_bShouldSpark );
	CNetworkVar( bool, m_bUseParentDir );
	CNetworkVector( m_angParentAngles );
	QAngle m_angPortalExitAngles;
};

// Start of our data description for the class
BEGIN_DATADESC( CPortalLaser )
	
	DEFINE_KEYFIELD( m_bStartOff, FIELD_BOOLEAN, "StartState" ),
	DEFINE_KEYFIELD( m_nSkin, FIELD_INTEGER, "skin" ),

	// Links our input name from Hammer to our input member function
	DEFINE_INPUTFUNC( FIELD_VOID, "Toggle", InputToggle ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOn", InputTurnOn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "TurnOff", InputTurnOff ),

	DEFINE_THINKFUNC( StrikeThink ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CPortalLaser, DT_PortalLaser)
	SendPropEHandle( SENDINFO( m_hReflector ) ),
	SendPropVector( SENDINFO( m_vStartPoint ), 32, SPROP_COORD ),
	SendPropVector( SENDINFO( m_vEndPoint ), 32, SPROP_COORD ),
	SendPropBool( SENDINFO( m_bLaserOn ) ), 
	SendPropBool( SENDINFO( m_bIsLethal ) ), 
	SendPropBool( SENDINFO( m_bIsAutoAiming ) ), 
	SendPropBool( SENDINFO( m_bShouldSpark ) ), 
	SendPropBool( SENDINFO( m_bUseParentDir ) ),
	SendPropVector( SENDINFO( m_angParentAngles ) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( env_portal_laser, CPortalLaser );

CPortalLaser::CPortalLaser( void )
	: m_pParentLaser( NULL ),
	m_pChildLaser( NULL ),
	m_bFromReflectedCube( false ),
	m_bAutoAimEnabled( true ),
	m_bNoPlacementHelper( false )
{
	//  IPortalLaserAutoList::IPortalLaserAutoList((IPortalLaserAutoList *)0x9215c9,(bool)((char)this + -0x38));
}

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CPortalLaser::Precache( void )
{
	if (m_bIsLethal)
		CBaseEntity::PrecacheScriptSound("LaserGreen.BeamLoop");
	else
		CBaseEntity::PrecacheScriptSound("Laser.BeamLoop");

	CBaseEntity::PrecacheScriptSound("Flesh.LaserBurn");
	CBaseEntity::PrecacheScriptSound("Player.PainSmall");
	PrecacheParticleSystem("laser_start_glow");
	PrecacheParticleSystem("reflector_start_glow");

	if (m_bFromReflectedCube)
	    return;

	const char* cModelName = m_ModelName.ToCStr();

	if(cModelName == nullptr || *cModelName == '\0')
		cModelName = "models/props/laser_emitter.mdl";

	CBaseEntity::PrecacheModel(cModelName);
}


//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPortalLaser::Spawn( void )
{
	m_bGlowInitialized = false;
	if(!m_bFromReflectedCube)
	{
		SetSolid( SOLID_VPHYSICS );

		m_iLaserAttachment = LookupAttachment("laser_attachment");
		if ( !m_iLaserAttachment )
		{
			Warning("env_portal_laser \'%s\' : model named \'%s\' does not have attachment \'laser_attachment\'\n", STRING(GetEntityName()), STRING(GetModelName()));
			return;
		}
	}

	for( int i; i <= 33; i++ )
	{
		m_pAmbientSound[i] = nullptr;
	}

	//CreateHelperEntities();

	if(!m_bStartOff)
		TurnOn();

	SetFadeDistance(-1.0f, 0.0);
}

void CPortalLaser::TurnOn( void )
{
	if(!m_bLaserOn)
		m_bLaserOn = true;

	if(m_pfnThink != nullptr)
		return;

	float fTime;
	if(m_bFromReflectedCube)
		fTime = sv_portal_laser_normal_update.GetFloat();
	else
		fTime = sv_portal_laser_high_precision_update.GetFloat();

	SetThink(&CPortalLaser::StrikeThink);
	SetNextThink(gpGlobals->curtime + fTime);
}
void CPortalLaser::TurnOff( void )
{
	if(m_bLaserOn)
		m_bLaserOn = false;

	// TODO: Implement
	//RemoveChildLaser();
	//TornOffGlow();
	//TornOffLaserSound();
	SetThink(nullptr);
}

//-----------------------------------------------------------------------------
// Purpose: Toggle the laser on and off
//-----------------------------------------------------------------------------
void CPortalLaser::InputToggle( inputdata_t &inputData )
{
	if(m_bLaserOn)
		TurnOff();
	else
		TurnOn();

	return;
}
void CPortalLaser::InputTurnOn( inputdata_t &inputData )
{
	TurnOn();
}
void CPortalLaser::InputTurnOff( inputdata_t &inputData )
{
	TurnOff();
}

//-----------------------------------------------------------------------------
// Purpose: Toggle the laser on and off
// TODO: Not how it should be implemented!
//-----------------------------------------------------------------------------
void CPortalLaser::StrikeThink( void )
{
	if(m_bLaserOn)
	{
		Ray_t rayDmg;
		Vector vForward;
		AngleVectors( GetAbsAngles(), &vForward, NULL, NULL );
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
			else if( FClassnameIs( traceDmg.m_pEnt, "prop_weighted_cube" ) && UTIL_IsReflectiveCube( traceDmg.m_pEnt ) )
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
}
