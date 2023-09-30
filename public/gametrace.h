//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef GAMETRACE_H
#define GAMETRACE_H
#ifdef _WIN32
#pragma once
#endif


#include "cmodel.h"
#include "utlvector.h"
#include "ihandleentity.h"
#include "ispatialpartition.h"

#if defined( CLIENT_DLL )
	class C_BaseEntity;
#else
	class CBaseEntity;
#endif


//-----------------------------------------------------------------------------
// Purpose: A trace is returned when a box is swept through the world
// NOTE: eventually more of this class should be moved up into the base class!!
//-----------------------------------------------------------------------------
class CGameTrace : public CBaseTrace
{
public:

	// Returns true if hEnt points at the world entity.
	// If this returns true, then you can't use GetHitBoxIndex().
	bool DidHitWorld() const;
	
	// Returns true if we hit something and it wasn't the world.
	bool DidHitNonWorldEntity() const;

	// Gets the entity's network index if the trace has hit an entity.
	// If not, returns -1.
	int GetEntityIndex() const;

	// Returns true if there was any kind of impact at all
	bool DidHit() const;

	// The engine doesn't know what a CBaseEntity is, so it has a backdoor to 
	// let it get at the edict.
#if defined( ENGINE_DLL )
	void SetEdict( edict_t *pEdict );
	edict_t* GetEdict() const;
#endif	


public:

	float		fractionleftsolid;		// time we left a solid, only valid if we started in solid
	csurface_t	surface;				// surface hit (impact surface)

	int			hitgroup;				// 0 == generic, non-zero is specific body part
	short		physicsbone;			// physics bone hit by trace in studio

#if defined( CLIENT_DLL )
		C_BaseEntity *m_pEnt;
#else
		CBaseEntity *m_pEnt;
#endif

	// NOTE: this member is overloaded.
	// If hEnt points at the world entity, then this is the static prop index.
	// Otherwise, this is the hitbox index.
	int			hitbox;					// box hit by trace in studio

	CGameTrace() = default;

private:
	// No copy constructors allowed
	CGameTrace(const CGameTrace& vOther);
};


//-----------------------------------------------------------------------------
// Returns true if there was any kind of impact at all
//-----------------------------------------------------------------------------
inline bool CGameTrace::DidHit() const 
{ 
	return fraction < 1 || allsolid || startsolid; 
}


typedef CGameTrace trace_t;

//=============================================================================

class ITraceListData
{
public:
	virtual ~ITraceListData() {}

	virtual void Reset() = 0;
	virtual bool IsEmpty() = 0;
	// CanTraceRay will return true if the current volume encloses the ray
	// NOTE: The leaflist trace will NOT check this.  Traces are intersected
	// against the culled volume exclusively.
	virtual bool CanTraceRay( const Ray_t &ray ) = 0;
};

#define TLD_DEF_BRUSH_MAX	64
#define TLD_DEF_DISP_MAX	32
#define TLD_DEF_LEAF_MAX	256
#define TLD_DEF_ENTITY_MAX	1024

class ICollideable;
struct collideable_handleentity_t
{
	IHandleEntity *pEntity;
	ICollideable *pCollideable;
};


class CTraceListData : public IPartitionEnumerator, public ITraceListData
{
public:

	CTraceListData() 
	{
		m_pEngineTrace = NULL;
		m_bFoundNonSolidLeaf = false;
		m_mins.Init();
		m_maxs.Init();
	}
	~CTraceListData() {}

	void Reset()
	{
		m_brushList.RemoveAll();
		m_dispList.RemoveAll();
		m_entityList.RemoveAll();
		m_staticPropList.RemoveAll();
		m_mins.Init();
		m_maxs.Init();
		m_pEngineTrace = NULL;
		m_bFoundNonSolidLeaf = false;
	}

	bool IsEmpty() { return m_pEngineTrace == NULL ? true : false; }
	// For entities...
	IterationRetval_t EnumElement( IHandleEntity *pHandleEntity );
	bool CanTraceRay( const Ray_t &ray );

public:

	CUtlVectorFixedGrowable<unsigned short, TLD_DEF_BRUSH_MAX>	m_brushList;
	CUtlVectorFixedGrowable<unsigned short, TLD_DEF_DISP_MAX>	m_dispList;
	CUtlVectorFixedGrowable<collideable_handleentity_t, TLD_DEF_ENTITY_MAX>	m_entityList;
	CUtlVectorFixedGrowable<collideable_handleentity_t, TLD_DEF_ENTITY_MAX>	m_staticPropList;

	Vector	m_mins;
	Vector	m_maxs;
	class CEngineTrace *m_pEngineTrace;
	bool	m_bFoundNonSolidLeaf;
};

#endif // GAMETRACE_H

