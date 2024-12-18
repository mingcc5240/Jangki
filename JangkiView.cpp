// JangkiView.cpp : implementation of the CJangkiView class
//

#include "stdafx.h"
#include "Jangki.h"

#include "JangkiDoc.h"
#include "JangkiView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJangkiView

IMPLEMENT_DYNCREATE(CJangkiView, CView)

BEGIN_MESSAGE_MAP(CJangkiView, CView)
	//{{AFX_MSG_MAP(CJangkiView)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDTB_BACKMOVE, OnBackmove)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJangkiView construction/destruction

CJangkiView::CJangkiView()
{
	m_enGameState = GS_HUMANSELECT;
	m_bInferAfterDraw = false;
}

CJangkiView::~CJangkiView()
{
}

void CJangkiView::DrawPanel(CDC *pDC)
{
	int i,j;
	int nLocNo = 0;
	CString szLocNo;
	// Draw the horizontal line
	for ( i = 0; i < DF_PANEL_HEIGHT; ++ i )
	{
		szLocNo.Format("%d",nLocNo ++);
		pDC->TextOut(DF_LEFTUP_MARGIN - 40,i * DF_SMALL_RECT_SIDE + DF_LEFTUP_MARGIN,szLocNo);
		pDC->MoveTo(DF_LEFTUP_MARGIN,i * DF_SMALL_RECT_SIDE + DF_LEFTUP_MARGIN);
		pDC->LineTo(DF_LEFTUP_MARGIN + DF_SMALL_RECT_SIDE * (DF_PANEL_WIDTH - 1),
					i * DF_SMALL_RECT_SIDE + DF_LEFTUP_MARGIN);
	}

	nLocNo = 0;
	// Draw the vertical line
	for ( i = 0; i < DF_PANEL_WIDTH; ++ i )
	{
		szLocNo.Format("%d",nLocNo ++);
		pDC->TextOut(DF_LEFTUP_MARGIN + i * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN - 40,szLocNo);
		pDC->MoveTo(DF_LEFTUP_MARGIN + i * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN);
		pDC->LineTo(DF_LEFTUP_MARGIN + i * DF_SMALL_RECT_SIDE,
					DF_SMALL_RECT_SIDE * (DF_PANEL_HEIGHT - 1)+ DF_LEFTUP_MARGIN);
	}
	// Draw the diagonal line of upper side
	pDC->MoveTo(DF_LEFTUP_MARGIN + 3 * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN);
	pDC->LineTo(DF_LEFTUP_MARGIN + 5 * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN + 2 * DF_SMALL_RECT_SIDE);
	pDC->MoveTo(DF_LEFTUP_MARGIN + 3 * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN + 2 * DF_SMALL_RECT_SIDE);
	pDC->LineTo(DF_LEFTUP_MARGIN + 5 * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN);

	// Draw the diagonal line downword
	pDC->MoveTo(DF_LEFTUP_MARGIN + 3 * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN + 7 * DF_SMALL_RECT_SIDE);
	pDC->LineTo(DF_LEFTUP_MARGIN + 5 * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN + 9 * DF_SMALL_RECT_SIDE);
	pDC->MoveTo(DF_LEFTUP_MARGIN + 3 * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN + 9 * DF_SMALL_RECT_SIDE);
	pDC->LineTo(DF_LEFTUP_MARGIN + 5 * DF_SMALL_RECT_SIDE,DF_LEFTUP_MARGIN + 7 * DF_SMALL_RECT_SIDE);

	CBrush CHOBrush(RGB(0xFF,0x0,0xFF));
	CBrush HANBrush(RGB(0x0,0xFF,0xFF));
	CBrush MovableLocBrush(RGB(0xFF,0xFF,0xF0));
	CBrush *pOldBrush = pDC->GetCurrentBrush();

	pDC->SetBkMode(TRANSPARENT);
	
	for ( i = 0; i < DF_PANEL_HEIGHT; ++ i )
		for ( j = 0; j < DF_PANEL_WIDTH; ++ j )
		{
			switch ( m_cStage.GetPiece(j,i) )
			{
			case PS_CHO_JANG		: 
				{
					pDC->SelectObject(CHOBrush);
					CRect rect(DF_BIG_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_BIG_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_BIG_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_BIG_PIECE,
							   DF_BIG_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_BIG_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("JANG",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_CHO_SA		: 
				{
					pDC->SelectObject(CHOBrush);
					CRect rect(DF_SM_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_SM_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_SM_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_SM_PIECE,
							   DF_SM_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_SM_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("SA",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_CHO_JOL		: 
				{
					pDC->SelectObject(CHOBrush);
					CRect rect(DF_SM_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_SM_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_SM_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_SM_PIECE,
							   DF_SM_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_SM_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("JOL",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_CHO_PO		: 
				{
					pDC->SelectObject(CHOBrush);
					CRect rect(DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_MID_PIECE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_MID_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("PO",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_CHO_CHA		: 
				{
					pDC->SelectObject(CHOBrush);
					CRect rect(DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_MID_PIECE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_MID_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("CHA",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_CHO_MA		: 
				{
					pDC->SelectObject(CHOBrush);
					CRect rect(DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_MID_PIECE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_MID_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("MA",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_CHO_SANG		: 
				{
					pDC->SelectObject(CHOBrush);
					CRect rect(DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_MID_PIECE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_MID_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("SANG",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_HAN_JANG		: 
				{
					pDC->SelectObject(HANBrush);
					CRect rect(DF_BIG_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_BIG_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_BIG_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_BIG_PIECE,
							   DF_BIG_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_BIG_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("JANG",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_HAN_SA		: 
				{
					pDC->SelectObject(HANBrush);
					CRect rect(DF_SM_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_SM_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_SM_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_SM_PIECE,
							   DF_SM_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_SM_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("SA",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_HAN_JOL		: 
				{
					pDC->SelectObject(HANBrush);
					CRect rect(DF_SM_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_SM_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_SM_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_SM_PIECE,
							   DF_SM_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_SM_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("JOL",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_HAN_PO		: 
				{
					pDC->SelectObject(HANBrush);
					CRect rect(DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_MID_PIECE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_MID_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("PO",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_HAN_CHA		: 
				{
					pDC->SelectObject(HANBrush);
					CRect rect(DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_MID_PIECE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_MID_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("CHA",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_HAN_MA		: 
				{
					pDC->SelectObject(HANBrush);
					CRect rect(DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_MID_PIECE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_MID_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("MA",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}
			case PS_HAN_SANG		: 
				{
					pDC->SelectObject(HANBrush);
					CRect rect(DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE,
							   DF_MID_PIECE_MARGIN + j * DF_SMALL_RECT_SIDE + DF_MID_PIECE,
							   DF_MID_PIECE_MARGIN + i * DF_SMALL_RECT_SIDE + DF_MID_PIECE);
					pDC->Ellipse(rect);
					pDC->DrawText("SANG",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					break;
				}

			}
		}
	pDC->SelectObject(MovableLocBrush);
	for ( i = 0; i < m_cMD.GetIndexCount(); ++ i )
	{
		CRect rect(DF_LOC_PIECE_MARGIN + m_cMD.GetToX(i) * DF_SMALL_RECT_SIDE,
				   DF_LOC_PIECE_MARGIN + m_cMD.GetToY(i) * DF_SMALL_RECT_SIDE,
				   DF_LOC_PIECE_MARGIN + m_cMD.GetToX(i) * DF_SMALL_RECT_SIDE + DF_LOC_PIECE,
				   DF_LOC_PIECE_MARGIN + m_cMD.GetToY(i) * DF_SMALL_RECT_SIDE + DF_LOC_PIECE);
		pDC->Ellipse(rect);
	}
	pDC->SelectObject(pOldBrush);

	SHistory sH;
	if ( m_cStage.GetLastMove(sH) )
	{
		CPen PathPen(PS_SOLID,3,RGB(0xFF,0,0));
		CPen CircPen(PS_DOT,1,RGB(0x0,0x1F,0x0));
		CPen *pOldPen = pDC->SelectObject(&PathPen);

		// Draw the path
		pDC->MoveTo(DF_LEFTUP_MARGIN + sH.nFromX * DF_SMALL_RECT_SIDE,
					DF_LEFTUP_MARGIN + sH.nFromY * DF_SMALL_RECT_SIDE);

		pDC->LineTo(DF_LEFTUP_MARGIN + sH.nToX * DF_SMALL_RECT_SIDE,
					DF_LEFTUP_MARGIN + sH.nToY * DF_SMALL_RECT_SIDE);

		// Draw a circle where the moved piece placed
		pDC->SelectObject(&CircPen);
		CRect rect(DF_SM_PIECE_MARGIN + sH.nFromX * DF_SMALL_RECT_SIDE,
				   DF_SM_PIECE_MARGIN + sH.nFromY * DF_SMALL_RECT_SIDE,
				   DF_SM_PIECE_MARGIN + sH.nFromX * DF_SMALL_RECT_SIDE + DF_SM_PIECE,
				   DF_SM_PIECE_MARGIN + sH.nFromY * DF_SMALL_RECT_SIDE + DF_SM_PIECE);
		pDC->Ellipse(rect);

		pDC->SelectObject(pOldPen);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CJangkiView drawing

void CJangkiView::OnDraw(CDC* pDC)
{
	CJangkiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	DrawPanel(pDC);
	if ( m_bInferAfterDraw )
	{
		CMainFrame *pFrame = (CMainFrame *) AfxGetMainWnd();
		CString str;
		str.Format("Last Cutoff Depth : %d , Last Feed Over Depth : %d , INFERING . . .",
			m_cStage.GetCutOffDepth(),m_cStage.GetFeedOverDepth());
		pFrame->SetStatusBackString(str);
		
		switch ( m_cStage.Infer(DF_COMPUTER_TEAM) )
		{
		case MR_HANDEAD	:
			{
				AfxMessageBox("JO Win!");
				break;
			}
		case MR_CHODEAD	:
			{
				AfxMessageBox("Han Win");
				break;
			}
		}
		m_bInferAfterDraw = false;
		pFrame->SetStatusBackString("Select Piece to move");
		Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJangkiView printing

BOOL CJangkiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CJangkiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}


/////////////////////////////////////////////////////////////////////////////
// CJangkiView diagnostics

#ifdef _DEBUG
void CJangkiView::AssertValid() const
{
	CView::AssertValid();
}

void CJangkiView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CJangkiDoc* CJangkiView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CJangkiDoc)));
	return (CJangkiDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJangkiView message handlers

void CJangkiView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	int nX = (point.x - DF_BIG_PIECE_MARGIN) / DF_SMALL_RECT_SIDE;
	int nY = (point.y - DF_BIG_PIECE_MARGIN) / DF_SMALL_RECT_SIDE;

	switch ( m_enGameState )
	{
	case GS_HUMANSELECT	:
		{
			if ( m_cStage.GetTeamAt(nX,nY) == DF_COMPUTER_TEAM ) break;
			if ( m_cStage.GetNextPieceLoc(nX,nY,m_cMD) )
				if ( m_cMD.GetIndexCount() > 0 ) 
				{
					m_enGameState = GS_HUMANMOVE;
					Invalidate();
				}
			break;
		}
	case GS_HUMANMOVE	:
		{
			bool bMoved = false;
			for ( int i = 0; i < m_cMD.GetIndexCount(); ++ i )
			{
				if ( nX == m_cMD.GetToX(i) && nY == m_cMD.GetToY(i) )
				{
					switch ( m_cStage.HumanMove(m_cMD.GetFromX(),m_cMD.GetFromY(),m_cMD.GetToX(i),m_cMD.GetToY(i)) )
					{
					case MR_HANDEAD	:
						{
							AfxMessageBox("JO Win");
							break;
						}
					case MR_CHODEAD	:
						{
							AfxMessageBox("Han Win");
							break;
						}
					}
					m_enGameState = GS_HUMANSELECT;
					m_cMD.Reset();
					bMoved = true;
					m_bInferAfterDraw = true;
					Invalidate();
					break;
				}
			}
			if ( !bMoved )
			{
				m_cMD.Reset();
				m_enGameState = GS_HUMANSELECT;
				Invalidate();
			}
			break;

		}
	}

	CView::OnLButtonDown(nFlags, point);
}

void CJangkiView::OnBackmove() 
{
	if ( !m_cStage.MoveBack() )
	{
		AfxMessageBox("It's impossible to UNDO");
		return;
	}
	if ( !m_cStage.MoveBack() )
	{
		AfxMessageBox("It's impossible to UNDO");
		return;
	}

	Invalidate();
}

void CJangkiView::OnFileOpen() 
{
	char szFilter[] = "Sebals JangKi Files (*.sjk)|*.sjk|All Files (*.*)|*.*||";
	CFileDialog fd(true,"sjk",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if ( fd.DoModal() == IDOK )
	{
		m_cStage.LoadStage(fd.GetFileName());
		Invalidate();
	}
}

void CJangkiView::OnFileSave() 
{
	char szFilter[] = "Sebals JangKi Files (*.sjk)|*.sjk|All Files (*.*)|*.*||";
	CFileDialog fd(false,"sjk",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);
	if ( fd.DoModal() == IDOK )
	{
		m_cStage.SaveStage(fd.GetFileName());
		Invalidate();
	}
}

void CJangkiView::OnFileNew() 
{
	m_cStage.Initialize();
	m_cStage.sm_cHistoryStack.ClearStack();
	Invalidate();
}