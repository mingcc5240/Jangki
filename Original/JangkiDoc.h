// JangkiDoc.h : interface of the CJangkiDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_JANGKIDOC_H__530CA6EB_6E30_11D3_B509_BEB3B0969997__INCLUDED_)
#define AFX_JANGKIDOC_H__530CA6EB_6E30_11D3_B509_BEB3B0969997__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CJangkiDoc : public CDocument
{
protected: // create from serialization only
	CJangkiDoc();
	DECLARE_DYNCREATE(CJangkiDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJangkiDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJangkiDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CJangkiDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JANGKIDOC_H__530CA6EB_6E30_11D3_B509_BEB3B0969997__INCLUDED_)
