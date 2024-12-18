/////////////////////////////////////////////////////////////////////////////
// JKStage.cpp: implementation of the CJKStage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JKStage.h"
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CJKStage::sm_nTotalNode;
// Heuristic flags
int CJKStage::sm_nCutOffDepth;
int CJKStage::sm_nFeedOverDepth;
bool CJKStage::sm_bPoBeforeJang;
bool CJKStage::sm_bGatherJol;
bool CJKStage::sm_bAggressivePO;
bool CJKStage::sm_bAggressiveCHA;
bool CJKStage::sm_bAggressiveJOL;
bool CJKStage::sm_bAggressiveSANG;
bool CJKStage::sm_bAggressiveMA;
bool CJKStage::sm_bOnlyJangCenter;
CHistoryStack CJKStage::sm_cHistoryStack;

CJKStage::CJKStage()
{
	Initialize();
}

CJKStage::CJKStage(CJKStage * pParentStage)
{
	m_pParent = pParentStage;
	m_nNodeCount = 0;
	m_pFirst = NULL;
	m_pLast = NULL;
	m_nDepth = pParentStage->m_nDepth + 1;
	m_enTeam = pParentStage->m_enTeam == TM_HAN ? TM_CHO : TM_HAN;
	m_bAmIMax = !pParentStage->IsMaxTurn();
	::memcpy(m_enPanel,pParentStage->m_enPanel,sizeof(m_enPanel));
}

CJKStage::~CJKStage()
{
	ClearList();
}

void CJKStage::Initialize()
{
	m_pParent = NULL;
	m_nNodeCount = 0;
	m_pFirst = NULL;

	ClearList();

	::memset(m_enPanel,PS_NOTHING,sizeof(m_enPanel));
	m_enPanel[0][0] = PS_HAN_CHA;
	m_enPanel[0][1] = PS_HAN_SANG;
	m_enPanel[0][2] = PS_HAN_MA;
	m_enPanel[0][3] = PS_HAN_SA;
	m_enPanel[0][5] = PS_HAN_SA;
	m_enPanel[0][6] = PS_HAN_SANG;
	m_enPanel[0][7] = PS_HAN_MA;
	m_enPanel[0][8] = PS_HAN_CHA;
	m_enPanel[1][4] = PS_HAN_JANG;
	m_enPanel[2][1] = PS_HAN_PO;
	m_enPanel[2][7] = PS_HAN_PO;
	m_enPanel[3][0] = PS_HAN_JOL;
	m_enPanel[3][2] = PS_HAN_JOL;
	m_enPanel[3][4] = PS_HAN_JOL;
	m_enPanel[3][6] = PS_HAN_JOL;
	m_enPanel[3][8] = PS_HAN_JOL;

	m_enPanel[9][0] = PS_CHO_CHA;
	m_enPanel[9][1] = PS_CHO_SANG;
	m_enPanel[9][2] = PS_CHO_MA;
	m_enPanel[9][3] = PS_CHO_SA;
	m_enPanel[9][5] = PS_CHO_SA;
	m_enPanel[9][6] = PS_CHO_SANG;
	m_enPanel[9][7] = PS_CHO_MA;
	m_enPanel[9][8] = PS_CHO_CHA;
	m_enPanel[8][4] = PS_CHO_JANG;
	m_enPanel[7][1] = PS_CHO_PO;
	m_enPanel[7][7] = PS_CHO_PO;
	m_enPanel[6][0] = PS_CHO_JOL;
	m_enPanel[6][2] = PS_CHO_JOL;
	m_enPanel[6][4] = PS_CHO_JOL;
	m_enPanel[6][6] = PS_CHO_JOL;
	m_enPanel[6][8] = PS_CHO_JOL; 

	sm_bPoBeforeJang	= true;
	sm_bGatherJol		= true;
	sm_bAggressivePO	= true;
	sm_bAggressiveCHA	= true;
	sm_bAggressiveJOL	= true;
	sm_bAggressiveSANG	= true;
	sm_bAggressiveMA	= true;
	sm_bOnlyJangCenter	= true;
}

/////////////////////////////////////////////////
// Check whether piece can pass the place located
bool CJKStage::IsPassable(int nX,int nY)
{
	if ( nX < 0 ) return false;
	if ( nY < 0 ) return false;
	if ( nX >= DF_PANEL_WIDTH ) return false;
	if ( nY >= DF_PANEL_HEIGHT ) return false;

	if ( m_enPanel[nY][nX] == PS_NOTHING ) 
		return true;
	else
		return false;
}

void CJKStage::ClearList()
{
	if ( m_pFirst )
	{
		CJKStage *pNode = m_pFirst;
		CJKStage *pTmp;
		do
		{
			pTmp = pNode->m_pNext;
			delete pNode;
			pNode = pTmp;
		} while (pNode != NULL);
		m_pFirst = NULL;
		m_pLast = NULL;
		m_nNodeCount	= 0;
	}
}

void CJKStage::AddNextStageEffectively(CJKStage *pNewStage)
{
	int nNewScore	= pNewStage->Evaluate();

	///////////////////////////////////////////////////////
	// Nodes which are placed in same level must be sorted
	// to make Alpha-Beta algorithm effeciently.
	// When it's in Max turn, it must be sorted ascendently
	// in case of Min,Decendently.
	if ( m_bAmIMax )
	{
		if ( m_pFirst )
		{
			CJKStage *pNode = m_pFirst;
			do
			{
				if ( nNewScore >= pNode->m_nScore ) break;
				pNode = pNode->m_pNext;
			} while( pNode != NULL );
			if ( pNode == NULL )
			{
				m_pLast->m_pNext = pNewStage;
				pNewStage->m_pPrev = m_pLast;
				pNewStage->m_pNext = NULL;
				m_pLast = pNewStage;
			} else
			{
				CJKStage * pTmp = pNode->m_pPrev;
				pNode->m_pPrev = pNewStage;
				pNewStage->m_pNext = pNode;
				pNewStage->m_pPrev = pTmp;
				if ( pTmp )
				{
					pTmp->m_pNext = pNewStage;
				} else
				{
					m_pFirst = pNewStage;
				}
			}
		} else
		{
			m_pFirst = pNewStage;
			m_pLast = pNewStage;
			pNewStage->m_pPrev = NULL;
			pNewStage->m_pNext  = NULL;
		}
	} else
	{
		if ( m_pFirst )
		{
			CJKStage *pNode = m_pFirst;
			do
			{
				if ( nNewScore <= pNode->m_nScore ) break;
				pNode = pNode->m_pNext;
			} while(pNode != NULL);
			if ( pNode == NULL )
			{
				m_pLast->m_pNext = pNewStage;
				pNewStage->m_pPrev = m_pLast;
				pNewStage->m_pNext = NULL;
				m_pLast = pNewStage;

			} else
			{
				CJKStage * pTmp = pNode->m_pPrev;
				pNode->m_pPrev = pNewStage;
				pNewStage->m_pNext = pNode;
				pNewStage->m_pPrev = pTmp;
				if ( pTmp )
				{
					pTmp->m_pNext = pNewStage;
				} else
				{
					m_pFirst = pNewStage;
				}
			}
		} else
		{
			m_pFirst = pNewStage;
			m_pLast = pNewStage;
			pNewStage->m_pPrev = NULL;
			pNewStage->m_pNext  = NULL;
		}
	}
	++ m_nNodeCount;
}

//////////////////////////////////////////////
// Add possible movable location to the list 
bool CJKStage::TryToAddMovableXY(int nFromX,int nFromY,int nToX,int nToY)
{
	if ( nToX < 0 ) return false;
	if ( nToY < 0 ) return false;
	if ( nToX >= DF_PANEL_WIDTH ) return false;
	if ( nToY >= DF_PANEL_HEIGHT ) return false;
	
	// Impossible to move to a location where other piece of 
	// same team exist.
	if ( GetTeamAt(nToX,nToY) == GetTeamAt(nFromX,nFromY) ) return false;
	
	CJKStage *pNewStage = new CJKStage(this);
	pNewStage->MovePiece(nFromX,nFromY,nToX,nToY);

	pNewStage->m_nFromX			= nFromX;
	pNewStage->m_nFromY			= nFromY;
	pNewStage->m_nToX			= nToX;
	pNewStage->m_nToY			= nToY;
	pNewStage->m_enEatenPiece	= m_enPanel[nToY][nToX];

	++ sm_nTotalNode;
	
	AddNextStageEffectively(pNewStage);
	
	return true;
}

TEAM CJKStage::GetTeamAt(int nX,int nY)
{
	if ( m_enPanel[nY][nX] == PS_NOTHING ) return TM_NOTHING;
	if ( m_enPanel[nY][nX] < PS_BOUNDARY ) return TM_CHO;

	return TM_HAN;
}

void CJKStage::GetMovableSaAndJang(int nFromX,int nFromY)
{
	switch ( nFromY )
	{
	case 0	:
	case 7	:
		switch ( nFromX )
		{
		case 3	:
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY + 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY + 1);
			break;
		case 4	:
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY + 1);
			break;
		case 5	:
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY + 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY + 1);
			break;
		}
		break;
	case 1	:
	case 8	:
		switch ( nFromX )
		{
		case 3	:
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY + 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY);
			break;
		case 4	:
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY + 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY + 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY + 1);
			break;
		case 5	:
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY + 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY);
			break;
		}
		break;
	case 2	:
	case 9	:
		switch ( nFromX )
		{
		case 3	:
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY);
			break;
		case 4	:
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY - 1);
			break;
		case 5	:
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY);
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY - 1);
			break;
		}
		break;
	}
}

void CJKStage::GetMovableJol(int nFromX,int nFromY)
{
	int nDirection = GetTeamAt(nFromX,nFromY) == TM_CHO ? -1 : 1;
	
	//////////////////////////////////////////////////
	// JOL also can move diagonaly in the area of King
	switch ( nFromY )
	{
	case 1	:
	case 8	:
		switch ( nFromX )
		{
		case 4	:
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY + nDirection);
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY + nDirection);
			break;
		}
		break;
	case 2	:
	case 7	:
		switch ( nFromX )
		{
		case 3	:
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY + nDirection);
			break;
		case 5	:
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY + nDirection);
			break;
		}
		break;
	}

	// Tru to move to left
	TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY);
	// Try to move to right	
	TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY);
	// Try to move forward
	TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY + nDirection);	
}


void CJKStage::GetMovablePo(int nFromX,int nFromY)
{
	bool bMovable = false;
	// Try Right side line
	for ( int k = nFromX + 1; k < DF_PANEL_WIDTH; ++ k )
	{
		if ( bMovable )
		{
			if ( m_enPanel[nFromY][k] != PS_CHO_PO &&
				 m_enPanel[nFromY][k] != PS_HAN_PO)
			{
				TryToAddMovableXY(nFromX,nFromY,k,nFromY);
				if ( m_enPanel[nFromY][k] != PS_NOTHING )
					break;
			} else 
			{
				break;
			}
		} else
		{
			////////////////////////////////////////////////////////
			// PO only can move over the other piece except other PO
			if ( m_enPanel[nFromY][k] == PS_HAN_PO )
				break;
			if ( m_enPanel[nFromY][k] == PS_CHO_PO )
				break;
			if ( m_enPanel[nFromY][k] != PS_NOTHING )
				bMovable = true;
		}
	}
	
	bMovable = false;
	// Try left side line
	for ( k = nFromX - 1; k >= 0; -- k )
	{
		if ( bMovable )
		{
			if ( m_enPanel[nFromY][k] != PS_CHO_PO &&
				 m_enPanel[nFromY][k] != PS_HAN_PO )
			{
				TryToAddMovableXY(nFromX,nFromY,k,nFromY);
				if ( m_enPanel[nFromY][k] != PS_NOTHING )
					break;
			} else 
			{
				break;
			}
		} else
		{
			////////////////////////////////////////////////////////
			// PO only can move over the other piece except other PO
			if ( m_enPanel[nFromY][k] == PS_HAN_PO )
				break;
			if ( m_enPanel[nFromY][k] == PS_CHO_PO )
				break;
			if ( m_enPanel[nFromY][k] != PS_NOTHING )
				bMovable = true;
		}
	}
	bMovable = false;
	// Try upper line
	for ( k = nFromY - 1; k >= 0; -- k )
	{
		if ( bMovable )
		{
			if ( m_enPanel[k][nFromX] != PS_CHO_PO &&
				 m_enPanel[k][nFromX] != PS_HAN_PO )
			{
				TryToAddMovableXY(nFromX,nFromY,nFromX,k);
				if ( m_enPanel[k][nFromX] != PS_NOTHING )
					break;
			} else 
			{
				break;
			}
		} else
		{
			////////////////////////////////////////////////////////
			// PO only can move over the other piece except other PO
			if ( m_enPanel[k][nFromX] == PS_HAN_PO )
				break;
			if ( m_enPanel[k][nFromX] == PS_CHO_PO )
				break;
			if ( m_enPanel[k][nFromX] != PS_NOTHING )
				bMovable = true;
		}
	}
	bMovable = false;
	// Try under line
	for ( k = nFromY + 1; k < DF_PANEL_HEIGHT; ++ k )
	{
		if ( bMovable )
		{
			if ( m_enPanel[k][nFromX] != PS_CHO_PO &&
				 m_enPanel[k][nFromX] != PS_HAN_PO )
			{
				TryToAddMovableXY(nFromX,nFromY,nFromX,k);
				if ( m_enPanel[k][nFromX] != PS_NOTHING )
					break;
			} else 
			{
				break;
			}
		} else
		{
			////////////////////////////////////////////////////////
			// PO only can move over the other piece except other PO
			if ( m_enPanel[k][nFromX] == PS_HAN_PO )
				break;
			if ( m_enPanel[k][nFromX] == PS_CHO_PO )
				break;
			if ( m_enPanel[k][nFromX] != PS_NOTHING )
				bMovable = true;
		}
	}
}


void CJKStage::GetMovableCha(int nFromX,int nFromY)
{
	//////////////////////////////////////////////////
	// CHA also can move diagonaly in the area of King
	switch ( nFromY )
	{
	case 0	:
	case 7	:
		switch ( nFromX )
		{
		case 3	:
			if ( GetTeamAt(4,nFromY) != GetTeamAt(nFromX,nFromY) )
				TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY + 1);
			if ( GetTeamAt(4,nFromY + 1) == PS_NOTHING )
				TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY + 2);
			break;
		case 5	:
			if ( GetTeamAt(4,nFromY) != GetTeamAt(nFromX,nFromY) )
				TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY + 1);
			if ( GetTeamAt(4,nFromY + 1) == PS_NOTHING )
				TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY + 2);
			break;
		}
		break;
	case 1	:
	case 8	:
		switch ( nFromX )
		{
		case 4	:
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY - 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY + 1);
			TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY + 1);
			break;
		}
		break;
	case 2	:
	case 9	:
		switch ( nFromX )
		{
		case 3	:
			if ( GetTeamAt(4,nFromY) != GetTeamAt(nFromX,nFromY) )
				TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY - 1);
			if ( GetTeamAt(4,nFromY - 1) == PS_NOTHING )
				TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY - 2);
			break;
		case 5	:
			if ( GetTeamAt(4,nFromY) != GetTeamAt(nFromX,nFromY) )
				TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY - 1);
			if ( GetTeamAt(4,nFromY - 1) == PS_NOTHING )
				TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY - 2);
			break;
		}
		break;
	}
	// try Right line
	for ( int k = nFromX + 1; k < DF_PANEL_WIDTH; ++ k )
	{
		// Stop if other piece of same team exist
		if ( GetTeamAt(k,nFromY) == GetTeamAt(nFromX,nFromY) )
			break;
		TryToAddMovableXY(nFromX,nFromY,k,nFromY);
		if ( GetTeamAt(k,nFromY) != PS_NOTHING )
			break;
	}
	// try Left line 
	for ( k = nFromX - 1; k >= 0; -- k )
	{
		if ( GetTeamAt(k,nFromY) == GetTeamAt(nFromX,nFromY) )
			break;
		TryToAddMovableXY(nFromX,nFromY,k,nFromY);
		if ( GetTeamAt(k,nFromY) != PS_NOTHING )
			break;
	}

	// try Upper line
	for ( k = nFromY - 1; k >= 0; -- k )
	{
		if ( GetTeamAt(nFromX,k) == GetTeamAt(nFromX,nFromY) )
			break;
		TryToAddMovableXY(nFromX,nFromY,nFromX,k);
		if ( GetTeamAt(nFromX,k) != PS_NOTHING )
			break;
	}
	
	// Try under line 
	for ( k = nFromY + 1; k < DF_PANEL_HEIGHT; ++ k )
	{
		if ( GetTeamAt(nFromX,k) == GetTeamAt(nFromX,nFromY) )
			break;
		TryToAddMovableXY(nFromX,nFromY,nFromX,k);
		if ( GetTeamAt(nFromX,k) != PS_NOTHING )
			break;
	}
}

void CJKStage::GetMovableMa(int nFromX,int nFromY)
{
	// Left Upper 
	if ( IsPassable(nFromX,nFromY - 1) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY - 2);
	// Right Upper
	if ( IsPassable(nFromX,nFromY - 1) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY - 2);
	// Left under
	if ( IsPassable(nFromX,nFromY + 1) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY + 2);
	// Right under
	if ( IsPassable(nFromX,nFromY + 1) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY + 2);
	// Left Upper 
	if ( IsPassable(nFromX - 1,nFromY) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY - 1);
	// Left under
	if ( IsPassable(nFromX - 1,nFromY) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY + 1);
	// Right upper
	if ( IsPassable(nFromX + 1,nFromY) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY - 1);
	// right under
	if ( IsPassable(nFromX + 1,nFromY) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY + 1);
}

void CJKStage::GetMovableSang(int nFromX,int nFromY)
{
	// Upper & Left
	if ( IsPassable(nFromX,nFromY - 1) && IsPassable(nFromX - 1,nFromY - 2) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY - 3);

	// Upper & Right
	if ( IsPassable(nFromX,nFromY - 1) && IsPassable(nFromX + 1,nFromY - 2) )
		TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY - 3);
	
	// Under & Left
	if ( IsPassable(nFromX,nFromY + 1) && IsPassable(nFromX - 1,nFromY + 2) )
		TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY + 3);
	
	// Under & Right
	if ( IsPassable(nFromX,nFromY + 1) && IsPassable(nFromX + 1,nFromY + 2) )
		TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY + 3);
	
	// Left & Upper
	if ( IsPassable(nFromX - 1,nFromY) && IsPassable(nFromX - 2,nFromY - 1) )
		TryToAddMovableXY(nFromX,nFromY,nFromX - 3,nFromY - 2);
	
	// Left & Under
	if ( IsPassable(nFromX - 1,nFromY) && IsPassable(nFromX - 2,nFromY + 1) )
		TryToAddMovableXY(nFromX,nFromY,nFromX - 3,nFromY + 2);
	
	// Right & Upper
	if ( IsPassable(nFromX + 1,nFromY) && IsPassable(nFromX + 2,nFromY - 1) )
		TryToAddMovableXY(nFromX,nFromY,nFromX + 3,nFromY - 2);
	
	// Right & Under
	if ( IsPassable(nFromX + 1,nFromY) && IsPassable(nFromX + 2,nFromY + 1) )
		TryToAddMovableXY(nFromX,nFromY,nFromX + 3,nFromY + 2);
}
/////////////////////////////////////////////////
// HAN : Plus score
// CHO : Minus score
int CJKStage::Evaluate()
{
	m_nScore = 0;
	for ( int i,j = 0; j < DF_PANEL_HEIGHT; ++ j )
		for ( i = 0; i < DF_PANEL_WIDTH; ++ i )
		{
			switch ( m_enPanel[j][i] )
			{
			case PS_CHO_JANG		: 
				{
					m_nScore -= DF_SCORE_JANG;
					// An inclination to place PO in front of JANG
					if ( sm_bPoBeforeJang )
					{
						if ( m_enPanel[j - 1][i] == PS_CHO_PO ) 
							m_nScore -= 50;
					}
					break;
				}
			case PS_CHO_SA		: m_nScore -= DF_SCORE_SA;break;
			case PS_CHO_JOL		: 
				{
					m_nScore -= DF_SCORE_JOL;
					// Gather JOL to center
					if ( sm_bGatherJol )
					{
						if ( m_enPanel[j][i - 1] == PS_CHO_JOL )
							m_nScore -= 1;
						if ( m_enPanel[j][i + 1] == PS_CHO_JOL )
							m_nScore -= 1;
					}
					// Make JOL aggressive
					if ( sm_bAggressiveJOL )
					{
						if ( j < 7 )
							m_nScore -= 1;
					}
					break;
				}
			case PS_CHO_PO		: 
				{
					m_nScore -= DF_SCORE_PO;
					// Make PO aggressive
					if ( sm_bAggressivePO )
					{
						if ( j < 7 )
							m_nScore -= 1;
					}
					break;
				}
			case PS_CHO_CHA		: 
				{
					m_nScore -= DF_SCORE_CHA;
					// Make CHA Aggressive
					if ( sm_bAggressiveCHA )
					{
						if ( j < 7 )
							m_nScore -= 2;
					}
					break;
				}
			case PS_CHO_MA		: 
				{
					m_nScore -= DF_SCORE_MA;
					// Make MA aggressive
					if ( sm_bAggressiveMA )
					{
						if ( j < 7 )
							m_nScore -= 1;
					}
					break;
				}
			case PS_CHO_SANG	: 
				{
					m_nScore -= DF_SCORE_SANG;
					// Make SANG aggressive
					if ( sm_bAggressiveSANG )
					{
						if ( j < 7 )
							m_nScore -= 1;
					}
					break;
				}
			case PS_HAN_JANG	: 
				{
					m_nScore += DF_SCORE_JANG;
					// An inclination to place PO in front of JANG
					if ( sm_bPoBeforeJang )
					{			
						if ( m_enPanel[j + 1][i] == PS_HAN_PO )
							m_nScore += 50;
					}
					break;
				}
			case PS_HAN_SA		: m_nScore += DF_SCORE_SA;break;
			case PS_HAN_JOL	: 
				{
					m_nScore += DF_SCORE_JOL;
					// Gather JOL to center
					if ( sm_bGatherJol )
					{
						if ( m_enPanel[j][i - 1] == PS_HAN_JOL )
							m_nScore += 1;
						if ( m_enPanel[j][i + 1] == PS_HAN_JOL )
							m_nScore += 1;
					}
					// Aggressive JOL
					if ( sm_bAggressiveJOL )
					{
						if ( j > 3 )
							m_nScore += 1;
					}
					break;
				}
			case PS_HAN_PO		:
				{
					m_nScore += DF_SCORE_PO;
					// Aggressive PO
					if ( sm_bAggressivePO )
					{				
						if ( j > 3 )
							m_nScore += 1;
					}
					break;
				}
			case PS_HAN_CHA	: 
				{
					m_nScore += DF_SCORE_CHA;
					// Aggressive CHA
					if ( sm_bAggressiveCHA )
					{
						if ( j > 3 )
							m_nScore += 2;
					}
					break;
				}
			case PS_HAN_MA		: 
				{
					m_nScore += DF_SCORE_MA;
					// Aggressive MA
					if ( sm_bAggressiveMA )
					{
						if ( j > 3 )
							m_nScore += 1;
					}
					break;
				}
			case PS_HAN_SANG	: 
				{
					m_nScore += DF_SCORE_SANG;
					// Aggressive SANG
					if ( sm_bAggressiveSANG )
					{
						if ( j > 3 )
							m_nScore += 1;
					}
					break;
				}
			}
		}
	// Block up other pieces except SA and MA to come to the center of the area of JANG
	if ( sm_bOnlyJangCenter )
	{
		if ( m_enPanel[1][4] != PS_NOTHING && m_enPanel[1][4] != PS_HAN_JANG && m_enPanel[1][5] != PS_HAN_SA )
			m_nScore -= 10;
		if ( m_enPanel[8][4] != PS_NOTHING && m_enPanel[8][4] != PS_CHO_JANG && m_enPanel[8][4] != PS_CHO_SA )
			m_nScore += 10;
	}
	return m_nScore;
}

bool CJKStage::SaveStage(const char * filename)
{
	FILE *fp = fopen(filename,"w+b");
	if ( fp == NULL ) return false;
	
	fwrite(m_enPanel,1,sizeof(m_enPanel),fp);
	sm_cHistoryStack.SaveHistory(fp);

	fclose(fp);

	return true;
}

bool CJKStage::LoadStage(const char * filename)
{
	FILE *fp = fopen(filename,"r+b");
	if ( fp == NULL ) return false;

	fread(m_enPanel,1,sizeof(m_enPanel),fp);
	sm_cHistoryStack.LoadHistory(fp);
	fclose(fp);

	return true;
}

bool CJKStage::MovePiece(int nFromX,int nFromY,int nToX,int nToY)
{
	if ( nFromX < 0 || nFromX >= DF_PANEL_WIDTH ) return false;
	if ( nFromY < 0 || nFromY >= DF_PANEL_HEIGHT ) return false;
	if ( nToX < 0 || nToX >= DF_PANEL_WIDTH ) return false;
	if ( nToY < 0 || nToY >= DF_PANEL_HEIGHT ) return false;

	m_enPanel[nToY][nToX] = m_enPanel[nFromY][nFromX];
	m_enPanel[nFromY][nFromX] = PS_NOTHING;
	
	return true;
}

MOVERESULT CJKStage::HumanMove(int nFromX,int nFromY,int nToX,int nToY)
{
	PIECES enEatenPiece = m_enPanel[nToY][nToX];

	bool bResult = MovePiece(nFromX,nFromY,nToX,nToY);

	if ( !bResult )	return MR_UNMOVED;
	// Save History
	sm_cHistoryStack.Push(nFromX,nFromY,nToX,nToY,enEatenPiece);

	if ( enEatenPiece == PS_HAN_JANG ) return MR_HANDEAD;
	if ( enEatenPiece == PS_CHO_JANG ) return MR_CHODEAD;
	
	return MR_MOVED;
}

///////////////////////////////////////////////
// Makes another CJKStage object,and get
// the information of movable location.
bool CJKStage::GetNextPieceLoc(int nFromX,int nFromY,CMoveData & cMD)
{
	CJKStage cStage;
	::memcpy(cStage.m_enPanel,m_enPanel,sizeof(m_enPanel));

	cMD.Reset();
	/////////////////////////////////////////////////////
	// 각 말 별로 움직일 수 있는 범위를 구한다.
	switch ( cStage.m_enPanel[nFromY][nFromX] )
	{
	case PS_CHO_JANG	: 
	case PS_CHO_SA		: cStage.GetMovableSaAndJang(nFromX,nFromY);break;
	case PS_CHO_JOL		: cStage.GetMovableJol(nFromX,nFromY);break;
	case PS_CHO_PO		: cStage.GetMovablePo(nFromX,nFromY);break;
	case PS_CHO_CHA		: cStage.GetMovableCha(nFromX,nFromY);break;
	case PS_CHO_MA		: cStage.GetMovableMa(nFromX,nFromY);break;
	case PS_CHO_SANG	: cStage.GetMovableSang(nFromX,nFromY);break;
	case PS_HAN_JANG	: 
	case PS_HAN_SA		: cStage.GetMovableSaAndJang(nFromX,nFromY);break;
	case PS_HAN_JOL		: cStage.GetMovableJol(nFromX,nFromY);break;
	case PS_HAN_PO		: cStage.GetMovablePo(nFromX,nFromY);break;
	case PS_HAN_CHA		: cStage.GetMovableCha(nFromX,nFromY);break;
	case PS_HAN_MA		: cStage.GetMovableMa(nFromX,nFromY);break;
	case PS_HAN_SANG	: cStage.GetMovableSang(nFromX,nFromY);break;
	}

	cMD.SetFromXY(nFromX,nFromY);

	if ( cStage.m_pFirst )
	{
		CJKStage * pNode = cStage.m_pFirst;
		do
		{
			cMD.AddToXY(pNode->m_nToX,pNode->m_nToY);
			pNode = pNode->m_pNext;
		} while ( pNode != NULL );
	
		return true;
	}

	return true;
}

void CJKStage::GetNextStage()
{
	if ( m_enTeam == TM_HAN )
	{
		for ( int j,i = 0; i < DF_PANEL_HEIGHT; ++ i )
		{
			for ( j = 0; j < DF_PANEL_WIDTH; ++ j )
			{
				switch ( m_enPanel[i][j] )
				{
				case PS_HAN_JANG	: 
				case PS_HAN_SA		: GetMovableSaAndJang(j,i);break;
				case PS_HAN_JOL		: GetMovableJol(j,i);break;
				case PS_HAN_PO		: GetMovablePo(j,i);break;
				case PS_HAN_CHA		: GetMovableCha(j,i);break;
				case PS_HAN_MA		: GetMovableMa(j,i);break;
				case PS_HAN_SANG	: GetMovableSang(j,i);break;
				}
			}
		}
	} else
	{
		for ( int j,i = 0; i < DF_PANEL_HEIGHT; ++ i )
		{
			for ( j = 0; j < DF_PANEL_WIDTH; ++ j )
			{
				switch ( m_enPanel[i][j] )
				{
				case PS_CHO_JANG	: 
				case PS_CHO_SA		: GetMovableSaAndJang(j,i);break;
				case PS_CHO_JOL		: GetMovableJol(j,i);break;
				case PS_CHO_PO		: GetMovablePo(j,i);break;
				case PS_CHO_CHA		: GetMovableCha(j,i);break;
				case PS_CHO_MA		: GetMovableMa(j,i);break;
				case PS_CHO_SANG	: GetMovableSang(j,i);break;
				}
			}
		}
	}
}

bool CJKStage::CutOffTest()
{
	if ( m_enEatenPiece == PS_CHO_JANG ) 
		return true;
	if ( m_enEatenPiece == PS_HAN_JANG ) 
		return true;

	if ( m_nDepth < sm_nCutOffDepth ) return false;
	
	if ( m_enEatenPiece == PS_NOTHING )
		return true;
		
	if ( m_nDepth >= sm_nFeedOverDepth ) return true;
	
	return false;
}

int CJKStage::MaxValue(int nAlpha,int nBeta)
{
	// CUTOFF test
	if ( CutOffTest() ) return m_nScore;
	
	GetNextStage();

	if ( m_pFirst )
	{
		CJKStage * pNode = m_pFirst;
		int nValue;
		do
		{
			nValue = pNode->MinValue(nAlpha,nBeta);

			if ( nValue > nAlpha ) 
			{
				nAlpha = nValue;
			}
			if ( nAlpha >= nBeta ) 
			{
				ClearList();
				return nBeta;
			}
			pNode = pNode->m_pNext;
		} while ( pNode != NULL );
	} 
	ClearList();
	return nAlpha;
}

int CJKStage::MinValue(int nAlpha,int nBeta)
{
	// CUTOFF test
	if ( CutOffTest() ) return m_nScore;

	GetNextStage();
	if ( m_pFirst )
	{
		CJKStage * pNode = m_pFirst;

		int nValue;
		do
		{
			nValue = pNode->MaxValue(nAlpha,nBeta);
			if ( nValue < nBeta )
			{
				nBeta = nValue;
			}
			if ( nBeta <= nAlpha )
			{
				ClearList();
				return nAlpha;
			}
			pNode = pNode->m_pNext;
		} while ( pNode != NULL );
	}
	ClearList();
	return nBeta;
}

CJKStage * CJKStage::DeleteNode(CJKStage *pNode)
{
	if ( pNode )
	{
		if ( pNode->m_pPrev )
			pNode->m_pPrev->m_pNext = pNode->m_pNext;
		else
			m_pFirst = pNode->m_pNext;

		if ( pNode->m_pNext )
			pNode->m_pNext->m_pPrev = pNode->m_pPrev;
		else
			m_pLast = pNode->m_pPrev;

		-- m_nNodeCount;
	}
	return pNode;
}

MOVERESULT CJKStage::Infer(TEAM enTeam)
{
	/////////////////////////////////////////////////////
	// ALPHA - maximum value of child for max
	// BETA	 - minimum value of child for min
	/////////////////////////////////////////////////////
	// Team HAN gets plus score,so HAN must be Max
	m_bAmIMax = (enTeam == TM_HAN);
	m_enTeam = enTeam;

	m_nDepth = 0;
	sm_nTotalNode = 0;
	
	int nAlpha	= -DF_INFINITY;
	int nBeta	= +DF_INFINITY;
	int nScore;
	CJKStage *pSelected = NULL;
	
	ClearList();
	GetNextStage();

	sm_nCutOffDepth		= 6 - (m_nNodeCount / 10);
	sm_nFeedOverDepth	= 10 - (m_nNodeCount / 10);

	if ( sm_nCutOffDepth < 4 ) sm_nCutOffDepth = 3;
	if ( sm_nFeedOverDepth < 8 ) sm_nFeedOverDepth = 8;

	if ( m_pFirst == NULL ) return MR_UNMOVED;

	////////////////////////////////////////
	// In case of team HAN
	if ( m_bAmIMax )
	{
		CJKStage * pNode = m_pFirst;
		int snCount = 0;
		do
		{
			nScore = pNode->MinValue(nAlpha,nBeta);

			if ( nScore > nAlpha )
			{
				nAlpha = nScore;
				pSelected = pNode;
			}
			pNode->m_nScore = nScore;
			pNode = pNode->m_pNext;
		} while ( pNode != NULL );
	} else
	{
	////////////////////////////////////////
	// In case of team JO
		CJKStage * pNode = m_pFirst;
		int snCount = 0;
		do
		{
			nScore = pNode->MaxValue(nAlpha,nBeta);

			if ( nScore < nBeta )
			{
				nBeta = nScore;
				pSelected = pNode;
			}
			pNode->m_nScore = nScore;
			pNode = pNode->m_pNext;
		} while ( pNode != NULL );
	}
	if ( pSelected == NULL ) return MR_UNMOVED;

	m_nFromX		= pSelected->m_nFromX;
	m_nFromY		= pSelected->m_nFromY;
	m_nToX			= pSelected->m_nToX;
	m_nToY			= pSelected->m_nToY;
	m_enEatenPiece	= m_enPanel[m_nToY][m_nToX];

	MovePiece(m_nFromX,m_nFromY,m_nToX,m_nToY);
	// Save History
	sm_cHistoryStack.Push(m_nFromX,m_nFromY,m_nToX,m_nToY,m_enEatenPiece);

	m_enEatenPiece = pSelected->m_enEatenPiece;
	
	if ( m_enEatenPiece == PS_HAN_JANG ) return MR_HANDEAD;
	if ( m_enEatenPiece == PS_CHO_JANG ) return MR_CHODEAD;

	return MR_MOVED;
}

bool CJKStage::GetLastMove(SHistory & LMStore)
{
	return sm_cHistoryStack.GetLastMove(LMStore);
}

bool CJKStage::MoveBack()
{
	SHistory sH;
	if ( sm_cHistoryStack.Pop(sH) )
	{
		m_enPanel[sH.nFromY][sH.nFromX]	= m_enPanel[sH.nToY][sH.nToX];
		m_enPanel[sH.nToY][sH.nToX]		= sH.enEatenPiece;
		return true;
	} 

	return false;
}