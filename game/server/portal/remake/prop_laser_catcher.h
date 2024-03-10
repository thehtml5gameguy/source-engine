/**
 * @file prop_laser_catcher.h
 * @brief Adds laser catchers from Portal 2
 * @date 2023-04-05
 */

#include "cbase.h"

class CPropLaserCatcher : public CBaseAnimating //, public CCatcher
{
public:
	DECLARE_CLASS( CPropLaserCatcher, CBaseAnimating );
	DECLARE_DATADESC();

	CPropLaserCatcher();
	virtual void Precache( void );
	virtual void Spawn( void );
	void SetActivated( bool bActivate );

	virtual void SetSkin( int skinNum );

private:
	void SetCatcherSkin( void );
    void CatcherThink( void );

	COutputEvent m_OnPowered;
	COutputEvent m_OnUnpowered;

	int m_IdleSequence;
	int m_PowerOnSequence;
	int m_iTargetAttachment;

	bool m_bActivated;
	bool m_bRusted;
};
