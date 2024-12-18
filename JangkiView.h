// JangkiView.h : interface of the CJangkiView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_JANGKIVIEW_H__530CA6ED_6E30_11D3_B509_BEB3B0969997__INCLUDED_)
#define AFX_JANGKIVIEW_H__530CA6ED_6E30_11D3_B509_BEB3B0969997__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "JKStage.h"

#define DF_SMALL_RECT_SIDE		40
#define DF_LEFTUP_MARGIN		50
#define DF_LOC_PIECE			20
#define DF_SM_PIECE				28
#define DF_MID_PIECE			36
#define DF_BIG_PIECE			40
#define DF_LOC_PIECE_MARGIN		(DF_LEFTUP_MARGIN - DF_LOC_PIECE / 2)
#define DF_SM_PIECE_MARGIN		(DF_LEFTUP_MARGIN - DF_SM_PIECE / 2)
#define DF_MID_PIECE_MARGIN		(DF_LEFTUP_MARGIN - DF_MID_PIECE / 2)
#define DF_BIG_PIECE_MARGIN		(DF_LEFTUP_MARGIN - DF_BIG_PIECE / 2)

#define DF_COMPUTER_TEAM		TM_HAN

enum GAMESTATE { GS_HUMANSELECT,GS_HUMANMOVE,GS_COMPUTER };
class CJangkiView : public CView
{
protected: // create from serialization only
	CJangkiView();
	DECLARE_DYNCREATE(CJangkiView)

// Attributes
public:
	CJangkiDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJangkiView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJangkiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CJangkiView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBackmove();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void DrawPanel(CDC *pDC);
private:
	CJKStage		m_cStage;
	CMoveData		m_cMD;
	GAMESTATE		m_enGameState;
	bool			m_bInferAfterDraw;
};

#ifndef _DEBUG  // debug version in JangkiView.cpp
inline CJangkiDoc* CJangkiView::GetDocument()
   { return (CJangkiDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JANGKIVIEW_H__530CA6ED_6E30_11D3_B509_BEB3B0969997__INCLUDED_)
