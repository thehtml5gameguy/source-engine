/**
 * @file prop_weightedcube.cpp
 * @brief First-class cube entity so we can query by type and generally make inferences that are harder to do without an entity of that type.
 * @date 2023-04-05
 * 
 * @copyright Copyright � 1996-2009, Valve Corporation, All rights reserved.
 * @attention Partially stolen from the CS:GO leak!
 */

#include "prop_weightedcube.h"
#include "datacache/imdlcache.h"
#include "portal_util_shared.h"

#include <cstddef>

LINK_ENTITY_TO_CLASS( prop_weighted_cube, CPropWeightedCube );

BEGIN_DATADESC( CPropWeightedCube )

	DEFINE_FIELD( m_bActivated, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bPickupDisabled, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bTouchedByPlayer, FIELD_BOOLEAN ),

	DEFINE_KEYFIELD( m_bRusted, FIELD_BOOLEAN, "SkinType" ),
	DEFINE_KEYFIELD( m_nCubeType, FIELD_INTEGER, "CubeType" ),
	DEFINE_KEYFIELD( m_bNewSkins, FIELD_BOOLEAN, "NewSkins" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Dissolve", InputDissolve ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SilentDissolve", InputDissolve ), // InputSilentDissolve


END_DATADESC()

const char *CUBE_MODEL = "models/props/metal_box.mdl";
const char *CUBE_REFLECT_MODEL = "models/props/reflection_cube.mdl";
const char *CUBE_SPHERE_MODEL = "models/props_gameplay/mp_ball.mdl";
const char *CUBE_FX_FIZZLER_MODEL = "models/props/metal_box_fx_fizzler.mdl";
const char *CUBE_ANTIQUE_MODEL = "models/props_underground/underground_weighted_cube.mdl";
const char *CUBE_SCHRODINGER_MODEL = "models/props/reflection_cube.mdl";

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CPropWeightedCube::CPropWeightedCube()
				 : m_bRusted( false ),
				   m_bActivated( false ),
				   m_nCubeType( CUBE_STANDARD ),
				   m_bTouchedByPlayer( false )
{
}

// TODO: This is just a placeholder!
PaintPowerType CPropWeightedCube::GetPaintedPower( void )
{
	return NO_POWER;
}

//-----------------------------------------------------------------------------
// Purpose: Dissolve when prompted
//-----------------------------------------------------------------------------
void CPropWeightedCube::InputDissolve( inputdata_t &in )
{
	UTIL_Remove( this ); // TODO: Add dissolving effect!
}

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CPropWeightedCube::Precache( void )
{
	ConvertOldSkins();
	
	switch ( m_nCubeType )
	{
	default:
	case CUBE_STANDARD:
	case CUBE_COMPANION:
		PrecacheModel( CUBE_MODEL );
		break;

	case CUBE_REFLECTIVE:
		PrecacheModel( CUBE_REFLECT_MODEL );
		break;

	case CUBE_SPHERE:
		PrecacheModel( CUBE_SPHERE_MODEL );
		break;

	case CUBE_ANTIQUE:
		PrecacheModel( CUBE_ANTIQUE_MODEL );

	case CUBE_SCHRODINGER:
		PrecacheModel( CUBE_SCHRODINGER_MODEL );
		PrecacheScriptSound( "music.laser_node_02.play" );
		PrecacheScriptSound( "prop_laser_catcher.poweron" );
		PrecacheScriptSound( "prop_laser_catcher.poweroff" );
		break;
	}

	PrecacheModel( CUBE_FX_FIZZLER_MODEL );

	PrecacheScriptSound( "WeightedCube.JumpPowerActivateShort" );
	PrecacheScriptSound( "WeightedCube.JumpPowerActivateLong" );

	BaseClass::Precache();
}

void CPropWeightedCube::InputDisablePickup( inputdata_t &in )
{
	m_bPickupDisabled = true;
}

void CPropWeightedCube::InputEnablePickup( inputdata_t &in )
{
	m_bPickupDisabled = false;
}

void CPropWeightedCube::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if( m_bPickupDisabled == false )
	{
		BaseClass::Use( pActivator, pCaller, useType, value );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if pEntity is prop_weighted_cube
//-----------------------------------------------------------------------------
bool UTIL_IsWeightedCube( CBaseEntity *pEntity )
{
	if ( pEntity == NULL )
		return false;

	return ( FClassnameIs( pEntity, "prop_weighted_cube" ) );
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if pEntity is prop_weighted_cube with CUBE_REFLECTIVE type
//-----------------------------------------------------------------------------
bool UTIL_IsReflectiveCube( CBaseEntity *pEntity )
{
	if ( UTIL_IsWeightedCube( pEntity ) == false )
		return false;

	CPropWeightedCube *pCube = assert_cast<CPropWeightedCube*>( pEntity );
	return ( pCube && pCube->GetCubeType() == CUBE_REFLECTIVE );
}

void CPropWeightedCube::StartTouch( CBaseEntity *pOther )
{
	// TODO: Implement m_bMovementDisabled
	/*if( m_bMovementDisabled )
	{
		if( pOther->IsPlayer() )
		{
			Vector vecPlayerForward;
			AngleVectors( pOther->EyeAngles(), &vecPlayerForward );
			vecPlayerForward.NormalizeInPlace();
			Vector vecCubeToPlayer = (GetAbsOrigin() - pOther->EyePosition()).Normalized();

			float flPlayerLookDot = DotProduct( vecCubeToPlayer, vecPlayerForward );
			float flCubeDirDot = DotProduct( Forward().Normalized(), vecPlayerForward );

			//DevMsg( "Dot:%f, CubeDot:%f\n", flPlayerLookDot, flCubeDirDot );

			//If the cube is in front of the player
			if( ( flPlayerLookDot > 0.8f && flCubeDirDot > 0.8f ) || ( flPlayerLookDot > 0.85f ) )
			{
				ExitDisabledState();
			}
		}
	}*/
	
	if( pOther->IsPlayer() )
	{
		m_bTouchedByPlayer = true;
	}

	BaseClass::StartTouch( pOther );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropWeightedCube::SetCubeType( void )
{
	// FIXME: Remove for DLC2
	if ( m_nCubeType == CUBE_SCHRODINGER )
	{
	 	m_nCubeType = CUBE_REFLECTIVE;
	}
	
	switch( m_nCubeType )
	{
		//Standard cube
		case CUBE_STANDARD:
		case CUBE_COMPANION:
		{
			SetModelName( MAKE_STRING( CUBE_MODEL ) );
			break;
		}
		
		//Reflective cube
		case CUBE_REFLECTIVE:
		{
			SetModelName( MAKE_STRING( CUBE_REFLECT_MODEL ) );
			AddSpawnFlags( SF_PHYSPROP_ENABLE_ON_PHYSCANNON );
			break;
		}
		
		//Sphere
		case CUBE_SPHERE:
		{
			SetModelName( MAKE_STRING( CUBE_SPHERE_MODEL ) );
			break;
		}
		
		//Antique cube
		case CUBE_ANTIQUE:
		{
			SetModelName( MAKE_STRING( CUBE_ANTIQUE_MODEL ) );
			break;
		}
	}

	SetCubeSkin(); // TODO: Add cube skins
}

//-----------------------------------------------------------------------------
// Purpose: Changes the cube texture to use the activated one
//-----------------------------------------------------------------------------
void CPropWeightedCube::SetActivated( bool bActivate )
{
	m_bActivated = bActivate;

	SetCubeSkin();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropWeightedCube::SetCubeSkin( void )
{
	switch( m_nCubeType )
	{
		//Standard cube
		case CUBE_STANDARD:
		{
			//Rusted cubes don't show paint
			if( m_bRusted )
			{
				if( m_bActivated )
				{
					SetSkin( CUBE_STANDARD_RUSTED_ACTIVATED_SKIN );
				}
				else
				{
					SetSkin( CUBE_STANDARD_RUSTED_SKIN );
				}
			}
			else
			{
				switch( GetPaintedPower() )
				{
					//Bounce painted
					case BOUNCE_POWER:
					{
						if( m_bActivated )
						{
							SetSkin( CUBE_STANDARD_BOUNCE_ACTIVATED_SKIN );
						}
						else
						{
							SetSkin( CUBE_STANDARD_BOUNCE_SKIN );
						}
					}
					break;
					//Speed painted
					case SPEED_POWER:
					{
						if( m_bActivated )
						{
							SetSkin( CUBE_STANDARD_SPEED_ACTIVATED_SKIN );
						}
						else
						{
							SetSkin( CUBE_STANDARD_SPEED_SKIN );
						}
					}
					break;
					//Not painted
					default:
					{
						if( m_bActivated )
						{
							SetSkin( CUBE_STANDARD_CLEAN_ACTIVATED_SKIN );
						}
						else
						{
							SetSkin( CUBE_STANDARD_CLEAN_SKIN );
						}
					}
					break;
				}
			}
		}
		break;
		//Companion cube
		case CUBE_COMPANION:
		{
			switch( GetPaintedPower() )
			{
				//Bounce painted
				case BOUNCE_POWER:
				{
					if( m_bActivated )
					{
						SetSkin( CUBE_COMPANION_BOUNCE_ACTIVATED_SKIN );
					}
					else
					{
						SetSkin( CUBE_COMPANION_BOUNCE_SKIN );
					}
				}
				break;
				//Speed painted
				case SPEED_POWER:
				{
					if( m_bActivated )
					{
						SetSkin( CUBE_COMPANION_SPEED_ACTIVATED_SKIN );
					}
					else
					{
						SetSkin( CUBE_COMPANION_SPEED_SKIN );
					}
				}
				break;
				//Not painted
				default:
				{
					if( m_bActivated )
					{
						SetSkin( CUBE_COMPANION_CLEAN_ACTIVATED_SKIN );
					}
					else
					{
						SetSkin( CUBE_COMPANION_CLEAN_SKIN );
					}
				}
				break;
			}
		}
		break;
		//Reflective cube
		case CUBE_REFLECTIVE:
		{
			switch( GetPaintedPower() )
			{
				//Bounce painted
			case BOUNCE_POWER:
				{
					if( m_bRusted )
					{
						// FIXME
						SetSkin( CUBE_REFLECTIVE_BOUNCE_SKIN );
					}
					else
					{
						SetSkin( CUBE_REFLECTIVE_BOUNCE_SKIN );
					}
				}
				break;
				//Speed painted
			case SPEED_POWER:
				{
					if( m_bRusted )
					{
						// FIXME
						SetSkin( CUBE_REFLECTIVE_SPEED_SKIN );
					}
					else
					{
						SetSkin( CUBE_REFLECTIVE_SPEED_SKIN );
					}
				}
				break;
				//Not painted
			default:
				{
					if( m_bRusted )
					{
						SetSkin( CUBE_REFLECTIVE_RUSTED_SKIN );
					}
					else
					{
						SetSkin( CUBE_REFLECTIVE_CLEAN_SKIN );
					}
				}
				break;
			}
		}
		break;
		//Sphere
		case CUBE_SPHERE:
		{
			switch( GetPaintedPower() )
			{
				//Bounce painted
				case BOUNCE_POWER:
				{
					if( m_bActivated )
					{
						SetSkin( CUBE_SPHERE_BOUNCE_ACTIVATED_SKIN );
					}
					else
					{
						SetSkin( CUBE_SPHERE_BOUNCE_SKIN );
					}
				}
				break;
				//Speed painted
				case SPEED_POWER:
				{
					if( m_bActivated )
					{
						SetSkin( CUBE_SPHERE_SPEED_ACTIVATED_SKIN );
					}
					else
					{
						SetSkin( CUBE_SPHERE_SPEED_SKIN );
					}
				}
				break;
				//Not painted
				default:
				{
					if( m_bActivated )
					{
						SetSkin( CUBE_SPHERE_CLEAN_ACTIVATED_SKIN );
					}
					else
					{
						SetSkin( CUBE_SPHERE_CLEAN_SKIN );
					}
				}
				break;
			}
		}
		break;
		//Antique cube
		case CUBE_ANTIQUE:
		{
			switch( GetPaintedPower() )
			{
				//Bounce painted
				case BOUNCE_POWER:
				{
					SetSkin( CUBE_ANTIQUE_BOUNCE_SKIN );
				}
				break;
				//Speed painted
				case SPEED_POWER:
				{
					SetSkin( CUBE_ANTIQUE_SPEED_SKIN );
				}
				break;
				//Not painted
				default:
				{
					SetSkin( CUBE_ANTIQUE_CLEAN_SKIN );
				}
				break;
			}
		}
		break;

		//Antique cube
		case CUBE_SCHRODINGER:
		{
			switch( GetPaintedPower() )
			{
				//Bounce painted
				case BOUNCE_POWER:
				{
					SetSkin( CUBE_SCHRODINGER_BOUNCE_SKIN );
				}
				break;
				//Speed painted
				case SPEED_POWER:
				{
					SetSkin( CUBE_SCHRODINGER_SPEED_SKIN );
				}
				break;
				//Not painted
				default:
				{
					SetSkin( CUBE_SCHRODINGER_CLEAN_SKIN );
				}
				break;
			}
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPropWeightedCube::SetSkin( int skinNum )
{
	m_nSkin = skinNum;
}

void CPropWeightedCube::ConvertOldSkins( void )
{
	//HACK HACK: Make the cubes choose skins using the new method even though the maps have not been updated to use them.
	if( !m_bNewSkins )
	{
		if( m_nSkin > 1 )
		{
			m_nSkin--;
		}

		m_nCubeType = static_cast<WeightedCubeType_e>( m_nSkin.Get() );
		m_bNewSkins = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPropWeightedCube::Spawn( void )
{
	Precache(); // This includes ConvertOldSkins() valve calls it twice for some reason

	m_bPickupDisabled = false;
	SetCubeType();

	SetInteraction( PROPINTER_PHYSGUN_ALLOW_OVERHEAD );

	BaseClass::Spawn();

	SetCollisionGroup( COLLISION_GROUP_NONE ); // COLLISION_GROUP_WEIGHTED_CUBE

	SetFadeDistance( -1.0f, 0.0f );
}

//-----------------------------------------------------------------------------
// Creates a weighted cube of a specific type
//-----------------------------------------------------------------------------
void CPropWeightedCube::CreatePortalWeightedCube( WeightedCubeType_e objectType, bool bAtCursorPosition, const Vector &position )
{
	MDLCACHE_CRITICAL_SECTION();

	bool allowPrecache = CBaseEntity::IsPrecacheAllowed();
	CBaseEntity::SetAllowPrecache( true );

	// Try to create entity
	CPropWeightedCube *entity = ( CPropWeightedCube* )CreateEntityByName("prop_weighted_cube");
	if (entity)
	{
		entity->SetName( MAKE_STRING("cube") );
		entity->AddSpawnFlags( SF_PHYSPROP_ENABLE_PICKUP_OUTPUT );
		entity->m_nCubeType = objectType;
		entity->m_bNewSkins = true;
		entity->Precache();

		if ( !bAtCursorPosition )
		{
			entity->SetAbsOrigin( position );
		}

		DispatchSpawn(entity);

		if ( bAtCursorPosition )
		{
			// Now attempt to drop into the world
			CBasePlayer* pPlayer = UTIL_GetCommandClient();
			trace_t tr;
			Vector forward;
			Ray_t rayPath;
			pPlayer->EyeVectors( &forward );
			
			CTraceFilterSkipTwoEntities m_filterBeams( NULL, NULL, COLLISION_GROUP_INTERACTIVE );
			m_filterBeams.SetPassEntity( UTIL_PlayerByIndex( 1 ) );

			g_bBulletPortalTrace = true; // Why is this a global???
			rayPath.Init( pPlayer->EyePosition(), pPlayer->EyePosition() + forward * MAX_TRACE_LENGTH );
			UTIL_Portal_TraceRay(rayPath, MASK_SHOT, &m_filterBeams, &tr);
			g_bBulletPortalTrace = false;

			if ( tr.fraction != 1.0 )
			{
				tr.endpos.z += 12;
				entity->Teleport( &tr.endpos, NULL, NULL );
				UTIL_DropToFloor( entity, MASK_SOLID );
			}
		}
	}
	CBaseEntity::SetAllowPrecache( allowPrecache );
}

// Console command functions
void CC_Create_PortalWeightedCube()
{
	CPropWeightedCube::CreatePortalWeightedCube( CUBE_STANDARD );
}

void CC_Create_PortalCompanionCube()
{
	CPropWeightedCube::CreatePortalWeightedCube( CUBE_COMPANION );
}

void CC_Create_PortalReflectorCube()
{
	CPropWeightedCube::CreatePortalWeightedCube( CUBE_REFLECTIVE );
}

void CC_Create_PortalWeightedSphere()
{
	CPropWeightedCube::CreatePortalWeightedCube( CUBE_SPHERE );
}

void CC_Create_PortalWeightedAntique()
{
	CPropWeightedCube::CreatePortalWeightedCube( CUBE_ANTIQUE );
}

void CC_Create_PortalWeightedSchrodinger()
{
	CPropWeightedCube::CreatePortalWeightedCube( CUBE_SCHRODINGER );
}

// Console commands for creating cubes
static ConCommand ent_create_portal_reflector_cube("ent_create_portal_reflector_cube", CC_Create_PortalReflectorCube, "Creates a laser reflector cube cube where the player is looking.", FCVAR_GAMEDLL | FCVAR_CHEAT);
static ConCommand ent_create_portal_companion_cube("ent_create_portal_companion_cube", CC_Create_PortalCompanionCube, "Creates a companion cube where the player is looking.", FCVAR_GAMEDLL | FCVAR_CHEAT);
static ConCommand ent_create_portal_weighted_cube("ent_create_portal_weighted_cube", CC_Create_PortalWeightedCube, "Creates a standard cube where the player is looking.", FCVAR_GAMEDLL | FCVAR_CHEAT);
static ConCommand ent_create_portal_weighted_sphere("ent_create_portal_weighted_sphere", CC_Create_PortalWeightedSphere, "Creates a weighted sphere where the player is looking.", FCVAR_GAMEDLL | FCVAR_CHEAT);
static ConCommand ent_create_portal_weighted_antique("ent_create_portal_weighted_antique", CC_Create_PortalWeightedAntique, "Creates an antique cube where the player is looking.", FCVAR_GAMEDLL | FCVAR_CHEAT);
