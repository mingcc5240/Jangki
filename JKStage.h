// JKStage.h: interface for the CJKStage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JKSTAGE_H__530CA6F7_6E30_11D3_B509_BEB3B0969997__INCLUDED_)
#define AFX_JKSTAGE_H__530CA6F7_6E30_11D3_B509_BEB3B0969997__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

#define DF_PANEL_WIDTH		9
#define DF_PANEL_HEIGHT		10

#define DF_SCORE_JANG		500000
#define DF_SCORE_SA			10000
#define DF_SCORE_JOL		5000
#define DF_SCORE_PO			25000

#define DF_SCORE_CHA		32000
#define DF_SCORE_MA			15000
#define DF_SCORE_SANG		9000

#define DF_INFINITY			9999999

enum MOVERESULT { MR_HANDEAD,MR_CHODEAD,MR_UNMOVED,MR_MOVED };

enum TEAM { TM_NOTHING,TM_HAN,TM_CHO };
enum PIECES	{	PS_NOTHING,
				PS_CHO_JANG,
				PS_CHO_SA,
				PS_CHO_JOL,
				PS_CHO_PO,
				PS_CHO_CHA,
				PS_CHO_MA,
				PS_CHO_SANG,
				PS_BOUNDARY,
				PS_HAN_JANG,
				PS_HAN_SA,
				PS_HAN_JOL,
				PS_HAN_PO,
				PS_HAN_CHA,
				PS_HAN_MA,
				PS_HAN_SANG,
			};

class CMoveData {
public:
	CMoveData()
	{
		m_nIndexCount = 0;
	}
	void SetFromXY(int nX,int nY) 
	{
		m_nIndexCount = 0;
		m_nFromX = nX;
		m_nFromY = nY;
	}
	void AddToXY(int nX,int nY)
	{	
		m_nToX[m_nIndexCount] = nX;
		m_nToY[m_nIndexCount] = nY;
		++ m_nIndexCount;
	}
	int GetIndexCount() { return m_nIndexCount; }
	int GetFromX() { return m_nFromX; }
	int GetFromY() { return m_nFromY; }
	int GetToX(int nIndex) { return m_nToX[nIndex]; }
	int GetToY(int nIndex) { return m_nToY[nIndex]; }
	void Reset() { m_nIndexCount = 0; }

private:
	int	m_nFromX;			// X location of a piece to move from
	int m_nFromY;			// Y location of a piece to move from
	int m_nToX[40];			// X location of a piece to move to
	int m_nToY[40];			// Y location of a piece to move to
	int m_nIndexCount;		// Available location count
};

typedef struct tagSHistory {
	int			nFromX;
	int			nFromY;
	int			nToX;
	int			nToY;
	PIECES		enEatenPiece;
	struct tagSHistory	*pPrev;
} SHistory;

class CHistoryStack {
public:
	CHistoryStack()
	{
		m_nDepth	= 0;
		m_pLast		= NULL;
	}
	~CHistoryStack()
	{
		ClearStack();
	}

public:
	void ClearStack()
	{
		if ( m_pLast )
		{
			SHistory *pNode = m_pLast;
			SHistory *pTmp;
			do
			{
				pTmp = pNode->pPrev;
				delete pNode;
				pNode = pTmp;
			} while ( pNode != NULL );
		}
		m_pLast = NULL;
	}

	void SaveHistory(FILE *fp)
	{
		fwrite(&m_nDepth,1,sizeof(int),fp);
		if ( m_pLast )
		{
			SHistory *pNode = m_pLast;
			do
			{
				fwrite(&pNode->enEatenPiece,1,sizeof(PIECES),fp);
				fwrite(&pNode->nFromX,1,sizeof(int),fp);
				fwrite(&pNode->nFromY,1,sizeof(int),fp);
				fwrite(&pNode->nToX,1,sizeof(int),fp);
				fwrite(&pNode->nToY,1,sizeof(int),fp);

				pNode = pNode->pPrev;
			} while ( pNode != NULL );
		}
	}
	void LoadHistory(FILE *fp)
	{
		ClearStack();
		SHistory *pNode = NULL,*pPrev = NULL;

		fread(&m_nDepth,1,sizeof(int),fp);
		
		for ( int i = 0; i < m_nDepth; ++ i )
		{
			pNode = new SHistory();
			pNode->pPrev = pPrev;
			pPrev = pNode;
		}
		m_pLast = pPrev;
		if ( m_pLast )
		{
			do
			{
				fread(&pNode->enEatenPiece,1,sizeof(PIECES),fp);
				fread(&pNode->nFromX,1,sizeof(int),fp);
				fread(&pNode->nFromY,1,sizeof(int),fp);
				fread(&pNode->nToX,1,sizeof(int),fp);
				fread(&pNode->nToY,1,sizeof(int),fp);

				pNode = pNode->pPrev;
			} while ( pNode != NULL );
		}
	}
	void Push(int nFromX,int nFromY,int nToX,int nToY,PIECES enEatenPiece)
	{
		SHistory *pNewHistory = new SHistory();
		pNewHistory->enEatenPiece	= enEatenPiece;
		pNewHistory->nFromX			= nFromX;
		pNewHistory->nFromY			= nFromY;
		pNewHistory->nToX			= nToX;
		pNewHistory->nToY			= nToY;
		
		pNewHistory->pPrev			= m_pLast;
		m_pLast						= pNewHistory;
		++ m_nDepth;
	}
	
	bool Pop(SHistory & PopStore)
	{
		if ( m_pLast == NULL ) return false;

		PopStore.enEatenPiece	= m_pLast->enEatenPiece;
		PopStore.nFromX			= m_pLast->nFromX;
		PopStore.nFromY			= m_pLast->nFromY;
		PopStore.nToX			= m_pLast->nToX;
		PopStore.nToY			= m_pLast->nToY;

		SHistory	*pTmp		= m_pLast;
		m_pLast					= m_pLast->pPrev;
		delete pTmp;
		-- m_nDepth;
		
		return true;
	} 
	
	bool GetLastMove(SHistory & LMStore)
	{
		if ( m_pLast == NULL ) 
			return false;

		LMStore.enEatenPiece	= m_pLast->enEatenPiece;
		LMStore.nFromX			= m_pLast->nFromX;
		LMStore.nFromY			= m_pLast->nFromY;
		LMStore.nToX			= m_pLast->nToX;
		LMStore.nToY			= m_pLast->nToY;

		return true;
	} 

	int	GetDepth() { return m_nDepth; }

private:
	int			m_nDepth;
	SHistory	*m_pLast;
};

class CJKStage {
public:
	CJKStage();
	virtual ~CJKStage();

protected:
	CJKStage(CJKStage * pParentStage);
	bool	IsPassable(int nX,int nY);
	void	GetMovableSaAndJang(int nFromX,int nFromY);
	void	GetMovableJol(int nFromX,int nFromY);
	void	GetMovablePo(int nFromX,int nFromY);
	void	GetMovableCha(int nFromX,int nFromY);
	void	GetMovableMa(int nFromX,int nFromY);
	void	GetMovableSang(int nFromX,int nFromY);
	bool	TryToAddMovableXY(int nFromX,int nFromY,int nToX,int nToY);
	void	ClearList();
	bool	MovePiece(int nFromX,int nFromY,int nToX,int nToY);
	int		GetDepth() { return m_nDepth; }
	bool	IsMaxTurn() { return m_bAmIMax; }
	int		MaxValue(int nAlpha,int nBeta);
	int		MinValue(int nAlpha,int nBeta);
	void	AddNextStageEffectively(CJKStage *pNewStage);
	int		Evaluate();
	bool	CutOffTest();
	void	GetNextStage();
	CJKStage * DeleteNode(CJKStage *pNode);

public:
	void	Initialize();
	TEAM	GetTeamAt(int nX,int nY);
	MOVERESULT Infer(TEAM enTeam);
	MOVERESULT HumanMove(int nFromX,int nFromY,int nToX,int nToY);
	bool	GetNextPieceLoc(int nX,int nY,CMoveData & cMD);
	PIECES	GetPiece(int nIndexX,int nIndexY) { return m_enPanel[nIndexY][nIndexX]; }
	bool	GetLastMove(SHistory & LMStore);
	bool	SaveStage(const char * filename);
	bool	LoadStage(const char * filename);
	bool	MoveBack();
	int		GetCutOffDepth() { return sm_nCutOffDepth; }
	int		GetFeedOverDepth() { return sm_nFeedOverDepth; }

public:
	static	CHistoryStack	sm_cHistoryStack;

private:
	PIECES	m_enPanel[DF_PANEL_HEIGHT][DF_PANEL_WIDTH];
	int		m_nDepth;
	int		m_nScore;
	bool	m_bAmIMax;
	PIECES	m_enEatenPiece;
	int		m_nFromX;
	int		m_nFromY;
	int		m_nToX;
	int		m_nToY;
	int		m_nNodeCount;

	static	int sm_nTotalNode;
	
	// Flags for heuristic
	static int sm_nCutOffDepth;
	static int sm_nFeedOverDepth;

	static bool sm_bPoBeforeJang;
	static bool sm_bGatherJol;
	static bool sm_bAggressivePO;
	static bool sm_bAggressiveCHA;
	static bool sm_bAggressiveJOL;
	static bool sm_bAggressiveSANG;
	static bool sm_bAggressiveMA;
	static bool sm_bOnlyJangCenter;

	TEAM	m_enTeam;

	CJKStage * m_pPrev;
	CJKStage * m_pNext;
	CJKStage * m_pFirst;
	CJKStage * m_pLast;
	CJKStage * m_pParent;
};

#endif // !defined(AFX_JKSTAGE_H__530CA6F7_6E30_11D3_B509_BEB3B0969997__INCLUDED_)