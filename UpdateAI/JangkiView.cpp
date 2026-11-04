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
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_GAME_HUMAN_HUMAN, OnModeHumanHuman)
    ON_COMMAND(ID_GAME_HUMAN_AI, OnModeHumanAI)
    ON_COMMAND(ID_GAME_AI_AI, OnModeAIAI)
    ON_COMMAND(ID_GAME_PAUSE, OnAIPause)
    ON_COMMAND(ID_GAME_STEP, OnAIStep)
    ON_COMMAND(ID_GAME_SPEED_FAST, OnAISpeedFast)
    ON_COMMAND(ID_GAME_SPEED_NORMAL, OnAISpeedNormal)
    ON_COMMAND(ID_GAME_SPEED_SLOW, OnAISpeedSlow)
    ON_COMMAND(ID_GAME_STRENGTH_CHO, OnStrengthCho)
    ON_COMMAND(ID_GAME_STRENGTH_HAN, OnStrengthHan)
    ON_COMMAND(ID_GAME_STRENGTH_EQUAL, OnStrengthEqual)
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
    m_gameMode = MODE_HUMAN_AI;
    m_choDepth = 4; m_hanDepth = 2;
    m_choRandomness = 0; m_hanRandomness = 5000;
    m_currentTeam = TM_CHO;
    m_humanTeam = TM_CHO;
    m_paused = false;
    m_gameOver = false;
    m_aiDelay = 1000;
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

	CBrush CHOBrush(RGB(0x0,0xFF,0x0));
	CBrush HANBrush(RGB(0xFF,0x0,0x0));
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
				//	pDC->DrawText("䠂",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u695A";  // 楚

					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("士",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u58EB";  // 士

					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("卒",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u5352"; 

					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("包",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u5305";  

					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("車",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u8ECA";  

					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("馬",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u99AC";  
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("象",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u8C61";
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("漢",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u6F22";
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("士",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u58EB";
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("卒",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u5175";
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("包",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u5305";
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("車",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u8ECA";
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("馬",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u99AC"; 
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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
					//pDC->DrawText("象",rect,DT_VCENTER | DT_CENTER | DT_SINGLELINE);
					CStringW str = L"\u8C61";
					::DrawTextW(
						pDC->GetSafeHdc(),
						str.GetString(),
						-1,
						&rect,
						DT_VCENTER | DT_CENTER | DT_SINGLELINE
					);
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

    // Draw a complete, clean frame off-screen to prevent old move trails.
    CRect client;
    GetClientRect(&client);
    if (client.Width() <= 0 || client.Height() <= 0) return;

    CDC memoryDC;
    if (!memoryDC.CreateCompatibleDC(pDC)) return;
    CBitmap bitmap;
    if (!bitmap.CreateCompatibleBitmap(pDC, client.Width(), client.Height())) return;
    CBitmap* oldBitmap = memoryDC.SelectObject(&bitmap);
    memoryDC.FillSolidRect(0, 0, client.Width(), client.Height(), RGB(255,255,255));
    DrawPanel(&memoryDC);
    pDC->BitBlt(0, 0, client.Width(), client.Height(), &memoryDC, 0, 0, SRCCOPY);
    memoryDC.SelectObject(oldBitmap);
}

BOOL CJangkiView::OnEraseBkgnd(CDC* pDC)
{
    // OnDraw paints every pixel using the off-screen buffer.
    return TRUE;
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
    if (m_gameOver || IsAITurn()) { CView::OnLButtonDown(nFlags, point); return; }
    int nX = (point.x - DF_LEFTUP_MARGIN + DF_SMALL_RECT_SIDE / 2) / DF_SMALL_RECT_SIDE;
    int nY = (point.y - DF_LEFTUP_MARGIN + DF_SMALL_RECT_SIDE / 2) / DF_SMALL_RECT_SIDE;
    if (nX < 0 || nX >= DF_PANEL_WIDTH || nY < 0 || nY >= DF_PANEL_HEIGHT) return;
    if (m_enGameState == GS_HUMANSELECT)
    {
        if (m_cStage.GetTeamAt(nX,nY) == m_currentTeam &&
            m_cStage.GetNextPieceLoc(nX,nY,m_cMD) && m_cMD.GetIndexCount() > 0)
        {
            m_enGameState = GS_HUMANMOVE;
            Invalidate(FALSE);
        }
    }
    else if (m_enGameState == GS_HUMANMOVE)
    {
        for (int i = 0; i < m_cMD.GetIndexCount(); ++i)
        {
            if (nX == m_cMD.GetToX(i) && nY == m_cMD.GetToY(i))
            {
                MOVERESULT result = m_cStage.HumanMove(m_cMD.GetFromX(),m_cMD.GetFromY(),nX,nY);
                m_cMD.Reset();
                m_enGameState = GS_HUMANSELECT;
                FinishMove(result);
                CView::OnLButtonDown(nFlags, point);
                return;
            }
        }
        m_cMD.Reset();
        m_enGameState = GS_HUMANSELECT;
        Invalidate(FALSE);
    }
    CView::OnLButtonDown(nFlags, point);
}

bool CJangkiView::IsAITurn() const
{
    return m_gameMode == MODE_AI_AI ||
        (m_gameMode == MODE_HUMAN_AI && m_currentTeam != m_humanTeam);
}

void CJangkiView::ScheduleAI()
{
    KillTimer(AI_TIMER_ID);
    if (!m_gameOver && !m_paused && IsAITurn()) SetTimer(AI_TIMER_ID, m_aiDelay, NULL);
}

void CJangkiView::ResetGame(GAMEMODE mode)
{
    KillTimer(AI_TIMER_ID);
    m_cStage.Initialize();
    m_cStage.RandomizeOpening();
    CJKStage::sm_cHistoryStack.ClearStack();
    m_cMD.Reset();
    m_enGameState = GS_HUMANSELECT;
    m_gameMode = mode;
    m_currentTeam = TM_CHO;
    m_humanTeam = TM_CHO;
    m_paused = false;
    m_gameOver = false;
    m_bInferAfterDraw = false;
    Invalidate(FALSE);
    ScheduleAI();
}

void CJangkiView::FinishMove(MOVERESULT result)
{
    if (result == MR_HANDEAD || result == MR_CHODEAD || result == MR_UNMOVED)
    {
        m_gameOver = true;
        KillTimer(AI_TIMER_ID);
        Invalidate(FALSE);
        if (result == MR_HANDEAD) AfxMessageBox(_T("Cho wins!"));
        else if (result == MR_CHODEAD) AfxMessageBox(_T("Han wins!"));
        else AfxMessageBox(_T("No available move. Game stopped."));
        return;
    }
    m_currentTeam = (m_currentTeam == TM_CHO) ? TM_HAN : TM_CHO;
    Invalidate(FALSE);
    ScheduleAI();
}

void CJangkiView::ExecuteAITurn()
{
    if (m_gameOver || !IsAITurn()) return;
    MOVERESULT result = m_cStage.Infer(m_currentTeam,
        m_currentTeam == TM_CHO ? m_choDepth : m_hanDepth,
        m_currentTeam == TM_CHO ? m_choRandomness : m_hanRandomness);
    FinishMove(result);
}

void CJangkiView::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == AI_TIMER_ID)
    {
        KillTimer(AI_TIMER_ID);
        if (!m_paused) ExecuteAITurn();
        return;
    }
    CView::OnTimer(nIDEvent);
}

void CJangkiView::OnModeHumanHuman() { ResetGame(MODE_HUMAN_HUMAN); }
void CJangkiView::OnModeHumanAI() { ResetGame(MODE_HUMAN_AI); }
void CJangkiView::OnModeAIAI() { ResetGame(MODE_AI_AI); }
void CJangkiView::OnAIPause()
{
    m_paused = !m_paused;
    if (m_paused) KillTimer(AI_TIMER_ID);
    else ScheduleAI();
}
void CJangkiView::OnAIStep()
{
    if (!IsAITurn() || m_gameOver) return;
    m_paused = true;
    KillTimer(AI_TIMER_ID);
    ExecuteAITurn();
}
void CJangkiView::OnAISpeedFast() { m_aiDelay = 500; ScheduleAI(); }
void CJangkiView::OnAISpeedNormal() { m_aiDelay = 1000; ScheduleAI(); }
void CJangkiView::OnAISpeedSlow() { m_aiDelay = 2000; ScheduleAI(); }

void CJangkiView::OnBackmove()
{
    KillTimer(AI_TIMER_ID);
    int count = (m_gameMode == MODE_HUMAN_AI) ? 2 : 1;
    bool moved = false;
    for (int i = 0; i < count; ++i)
    {
        if (!m_cStage.MoveBack()) break;
        m_currentTeam = (m_currentTeam == TM_CHO) ? TM_HAN : TM_CHO;
        moved = true;
    }
    if (!moved) { AfxMessageBox(_T("Nothing to undo.")); return; }
    m_gameOver = false;
    m_paused = true;
    m_cMD.Reset();
    m_enGameState = GS_HUMANSELECT;
    Invalidate(FALSE);
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
    ResetGame(m_gameMode);
}
// Select different AI strengths. Takes effect on the next AI move.
void CJangkiView::OnStrengthCho()
{
    m_choDepth = 5; m_hanDepth = 3;
    m_choRandomness = 1000; m_hanRandomness = 5000;
}
void CJangkiView::OnStrengthHan()
{
    m_choDepth = 3; m_hanDepth = 5;
    m_choRandomness = 5000; m_hanRandomness = 1000;
}
void CJangkiView::OnStrengthEqual()
{
    m_choDepth = 5; m_hanDepth = 5;
    m_choRandomness = 1500; m_hanRandomness = 1500;
}
