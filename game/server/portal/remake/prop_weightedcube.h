/**
 * @file prop_weightedcube.h
 * @brief First-class cube entity so we can query by type and generally make inferences that are harder to do without an entity of that type.
 * @date 2023-04-05
 * 
 * @copyright Copyright � 1996-2009, Valve Corporation, All rights reserved.
 * @attention Partially stolen from the CS:GO leak!
 */

#include "cbase.h"
#include "props.h"
#include "portal2/paint_enum.h"

enum WeightedCubeType_e
{
	CUBE_STANDARD,
	CUBE_COMPANION,
	CUBE_REFLECTIVE,
	CUBE_SPHERE,
	CUBE_ANTIQUE,
	CUBE_SCHRODINGER,
};


//Standard cube skins
enum StandardCubeSkinType_t
{
	CUBE_STANDARD_CLEAN_SKIN = 0,
	CUBE_STANDARD_CLEAN_ACTIVATED_SKIN = 2,
	CUBE_STANDARD_RUSTED_SKIN = 3,
	CUBE_STANDARD_RUSTED_ACTIVATED_SKIN = 5,
	CUBE_STANDARD_BOUNCE_SKIN = 6,
	CUBE_STANDARD_BOUNCE_ACTIVATED_SKIN = 10,
	CUBE_STANDARD_SPEED_SKIN = 7,
	CUBE_STANDARD_SPEED_ACTIVATED_SKIN = 11
};

//Companion cube skins
enum CompanionCubeSkinType_t
{
	CUBE_COMPANION_CLEAN_SKIN = 1,
	CUBE_COMPANION_CLEAN_ACTIVATED_SKIN = 4,
	CUBE_COMPANION_BOUNCE_SKIN = 8,
	CUBE_COMPANION_BOUNCE_ACTIVATED_SKIN = 8,
	CUBE_COMPANION_SPEED_SKIN = 9,
	CUBE_COMPANION_SPEED_ACTIVATED_SKIN = 9
};

//Reflective cubs skins
enum ReflectiveCubeSkinType_t
{
	CUBE_REFLECTIVE_CLEAN_SKIN = 0,
	CUBE_REFLECTIVE_RUSTED_SKIN = 1,
	CUBE_REFLECTIVE_BOUNCE_SKIN = 2,
	CUBE_REFLECTIVE_SPEED_SKIN = 3
};

//Sphere skins
enum WeightedSpherSkinType_t
{
	CUBE_SPHERE_CLEAN_SKIN = 0,
	CUBE_SPHERE_CLEAN_ACTIVATED_SKIN = 1,
	CUBE_SPHERE_BOUNCE_SKIN = 2,
	CUBE_SPHERE_BOUNCE_ACTIVATED_SKIN = 2,
	CUBE_SPHERE_SPEED_SKIN = 3,
	CUBE_SPHERE_SPEED_ACTIVATED_SKIN = 3
};

//Antique cube skins
enum AntiqueCubeSkinType_t
{
	CUBE_ANTIQUE_CLEAN_SKIN = 0,
	CUBE_ANTIQUE_BOUNCE_SKIN = 1,
	CUBE_ANTIQUE_SPEED_SKIN = 2
};

//Schrodinger cube skins
enum SchrodingerCubeSkinType_t
{
	CUBE_SCHRODINGER_CLEAN_SKIN = 4,
	CUBE_SCHRODINGER_BOUNCE_SKIN = 5,
	CUBE_SCHRODINGER_SPEED_SKIN = 6
};

class CPropWeightedCube : public CPhysicsProp
{
public:
	DECLARE_CLASS( CPropWeightedCube, CPhysicsProp );
	DECLARE_DATADESC();

	CPropWeightedCube();

	PaintPowerType GetPaintedPower(); // FIXME: This is not from this class

	virtual void Precache( void );
	virtual void Spawn( void );

	virtual void SetSkin( int skinNum );

	bool WasTouchedByPlayer() { return m_bTouchedByPlayer; }

	virtual void StartTouch( CBaseEntity *pOther );
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void SetActivated( bool bActivate );
	// instead of getting which model it uses, we can just ask this
	WeightedCubeType_e GetCubeType( void ) { return m_nCubeType; }

	void SetLaser( CBaseEntity *pLaser );
	CBaseEntity* GetLaser()
	{
		return m_hLaser.Get();
	}
	bool HasLaser( void )
	{
		return m_hLaser.Get() != NULL;
	}

	static void CreatePortalWeightedCube( WeightedCubeType_e objectType, bool bAtCursorPosition = true, const Vector &position = vec3_origin );

private:

	void SetCubeType( void );
	void SetCubeSkin( void );
	void ConvertOldSkins( void );

	void InputDissolve( inputdata_t &in );
	void InputDisablePickup( inputdata_t &in );
	void InputEnablePickup( inputdata_t &in );

	WeightedCubeType_e m_nCubeType;
	bool m_bActivated;
	bool m_bPickupDisabled;
	bool m_bTouchedByPlayer;

	bool m_bRusted;
	bool m_bNewSkins;

	EHANDLE m_hLaser;
};

bool UTIL_IsReflectiveCube( CBaseEntity *pEntity );
bool UTIL_IsWeightedCube( CBaseEntity *pEntity );
