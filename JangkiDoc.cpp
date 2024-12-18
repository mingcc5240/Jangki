// JangkiDoc.cpp : implementation of the CJangkiDoc class
//

#include "stdafx.h"
#include "Jangki.h"

#include "JangkiDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJangkiDoc

IMPLEMENT_DYNCREATE(CJangkiDoc, CDocument)

BEGIN_MESSAGE_MAP(CJangkiDoc, CDocument)
	//{{AFX_MSG_MAP(CJangkiDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJangkiDoc construction/destruction

CJangkiDoc::CJangkiDoc()
{
	// TODO: add one-time construction code here

}

CJangkiDoc::~CJangkiDoc()
{
}

BOOL CJangkiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CJangkiDoc serialization

void CJangkiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CJangkiDoc diagnostics

#ifdef _DEBUG
void CJangkiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CJangkiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJangkiDoc commands
