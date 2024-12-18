========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : Jangki
========================================================================


AppWizard has created this Jangki application for you.  This application
not only demonstrates the basics of using the Microsoft Foundation classes
but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your Jangki application.

Jangki.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

Jangki.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CJangkiApp application class.

Jangki.cpp
    This is the main application source file that contains the application
    class CJangkiApp.

Jangki.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.

Jangki.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

res\Jangki.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file Jangki.rc.

res\Jangki.rc2
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.



/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the CMainFrame
    class. Edit this toolbar bitmap using the resource editor, and
    update the IDR_MAINFRAME TOOLBAR array in Jangki.rc to add
    toolbar buttons.
/////////////////////////////////////////////////////////////////////////////

AppWizard creates one document type and one view:

JangkiDoc.h, JangkiDoc.cpp - the document
    These files contain your CJangkiDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CJangkiDoc::Serialize).

JangkiView.h, JangkiView.cpp - the view of the document
    These files contain your CJangkiView class.
    CJangkiView objects are used to view CJangkiDoc objects.



/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Jangki.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is 
in a language other than the operating system's current language, you
will need to copy the corresponding localized resources MFC42XXX.DLL
from the Microsoft Visual C++ CD-ROM onto the system or system32 directory,
and rename it to be MFCLOC.DLL.  ("XXX" stands for the language abbreviation.
For example, MFC42DEU.DLL contains resources translated to German.)  If you
don't do this, some of the UI elements of your application will remain in the
language of the operating system.

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

int CJKStage::sm_nCount;
// 휴리스틱 관련 플래그
bool CJKStage::m_bPoBeforeJang;
bool CJKStage::m_bJolToCenter;
bool CJKStage::m_bCHAPOBalance;
bool CJKStage::m_bAggressivePO;
bool CJKStage::m_bAggressiveCHA;
bool CJKStage::m_bAggressiveJOL;
bool CJKStage::m_bAggressiveSANG;
bool CJKStage::m_bAggressiveMA;
bool CJKStage::m_bOnlyJangCenter;
CHistoryStack CJKStage::m_cHistoryStack;

CJKStage::CJKStage()
{
	::srand( (unsigned)time( NULL ) );
	Initialize();
}

CJKStage::CJKStage(CJKStage * pParentStage)
{
	m_pFirst = NULL;
	m_pSelectedChild = NULL;
	m_nDepth = pParentStage->m_nDepth + 1;
	m_enTeam = pParentStage->m_enTeam == TM_HAN ? TM_CHO : TM_HAN;
	m_bAmIMax = !pParentStage->IsMaxTurn();
	::memcpy(m_enPanel,pParentStage->m_enPanel,sizeof(m_enPanel));
//	GetCHAPOCount();
}

void CJKStage::GetCHAPOCount()
{
//	if ( !m_bCHAPOBalance ) return;
	m_nHANChaCount	=	0;
	m_nHANPoCount	=	0;
	m_nCHOChaCount	=	0;
	m_nCHOPoCount	=	0;
	for ( int x,y = 0; y < DF_PANEL_HEIGHT; ++ y )
		for ( x = 0; x < DF_PANEL_WIDTH; ++ x )
		{
			switch ( m_enPanel[y][x] )
			{
			case PS_HAN_CHA	: ++ m_nHANChaCount;
			case PS_HAN_PO	: ++ m_nHANPoCount;
			case PS_CHO_CHA	: ++ m_nCHOChaCount;
			case PS_CHO_PO	: ++ m_nCHOPoCount;
			}
		}
}


CJKStage::~CJKStage()
{
	ClearList();
	if ( m_pSelectedChild )
		delete m_pSelectedChild;
}

void CJKStage::Initialize()
{
	m_pFirst = NULL;
	m_pSelectedChild = NULL;
	m_cHistoryStack.ClearStack();

	::memset(m_enPanel,PS_NOTHING,sizeof(m_enPanel));
	m_enPanel[0][0] = PS_HAN_CHA;
	m_enPanel[0][1] = PS_HAN_SANG;
	m_enPanel[0][2] = PS_HAN_MA;
	m_enPanel[0][3] = PS_HAN_SA;
	m_enPanel[0][5] = PS_HAN_SA;
	m_enPanel[0][6] = PS_HAN_MA;
	m_enPanel[0][7] = PS_HAN_SANG;
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
	m_enPanel[9][1] = PS_CHO_MA;
	m_enPanel[9][2] = PS_CHO_SANG;
	m_enPanel[9][3] = PS_CHO_SA;
	m_enPanel[9][5] = PS_CHO_SA;
	m_enPanel[9][6] = PS_CHO_MA;
	m_enPanel[9][7] = PS_CHO_SANG;
	m_enPanel[9][8] = PS_CHO_CHA;
	m_enPanel[8][4] = PS_CHO_JANG;
	m_enPanel[7][1] = PS_CHO_PO;
	m_enPanel[7][7] = PS_CHO_PO;
	m_enPanel[6][0] = PS_CHO_JOL;
	m_enPanel[6][2] = PS_CHO_JOL;
	m_enPanel[6][4] = PS_CHO_JOL;
	m_enPanel[6][6] = PS_CHO_JOL;
	m_enPanel[6][8] = PS_CHO_JOL; 

	m_bPoBeforeJang		= true;
	m_bJolToCenter		= true;
	m_bCHAPOBalance		= true;
	m_bAggressivePO		= false;
	m_bAggressiveCHA	= true;
	m_bAggressiveJOL	= true;
	m_bAggressiveSANG	= false;
	m_bAggressiveMA		= false;
	m_bOnlyJangCenter	= true;

	GetCHAPOCount();
}

//////////////////////////////////////////////
// 지정된 위치로 말들이 통과할 수 있는지 체크
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
	}
}

void CJKStage::ClearList(CJKStage *pException)
{
	if ( m_pFirst )
	{
		CJKStage *pNode = m_pFirst;
		CJKStage *pTmp;
		do
		{
			pTmp = pNode->m_pNext;
			if ( pNode != pException ) delete pNode;
			pNode = pTmp;
		} while (pNode != NULL);
		m_pFirst = NULL;
	}
}
//////////////////////////////////////////////
// 지정된 위치로 말들이 이동할 수 있는지 체크
bool CJKStage::TryToAddMovableXY(int nFromX,int nFromY,int nToX,int nToY)
{
	
	if ( nToX < 0 ) return false;
	if ( nToY < 0 ) return false;
	if ( nToX >= DF_PANEL_WIDTH ) return false;
	if ( nToY >= DF_PANEL_HEIGHT ) return false;
	
	// 이동하고자 하는 곳에 같은 팀 말이 있으면 못 움직인다.
	if ( GetTeamAt(nToX,nToY) == GetTeamAt(nFromX,nFromY) ) return false;
	
	CJKStage *pNewStage = new CJKStage(this);
	pNewStage->MovePiece(nFromX,nFromY,nToX,nToY);

	pNewStage->m_nFromX			= nFromX;
	pNewStage->m_nFromY			= nFromY;
	pNewStage->m_nToX			= nToX;
	pNewStage->m_nToY			= nToY;
	pNewStage->m_enEatenPiece	= m_enPanel[nToY][nToX];

	pNewStage->m_nHANChaCount	= m_nHANChaCount;
	pNewStage->m_nHANPoCount	= m_nHANPoCount;
	pNewStage->m_nCHOChaCount	= m_nCHOChaCount;
	pNewStage->m_nCHOPoCount	= m_nCHOPoCount;

	switch ( pNewStage->m_enEatenPiece )
	{
	case PS_HAN_CHA	: -- pNewStage->m_nHANChaCount;break;
	case PS_HAN_PO	: -- pNewStage->m_nHANPoCount;break;
	case PS_CHO_CHA	: -- pNewStage->m_nCHOChaCount;break;
	case PS_CHO_PO	: -- pNewStage->m_nCHOPoCount;break;
	} 
	int nNewScore	= pNewStage->PayOff();
	if ( sm_nCount % 1000 == 0 )
	{
		TRACE("Count - %d\n",sm_nCount);
	}

	++ sm_nCount;

	///////////////////////////////////////////////////////
	// Max입장에선 최대 값을 가지는 국면을 선택하므로
	// "가치가 큰 국면순"으로 정렬해 두면 알파베타 가지치기
	// 효율이 증대된다.
	if ( m_bAmIMax )
	{
		if ( m_pFirst )
		{
			CJKStage *pNode = m_pFirst;
			do
			{
				if ( nNewScore >= pNode->GetScore() ) break;
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
				if ( nNewScore <= pNode->GetScore() ) break;
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

	return true;
}

//////////////////////////////////////////////
// 지정된 위치에 있는 말의 소속 팀을 반환
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
	// 사,장이 움직이는 영역 안에선 사,장과 똑같이 
	// 대각선으로도 움직일 수있다.뒤로는 금지
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

	// 좌로 움직여 본다.
	TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY);
	// 우로 움직여 본다.	
	TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY);
	// 직진 해 본다.
	TryToAddMovableXY(nFromX,nFromY,nFromX,nFromY + nDirection);	
}


void CJKStage::GetMovablePo(int nFromX,int nFromY)
{
	bool bMovable = false;
	// 우측 라인으로 이동시켜본다.
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
			/////////////////////////////////////////////////////
			// 포 이외의 다른 말이 나올 경우 그 다음칸부터 
			// 포가 이동할 수 있는 영역이다.
			if ( m_enPanel[nFromY][k] == PS_HAN_PO )
				break;
			if ( m_enPanel[nFromY][k] == PS_CHO_PO )
				break;
			if ( m_enPanel[nFromY][k] != PS_NOTHING )
				bMovable = true;
		}
	}
	
	bMovable = false;
	// 좌측 라인으로 이동시켜본다.
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
			/////////////////////////////////////////////////////
			// 포 이외의 다른 말이 나올 경우 그 다음칸부터 
			// 포가 이동할 수 있는 영역이다.
			if ( m_enPanel[nFromY][k] == PS_HAN_PO )
				break;
			if ( m_enPanel[nFromY][k] == PS_CHO_PO )
				break;
			if ( m_enPanel[nFromY][k] != PS_NOTHING )
				bMovable = true;
		}
	}
	bMovable = false;
	// 위로 이동시켜 본다.
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
			/////////////////////////////////////////////////////
			// 포 이외의 다른 말이 나올 경우 그 다음칸부터 
			// 포가 이동할 수 있는 영역이다.
			if ( m_enPanel[k][nFromX] == PS_HAN_PO )
				break;
			if ( m_enPanel[k][nFromX] == PS_CHO_PO )
				break;
			if ( m_enPanel[k][nFromX] != PS_NOTHING )
				bMovable = true;
		}
	}
	bMovable = false;
	// 아래로 이동시켜본다.
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

			/////////////////////////////////////////////////////
			// 포 이외의 다른 말이 나올 경우 그 다음칸부터 
			// 포가 이동할 수 있는 영역이다.
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
	// 사,장이 움직이는 영역 안에선 사,장과 똑같이 
	// 대각선으로도 움직일 수있다.
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
	// 우측 라인으로 이동시켜본다.
	for ( int k = nFromX + 1; k < DF_PANEL_WIDTH; ++ k )
	{
		// 같은팀 말이 진행로에 있으면 더 이상 못 간다.
		if ( GetTeamAt(k,nFromY) == GetTeamAt(nFromX,nFromY) )
			break;
		TryToAddMovableXY(nFromX,nFromY,k,nFromY);
		if ( GetTeamAt(k,nFromY) != PS_NOTHING )
			break;
	}
	// 좌측 라인으로 이동시켜본다.
	for ( k = nFromX - 1; k >= 0; -- k )
	{
		// 같은팀 말이 진행로에 있으면 더 이상 못 간다.
		if ( GetTeamAt(k,nFromY) == GetTeamAt(nFromX,nFromY) )
			break;
		TryToAddMovableXY(nFromX,nFromY,k,nFromY);
		if ( GetTeamAt(k,nFromY) != PS_NOTHING )
			break;
	}

	// 위로 이동시켜 본다.
	for ( k = nFromY - 1; k >= 0; -- k )
	{
		// 같은팀 말이 진행로에 있으면 더 이상 못 간다.
		if ( GetTeamAt(nFromX,k) == GetTeamAt(nFromX,nFromY) )
			break;
		TryToAddMovableXY(nFromX,nFromY,nFromX,k);
		if ( GetTeamAt(nFromX,k) != PS_NOTHING )
			break;
	}
	
	// 아래로 이동시켜본다.
	for ( k = nFromY + 1; k < DF_PANEL_HEIGHT; ++ k )
	{
		// 같은팀 말이 진행로에 있으면 더 이상 못 간다.
		if ( GetTeamAt(nFromX,k) == GetTeamAt(nFromX,nFromY) )
			break;
		TryToAddMovableXY(nFromX,nFromY,nFromX,k);
		if ( GetTeamAt(nFromX,k) != PS_NOTHING )
			break;
	}
}

void CJKStage::GetMovableMa(int nFromX,int nFromY)
{
	// 전방 & 좌측으로 이동해 본다.
	if ( IsPassable(nFromX,nFromY - 1) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY - 2);
	// 전방 & 우측으로 이동해 본다.
	if ( IsPassable(nFromX,nFromY - 1) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY - 2);
	// 후방 & 좌측으로 이동해 본다.
	if ( IsPassable(nFromX,nFromY + 1) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 1,nFromY + 2);
	// 후방 & 우측으로 이동해 본다.
	if ( IsPassable(nFromX,nFromY + 1) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX + 1,nFromY + 2);
	// 좌측 & 전방으로 이동해 본다.
	if ( IsPassable(nFromX - 1,nFromY) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY - 1);
	// 좌측 & 후방으로 이동해 본다.
	if ( IsPassable(nFromX - 1,nFromY) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY + 1);
	// 우측 & 전방으로 이동해 본다.
	if ( IsPassable(nFromX + 1,nFromY) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY - 1);
	// 우측 & 후방으로 이동해 본다.
	if ( IsPassable(nFromX + 1,nFromY) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY + 1);
}

void CJKStage::GetMovableSang(int nFromX,int nFromY)
{
	// 전방 & 좌측으로 이동해 본다.
	if ( IsPassable(nFromX,nFromY - 1) && IsPassable(nFromX - 1,nFromY - 2) ) 
		TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY - 3);

	// 전방 & 우측으로 이동해 본다.
	if ( IsPassable(nFromX,nFromY - 1) && IsPassable(nFromX + 1,nFromY - 2) )
		TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY - 3);
	
	// 후방 & 좌측으로 이동해 본다.
	if ( IsPassable(nFromX,nFromY + 1) && IsPassable(nFromX - 1,nFromY + 2) )
		TryToAddMovableXY(nFromX,nFromY,nFromX - 2,nFromY + 3);
	
	// 후방 & 우측으로 이동해 본다.
	if ( IsPassable(nFromX,nFromY + 1) && IsPassable(nFromX + 1,nFromY + 2) )
		TryToAddMovableXY(nFromX,nFromY,nFromX + 2,nFromY + 3);
	
	// 좌측 & 전방으로 이동해 본다.
	if ( IsPassable(nFromX - 1,nFromY) && IsPassable(nFromX - 2,nFromY - 1) )
		TryToAddMovableXY(nFromX,nFromY,nFromX - 3,nFromY - 2);
	
	// 좌측 & 후방으로 이동해 본다.
	if ( IsPassable(nFromX - 1,nFromY) && IsPassable(nFromX - 2,nFromY + 1) )
		TryToAddMovableXY(nFromX,nFromY,nFromX - 3,nFromY + 2);
	
	// 우측 & 전방으로 이동해 본다.
	if ( IsPassable(nFromX + 1,nFromY) && IsPassable(nFromX + 2,nFromY - 1) )
		TryToAddMovableXY(nFromX,nFromY,nFromX + 3,nFromY - 2);
	
	// 우측 & 후방으로 이동해 본다.
	if ( IsPassable(nFromX + 1,nFromY) && IsPassable(nFromX + 2,nFromY + 1) )
		TryToAddMovableXY(nFromX,nFromY,nFromX + 3,nFromY + 2);
}
/////////////////////////////////////////////////
// HAN : Plus score
// CHO : Minus score
int CJKStage::PayOff()
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
					// 포를 장 앞에 두려는 성향
					if ( m_bPoBeforeJang )
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
					// 졸을 서로 모으되 가운데쪽으로 모은다.
					if ( m_bJolToCenter )
					{
						if ( m_enPanel[j][i - 1] == PS_CHO_JOL && i > 5 )
							m_nScore -= 1;
						if ( m_enPanel[j][i + 1] == PS_CHO_JOL &&  i <= 5 )
							m_nScore -= 1;
					}
					// 졸의 공격 성향 업
					if ( m_bAggressiveJOL )
					{
						if ( j < 7 )
							m_nScore -= 1;
					}
					break;
				}
			case PS_CHO_PO		: 
				{
					// 포 2,차 1이면 차 점수가 높다.
					// 포 1,차 2이면 포 점수가 높다.
					// 그렇지 않음 포와 차는 동급이다.
					m_nScore -= DF_SCORE_PO;
					if ( m_bCHAPOBalance && m_nCHOPoCount < m_nCHOChaCount )
						m_nScore -= 2000;
					// 포의 공격 성향 업
					if ( m_bAggressivePO )
					{
						if ( j < 7 )
							m_nScore -= 1;
					}
					break;
				}
			case PS_CHO_CHA		: 
				{
					// 포 2,차 1이면 차 점수가 높다.
					// 포 1,차 2이면 포 점수가 높다.
					// 그렇지 않음 포와 차는 동급이다.
					m_nScore -= DF_SCORE_CHA;
					if ( m_bCHAPOBalance && m_nCHOChaCount < m_nCHOPoCount )
						m_nScore -= 2000;
					// 차의 공격 성향 업
					if ( m_bAggressiveCHA )
					{
						if ( j < 7 )
							m_nScore -= 2;
					}
					break;
				}
			case PS_CHO_MA		: 
				{
					m_nScore -= DF_SCORE_MA;
					// 마의 공격 성향 업
					if ( m_bAggressiveMA )
					{
						if ( j < 7 )
							m_nScore -= 1;
					}
					break;
				}
			case PS_CHO_SANG	: 
				{
					m_nScore -= DF_SCORE_SANG;
					// 상의 공격 성향 업
					if ( m_bAggressiveSANG )
					{
						if ( j < 7 )
							m_nScore -= 1;
					}
					break;
				}
			case PS_HAN_JANG	: 
				{
					m_nScore += DF_SCORE_JANG;
					// 포를 장 앞에 두려는 성향
					if ( m_bPoBeforeJang )
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
					// 졸을 서로 모으되 가운데쪽으로 모은다.
					if ( m_bJolToCenter )
					{
						if ( m_enPanel[j][i - 1] == PS_HAN_JOL && i > 5 )
							m_nScore += 1;
						if ( m_enPanel[j][i + 1] == PS_HAN_JOL && i <= 5 )
							m_nScore += 1;
					}
					// 졸의 공격 성향 업
					if ( m_bAggressiveJOL )
					{
						if ( j > 3 )
							m_nScore += 1;
					}
					break;
				}
			case PS_HAN_PO		:
				{
					// 포 2,차 1이면 차 점수가 높다.
					// 포 1,차 2이면 포 점수가 높다.
					// 그렇지 않음 포와 차는 동급이다.
					m_nScore += DF_SCORE_PO;
					if ( m_bCHAPOBalance && m_nHANPoCount < m_nHANChaCount )
						m_nScore += 2000;

					// 포의 공격 성향 업
					if ( m_bAggressivePO )
					{				
						if ( j > 3 )
							m_nScore += 1;
					}
					break;
				}
			case PS_HAN_CHA	: 
				{
					// 포 2,차 1이면 차 점수가 높다.
					// 포 1,차 2이면 포 점수가 높다.
					// 그렇지 않음 포와 차는 동급이다.
					m_nScore += DF_SCORE_CHA;
					if ( m_bCHAPOBalance && m_nHANChaCount < m_nHANPoCount )
						m_nScore += 2000;
					// 차의 공격 성향 업
					if ( m_bAggressiveCHA )
					{
						if ( j > 3 )
							m_nScore += 2;
					}
					break;
				}
			case PS_HAN_MA		: 
				{
					m_nScore += DF_SCORE_MA;
					// 마의 공격 성향 업
					if ( m_bAggressiveMA )
					{
						if ( j > 3 )
							m_nScore += 1;
					}
					break;
				}
			case PS_HAN_SANG	: 
				{
					m_nScore += DF_SCORE_SANG;
					// 상의 공격 성향 업
					if ( m_bAggressiveSANG )
					{
						if ( j > 3 )
							m_nScore += 1;
					}
					break;
				}
			}
		}
	// 장 이외에는 장 영역 중간에 오는 걸 막는다.
	if ( m_bOnlyJangCenter )
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
	m_cHistoryStack.SaveHistory(fp);

	fclose(fp);

	return true;
}

bool CJKStage::LoadStage(const char * filename)
{
	FILE *fp = fopen(filename,"r+b");
	if ( fp == NULL ) return false;

	fread(m_enPanel,1,sizeof(m_enPanel),fp);
	m_cHistoryStack.LoadHistory(fp);
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

	if ( bResult )
	{
		// History 저장
		m_cHistoryStack.Push(nFromX,nFromY,nToX,nToY,enEatenPiece);
	}

	if ( !bResult )	return MR_UNMOVED;

	if ( enEatenPiece == PS_HAN_JANG ) return MR_HANDEAD;
	if ( enEatenPiece == PS_CHO_JANG ) return MR_CHODEAD;
	
	return MR_MOVED;
}

bool CJKStage::GetNextPieceLoc(int nFromX,int nFromY,CMoveData & cMD)
{
	if ( nFromX < 0 || nFromX >= DF_PANEL_WIDTH ) return false;
	if ( nFromY < 0 || nFromY >= DF_PANEL_HEIGHT ) return false;
	ClearList();
	cMD.Reset();
	/////////////////////////////////////////////////////
	// 각 말 별로 움직일 수 있는 범위를 구한다.
	switch ( m_enPanel[nFromY][nFromX] )
	{
	case PS_CHO_JANG	: 
	case PS_CHO_SA		: GetMovableSaAndJang(nFromX,nFromY);break;
	case PS_CHO_JOL		: GetMovableJol(nFromX,nFromY);break;
	case PS_CHO_PO		: GetMovablePo(nFromX,nFromY);break;
	case PS_CHO_CHA		: GetMovableCha(nFromX,nFromY);break;
	case PS_CHO_MA		: GetMovableMa(nFromX,nFromY);break;
	case PS_CHO_SANG	: GetMovableSang(nFromX,nFromY);break;
	case PS_HAN_JANG	: 
	case PS_HAN_SA		: GetMovableSaAndJang(nFromX,nFromY);break;
	case PS_HAN_JOL		: GetMovableJol(nFromX,nFromY);break;
	case PS_HAN_PO		: GetMovablePo(nFromX,nFromY);break;
	case PS_HAN_CHA		: GetMovableCha(nFromX,nFromY);break;
	case PS_HAN_MA		: GetMovableMa(nFromX,nFromY);break;
	case PS_HAN_SANG	: GetMovableSang(nFromX,nFromY);break;
	}
	cMD.SetFromXY(nFromX,nFromY);

	if ( m_pFirst )
	{
		CJKStage * pNode = m_pFirst;
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
	if ( m_enEatenPiece == PS_CHO_JANG ) return true;
	if ( m_enEatenPiece == PS_HAN_JANG ) return true;
	
	////////////////////////////////////////////////////////////
	// FeedOver를 할 때 인간이 둘 수가 정적일때 그만둬야 한다.
	// 컴퓨터 수가 정적이라서 그만 둘 경우 그 다음 인간 수에서
	// 치명타를 입을 수가 있다.
	if ( m_enEatenPiece != PS_NOTHING )
		if ( m_nDepth <= DF_FEEDOVER_DEPTH ) 
			return false;

	if ( m_nDepth >= DF_CUTOFF_DEPTH ) return true;

/*
	if ( m_enEatenPiece == PS_CHO_JANG ) return true;
	if ( m_enEatenPiece == PS_HAN_JANG ) return true;

	/////////////////////////////////////////////////////
	// 기본적으로 몇 수 앞까지는 무조건 내다본다.
	if ( m_nDepth <= DF_CUTOFF_DEPTH ) return false;

	/////////////////////////////////////////////////////
	// FEED OVER Depth까지 오면 추론을 완전히 그만둔다.
	if ( m_nDepth >= DF_FEEDOVER_DEPTH ) return true;

	/////////////////////////////////////////////////////
	// FEED OVER를 한다.즉 "인간이 둘 국면"이 정적일 때
	// 추론을 그만둔다.Depth 가 홀수이면 인간이 둘 수.
//	if ( m_nDepth % 2 == 1 && m_enEatenPiece == PS_NOTHING )
	if ( m_enEatenPiece == PS_NOTHING )
		return true;
*/	
	return true;
	
	return false;
}

int CJKStage::MaxValue(int nAlpha,int nBeta)
{
	// CUTOFF test
	if ( CutOffTest() ) return m_nScore;
	
	////////////////////////////////////////////////////
	// GetNextStage()에서 가능한 다음 국면들을 효율적으로
	// 소트(알파베타 가지치기에)한 후 리스트로 만들어둔다.
	GetNextStage();
	if ( m_pFirst )
	{
		CJKStage * pNode = m_pFirst;
		int nMinValue;
		// 다음 국면을 모두 테스트해본다.
		do
		{
			nMinValue = pNode->MinValue(nAlpha,nBeta);
			if ( nMinValue > nAlpha )
			{
				nAlpha = nMinValue;
				m_pSelectedChild = pNode;
			}
			////////////////////////////////////////////////////////
			// 부모인 Min은 작은 점수를 선택하려 할 것인데 내(Max)가
			// 만든 국면의 점수가 Beta보다 커버리면 부모인 Min의 
			// 선택권에서 벗아나므로 더 이상 진행에도 의미가 없다.
			// 나(Max)는 가장 큰 점수를 가지는 국면을 택하기 때문이다.
			if ( nAlpha >= nBeta ) break;
			pNode = pNode->m_pNext;
		} while ( pNode != NULL );
	} 
	// 선택된 다음 국면은 메모리상에서 삭제하지 않는다.
	ClearList(m_pSelectedChild);
	return nAlpha;
}

int CJKStage::MinValue(int nAlpha,int nBeta)
{
	// CUTOFF test
	if ( CutOffTest() ) return m_nScore;

	////////////////////////////////////////////////////
	// GetNextStage()에서 가능한 다음 국면들을 효율적으로
	// 소트(알파베타 가지치기에)한 후 리스트로 만들어둔다.
	GetNextStage();
	if ( m_pFirst )
	{
		CJKStage * pNode = m_pFirst;
		int nMaxValue;
		do
		{
			nMaxValue = pNode->MaxValue(nAlpha,nBeta);
			if ( nMaxValue < nBeta )
			{
				nBeta = nMaxValue;
				m_pSelectedChild = pNode;
			}
			////////////////////////////////////////////////////////
			// 부모인 Max는 큰 점수를 선택하려 할 것인데 내(Min)가
			// 만든 국면의 점수가 Alpha보다 작으면 부모인 Max의 
			// 선택권에서 벗아나므로 더 이상 진행에도 의미가 없다.
			// 나(Min)는 가장 작은 점수를 가지는 국면을 택하기 때문이다.
			if ( nBeta <= nAlpha ) break;
			pNode = pNode->m_pNext;
		} while ( pNode != NULL );
	}
	// 선택된 다음 국면은 메모리상에서 삭제하지 않는다.
	ClearList(m_pSelectedChild);
	return nBeta;
}

MOVERESULT CJKStage::Infer(TEAM enTeam)
{
	/////////////////////////////////////////////////////
	// ALPHA - Max가 선택할 다음 국면 후보의 값(최대치)
	// BETA	 - Min이 선택할 다음 국면 후보의 값(최소치)
	/////////////////////////////////////////////////////
	// 한나라가 Plus 점수이므로 Max가 되어야 한다.
	m_bAmIMax = (enTeam == TM_HAN);
	m_enTeam = enTeam;

	m_nDepth = 0;
	sm_nCount = 0;
	CJKStage *pSelected = NULL;
	/////////////////////////////////////////////////////
	// 알파는 최대한 작게,베타는 최대한 크게 초기화함으로서
	// 최초에는 가장 깊숙히까지 가지를 뻗을 수 있게 한다.
	// (알파베타 가지치기를 당하지 않게 한다.)
	int nAlpha	= -9999999;
	int nBeta	= +9999999;
	int nScore;
	
	ClearList();
	GetNextStage();
	int nSelectedNo;
	////////////////////////////////////////
	// 한나라 입장에서 추론한다면...
	if ( m_bAmIMax )
	{
		if ( m_pFirst )
		{
			CJKStage * pNode = m_pFirst;
			static int snCount = 0;
			do
			{
				nScore = pNode->MinValue(nAlpha,nBeta);

				CJKStage *pTmp = pNode;
				do
				{
					TRACE("Way No.%d,Depth - %d : %d,%d to %d,%d\n",snCount ,
													pTmp->m_nDepth,
													pTmp->m_nFromX,
													pTmp->m_nFromY,
													pTmp->m_nToX,
													pTmp->m_nToY);
					pTmp = pTmp->m_pSelectedChild;
				} while ( pTmp != NULL );
				TRACE("Way No.%d Score : %d\n\n",snCount ++,nScore);

				if ( nScore > nAlpha )
				{
					nAlpha = nScore;
					pSelected = pNode;
					nSelectedNo = snCount;
				}
				pNode = pNode->m_pNext;
			} while ( pNode != NULL );
		}
	} else
	{
	////////////////////////////////////////
	// 조나라 입장에서 추론한다면...
		if ( m_pFirst )
		{
			CJKStage * pNode = m_pFirst;
			static int snCount = 0;
			do
			{
				nScore = pNode->MaxValue(nAlpha,nBeta);

				CJKStage *pTmp = pNode;
				do
				{
					TRACE("Way No.%d,Depth - %d : %d,%d to %d,%d\n",snCount ,
													pTmp->m_nDepth,
													pTmp->m_nFromX,
													pTmp->m_nFromY,
													pTmp->m_nToX,
													pTmp->m_nToY);
					pTmp = pTmp->m_pSelectedChild;
				} while ( pTmp != NULL );
				snCount ++;
				TRACE("Way No.%d Score : %d\n\n",snCount,nScore);

				if ( nScore < nBeta )
				{
					nBeta = nScore;
					pSelected = pNode;
					nSelectedNo = snCount;
				}

				pNode = pNode->m_pNext;
			} while ( pNode != NULL );
		}

	}
	if ( pSelected )
	{
		TRACE("Selected No. %d with Score %d\n",-- nSelectedNo,nScore);
		m_nFromX		= pSelected->m_nFromX;
		m_nFromY		= pSelected->m_nFromY;
		m_nToX			= pSelected->m_nToX;
		m_nToY			= pSelected->m_nToY;
		m_enEatenPiece	= m_enPanel[m_nToY][m_nToX];

		MovePiece(m_nFromX,m_nFromY,m_nToX,m_nToY);
		// History 저장
		m_cHistoryStack.Push(m_nFromX,m_nFromY,m_nToX,m_nToY,m_enEatenPiece);
	
		m_enEatenPiece = pSelected->m_enEatenPiece;
		
		if ( m_enEatenPiece == PS_HAN_JANG ) return MR_HANDEAD;
		if ( m_enEatenPiece == PS_CHO_JANG ) return MR_CHODEAD;

		return MR_MOVED;
	}

	return MR_UNMOVED;
}

bool CJKStage::GetLastMove(SHistory & LMStore)
{
	return m_cHistoryStack.GetLastMove(LMStore);
}

bool CJKStage::MoveBack()
{
	SHistory sH;
	if ( m_cHistoryStack.Pop(sH) )
	{
		m_enPanel[sH.nFromY][sH.nFromX]	= m_enPanel[sH.nToY][sH.nToX];
		m_enPanel[sH.nToY][sH.nToX]		= sH.enEatenPiece;
		return true;
	} 

	return false;
}




