//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: One of the two ends of a portal pair which can be picked up and placed by weapon_camera
//
//===========================================================================//

#include "point_laser_target.h"
#include "baseentity.h"
#include "string_t.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
//-----------------------------------------------------------------------------
// Context think
//-----------------------------------------------------------------------------

LINK_ENTITY_TO_CLASS( prop_button, CPortalLaserTarget );

BEGIN_DATADESC( CPortalLaserTarget )
	
	DEFINE_KEYFIELD( m_ModelName, FIELD_STRING, "ModelName" ),

	DEFINE_FIELD( m_bPowered, FIELD_BOOLEAN ),

	DEFINE_OUTPUT( m_OnPowered,			"OnPowered" ),
	DEFINE_OUTPUT( m_OnUnpowered,		"OnUnpowered" ),
	
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CPortalLaserTarget::CPortalLaserTarget( void )
{
}









//-----------------------------------------------------------------------------
// Purpose: Spawn the laser target.
//-----------------------------------------------------------------------------
void CPortalLaserTarget::Spawn( void )
{
	Precache();
	CBaseEntity::AddFlag(0x2000000);

	m_bPowered = false;

	
	
}

