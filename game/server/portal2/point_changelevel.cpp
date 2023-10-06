//===== Copyright Valve Corporation, All rights reserved. ======//
//
//  Purpose: 
//
//===========================================================================//

#include "cbase.h"
#include "eventqueue.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
//-----------------------------------------------------------------------------
// Context think
//-----------------------------------------------------------------------------

#define cchMapNameMost 32

static char szDestinationMap[cchMapNameMost];
static char szOriginMap[cchMapNameMost];

static ConVar sv_transition_fade_time( "sv_transition_fade_time", "0.5", FCVAR_DEVELOPMENTONLY);

class CPointChangelevel : public CPointEntity
{
public:
	DECLARE_CLASS( CPointChangelevel, CPointEntity );
	DECLARE_DATADESC();

	virtual void InputChangeLevel( inputdata_t& inputdata );
	virtual void InputChangeLevelPostFade( inputdata_t& inputdata );

private:
	COutputEvent					m_OnChangeLevel;
};

LINK_ENTITY_TO_CLASS( point_changelevel, CPointChangelevel );

BEGIN_DATADESC( CPointChangelevel )

	DEFINE_OUTPUT( m_OnChangeLevel,			"OnChangeLevel" ),
	DEFINE_INPUTFUNC ( FIELD_STRING, "ChangeLevel", InputChangeLevel ),
	DEFINE_INPUTFUNC ( FIELD_STRING, "ChangeLevelPostFade", InputChangeLevelPostFade ),
	
END_DATADESC()

void CPointChangelevel::InputChangeLevel( inputdata_t& inputdata )
{
	float fade_time = sv_transition_fade_time.GetFloat();

	g_EventQueue.AddEvent((CBaseEntity*)this, "ChangeLevelPostFade", inputdata.value, fade_time, inputdata.pActivator ,inputdata.pCaller ,inputdata.nOutputID );
}

void CPointChangelevel::InputChangeLevelPostFade( inputdata_t& inputdata )
{
	m_OnChangeLevel.FireOutput(inputdata.pActivator, this, 0.0f);
	const char* current_map = gpGlobals->mapname.ToCStr();

	if(current_map == NULL)
		current_map = "";

	V_strncpy(szOriginMap, current_map, 32);

	const char* next_map;
	next_map = inputdata.value.String();
	if(next_map == NULL)
		next_map = "";
	V_strncpy(szDestinationMap, next_map, 32);
	
	engine->ChangeLevel(szDestinationMap, NULL);
}

const char *ChangeLevel_DestinationMapName( void )
{
	return szDestinationMap;
}
const char *ChangeLevel_OriginMapName( void )
{
	return szDestinationMap;
}
const char *ChangeLevel_GetLandmarkName( void )
{
	return "__p2_landmark";
}

class CInfoLandMark : public CPointEntity
{
public:
	DECLARE_CLASS( CInfoLandMark, CPointEntity );
};

LINK_ENTITY_TO_CLASS( info_landmark_entry, CInfoLandMark );
LINK_ENTITY_TO_CLASS( info_landmark_exit, CInfoLandMark );
