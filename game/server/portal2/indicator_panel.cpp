/**
 * @file indicator_panel.cpp
 * @brief 
 * @date 2023-10-22
 */

#include "cbase.h"
#include "entitylist.h"
#include "util.h"

// Name of our entity's model
#define	ENTITY_MODEL	"models/gibs/airboat_broken_engine.mdl"

class CPropIndicatorPanel : public CBaseAnimating
{
public:
	DECLARE_CLASS( CPropIndicatorPanel, CBaseAnimating );
	DECLARE_DATADESC();

	CPropIndicatorPanel()
	{
	}

	void Spawn( void );

	void Precache( void );

	void CreateIndicatorPanel( void );
	void SetTimerDuration( float fDuration);

	// Input function
	void InputCheck( inputdata_t &inputData );
	void InputUncheck( inputdata_t &inputData );
	void InputStart( inputdata_t &inputData );
	void InputStop( inputdata_t &inputData );
	void InputReset( inputdata_t &inputData );

	void ToggleThink( void );

	void StartTimer( void );
	void ResetTimer( void );

	void ToggleIndicatorLights( bool bLightsOn );

private:
	bool	m_bEnabled;
	bool	m_bStopped;
	bool	m_bCountingDown;
	bool	m_bIsCountdownTimer;
	bool	m_bIsChecked;

	string_t m_strIndicatorLights;
	float m_flTimerDuration;
	float m_flTimerStart;

	// CHandle<CLabIndicatorPanel> m_hIndicatorPanel;
};

LINK_ENTITY_TO_CLASS( prop_indicator_panel, CPropIndicatorPanel );

// Start of our data description for the class
BEGIN_DATADESC( CPropIndicatorPanel )
	DEFINE_FIELD( m_bEnabled, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bStopped, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bCountingDown, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bIsCountdownTimer, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bIsChecked, FIELD_BOOLEAN ),

	DEFINE_FIELD( m_flTimerDuration, FIELD_TIME ),
	DEFINE_FIELD( m_flTimerStart, FIELD_TIME ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Check", InputCheck ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Uncheck", InputUncheck ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Start", InputStart ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Stop", InputStop ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Reset", InputReset ),

	// Declare our think function
	DEFINE_THINKFUNC( ToggleThink ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );

	BaseClass::Precache();
}


//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CPropIndicatorPanel::Spawn( void )
{
	Precache();

	SetModel( ENTITY_MODEL );
	SetSolid( SOLID_VPHYSICS );
	SetMoveType(MOVETYPE_NONE);
	CreateIndicatorPanel();
}

void CPropIndicatorPanel::CreateIndicatorPanel( void )
{
	// TODO
}

void CPropIndicatorPanel::InputCheck( inputdata_t &inputData )
{
	// TODO: Implement vgui indecator.
	{

	}

	SetThink( NULL );
	ToggleIndicatorLights( true );
}

void CPropIndicatorPanel::InputUncheck( inputdata_t &inputData )
{
	// TODO: Implement vgui indecator.
	{

	}

	SetThink( NULL );
	ToggleIndicatorLights( false );
}

void CPropIndicatorPanel::InputStart( inputdata_t &inputData )
{
	StartTimer();
}
void CPropIndicatorPanel::InputReset( inputdata_t &inputData )
{
	ResetTimer();
}

// Does nothing.
void CPropIndicatorPanel::InputStop( inputdata_t &inputData )
{
	return;
}

void CPropIndicatorPanel::StartTimer( void )
{
	return;
}

void CPropIndicatorPanel::ResetTimer( void )
{
	return;
}


void CPropIndicatorPanel::ToggleThink( void )
{
	return;
}


void CPropIndicatorPanel::ToggleIndicatorLights( bool bLightsOn )
{
	CBaseEntity* pStartEntity = NULL;
	const char* indicator_lights = m_strIndicatorLights.ToCStr();
	while( true )
	{
		if(indicator_lights == NULL || *indicator_lights == '\0')
			indicator_lights = "";

		pStartEntity = gEntList.FindEntityByName( pStartEntity, indicator_lights );

		if (pStartEntity == NULL) 
			break;
	}
	return;
}

void CPropIndicatorPanel::SetTimerDuration( float fDuration )
{
	m_flTimerDuration = fDuration;
}
