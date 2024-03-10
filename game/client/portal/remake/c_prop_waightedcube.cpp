/** 
 * @file c_env_portal_laser.cpp
 * @brief Adds lasers from Portal 2
 * 
 * Mostly just copypasted from the rocket turret
 *
 * @bug Laser can be blocked by a player when laser goes through a portal.
 */

#include "cbase.h"
#include "c_physicsprop.h"
#include "beam_shared.h"
#include "mathlib/vector.h"
#include "portal_util_shared.h"

class C_PropWeightedCube : public C_PhysicsProp
{
public:
	DECLARE_CLASS( C_PropWeightedCube, C_PhysicsProp );
	DECLARE_CLIENTCLASS();

	C_PropWeightedCube( void );
	~C_PropWeightedCube( void );

	void Spawn( void );
	void ClientThink( void );

	void LaserOff( void );
	void LaserOn( void );
	float LaserEndPointSize( void );

private:
	CBeam *m_pBeam;
	CTraceFilterSkipTwoEntities m_filterBeams;

	int	m_nSiteHalo;
	int m_iAttachmentId;

	////////////
	//CNewParticleEffect m_pSparkEffect;
	bool m_bLaserOn;
	bool m_bIsLethal;
	////////////
};

LINK_ENTITY_TO_CLASS( prop_weighted_cube, C_PropWeightedCube );
IMPLEMENT_CLIENTCLASS_DT( C_PropWeightedCube, DT_PropWeightedCube, CPropWeightedCube )
	RecvPropBool( RECVINFO( m_bLaserOn ) ), 
	RecvPropBool( RECVINFO( m_bIsLethal ) ), 
END_RECV_TABLE()


C_PropWeightedCube::C_PropWeightedCube( void )
	: m_filterBeams( this, UTIL_PlayerByIndex( 1 ), COLLISION_GROUP_DEBRIS )
	//m_pSparkEffect()
{
}

C_PropWeightedCube::~C_PropWeightedCube( void )
{
	LaserOff();
	if( m_pBeam )
		m_pBeam->Remove();
}

void C_PropWeightedCube::Spawn()
{
	SetThink( &C_PropWeightedCube::ClientThink );
	SetNextClientThink( CLIENT_THINK_ALWAYS );

	m_pBeam = NULL;

	BaseClass::Spawn();
}

void C_PropWeightedCube::ClientThink()
{
	if( m_bLaserOn )
		LaserOn();
	else
		LaserOff();
}

void C_PropWeightedCube::LaserOff()
{
	if( m_pBeam )
		m_pBeam->AddEffects( EF_NODRAW );
}

void C_PropWeightedCube::LaserOn()
{
	if ( !IsBoneAccessAllowed() )
	{
		LaserOff();
		return;
	}

	Vector vecMuzzle;
	QAngle angMuzzleDir;
	GetAttachment( m_iAttachmentId, vecMuzzle, angMuzzleDir );
	
	QAngle angAimDir = GetAbsAngles();
	Vector vecAimDir;
	AngleVectors ( angAimDir, &vecAimDir );

	if(!m_pBeam)
	{
		if(m_bIsLethal)
			m_pBeam = CBeam::BeamCreate("sprites/laserbeam.vmt", 2);
		else
			m_pBeam = CBeam::BeamCreate("sprites/purplelaser1.vmt", 32);;
		m_pBeam->SetHaloTexture( m_nSiteHalo );
		m_pBeam->SetColor( 255, 255, 255 );
		m_pBeam->SetBrightness( 255 );
		m_pBeam->SetNoise( 0 );
		m_pBeam->SetWidth( 2 );
		m_pBeam->SetEndWidth( 0 );
		m_pBeam->SetScrollRate( 0 );
		m_pBeam->SetFadeLength( 0 );
		m_pBeam->SetHaloScale( 5.5f );
		m_pBeam->SetBeamFlag( FBEAM_REVERSED );
		m_pBeam->SetCollisionGroup( COLLISION_GROUP_NONE );
		m_pBeam->PointsInit( vecMuzzle + vecAimDir, vecMuzzle );
		m_pBeam->SetStartEntity( this );
	}
	else
	{
		m_pBeam->RemoveEffects( EF_NODRAW );
	}

	// Trace to find an endpoint (so the beam draws through portals)
	Vector vEndPoint;
	float fEndFraction;
	Ray_t rayPath;
	rayPath.Init( vecMuzzle, vecMuzzle + vecAimDir * 8192 );

	if ( UTIL_Portal_TraceRay_Beam( rayPath, MASK_SHOT, &m_filterBeams, &fEndFraction ) )
		vEndPoint = vecMuzzle + vecAimDir * 8192; // Trace went through portal and endpoint is unknown
	else
		vEndPoint = vecMuzzle + vecAimDir * 8192 * fEndFraction; // Trace hit a wall

	m_pBeam->PointsInit( vEndPoint, vecMuzzle );
}
