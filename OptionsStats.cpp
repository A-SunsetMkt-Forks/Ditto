// OptionsStats.cpp : implementation file
//

#include "stdafx.h"
#include "cp_main.h"
#include "OptionsStats.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsStats dialog

IMPLEMENT_DYNCREATE(COptionsStats, CPropertyPage)

COptionsStats::COptionsStats()
	: CPropertyPage(COptionsStats::IDD)
{
	//{{AFX_DATA_INIT(COptionsStats)
	m_eAllCopies = _T("");
	m_eAllPastes = _T("");
	m_eAllDate = _T("");
	m_eTripCopies = _T("");
	m_eTripDate = _T("");
	m_eTripPastes = _T("");
	m_eSavedCopies = _T("");
	m_eSavedCopyData = _T("");
	m_eDatabaseSize = _T("");
	//}}AFX_DATA_INIT
}


void COptionsStats::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsStats)
	DDX_Text(pDX, IDC_ALL_COPIES, m_eAllCopies);
	DDX_Text(pDX, IDC_ALL_PASTES, m_eAllPastes);
	DDX_Text(pDX, IDC_EDIT_ALL_DATE, m_eAllDate);
	DDX_Text(pDX, IDC_TRIP_COPIES, m_eTripCopies);
	DDX_Text(pDX, IDC_TRIP_DATE, m_eTripDate);
	DDX_Text(pDX, IDC_TRIP_PASTES, m_eTripPastes);
	DDX_Text(pDX, IDC_SAVED_COPIES, m_eSavedCopies);
	DDX_Text(pDX, IDC_SAVED_COPY_DATA, m_eSavedCopyData);
	DDX_Text(pDX, IDC_DATABASE_SIZE, m_eDatabaseSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsStats, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsStats)
	ON_BN_CLICKED(IDC_RESET_COUNTS, OnResetCounts)
	ON_BN_CLICKED(IDC_REMOVE_ALL, OnRemoveAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsStats message handlers

BOOL COptionsStats::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CTime time(CGetSetOptions::GetTotalDate());
	m_eAllDate = time.Format("%m/%d/%Y %I:%M %p");
	m_eAllCopies.Format("%d", CGetSetOptions::GetTotalCopyCount());
	m_eAllPastes.Format("%d", CGetSetOptions::GetTotalPasteCount());

	CTime time2(CGetSetOptions::GetTripDate());
	m_eTripDate = time2.Format("%m/%d/%Y %I:%M %p");
	m_eTripCopies.Format("%d", CGetSetOptions::GetTripCopyCount());
	m_eTripPastes.Format("%d", CGetSetOptions::GetTripPasteCount());

	CMainTable recset;
	recset.Open("SELECT * FROM Main");
	if(!recset.IsEOF())
	{
		recset.MoveLast();
		m_eSavedCopies.Format("%d", recset.GetRecordCount());
	}

	CDataTable recset2;
	recset2.Open(AFX_DAO_USE_DEFAULT_TYPE, "SELECT * FROM Data" ,NULL);
	if(!recset2.IsEOF())
	{
		recset2.MoveLast();
		m_eSavedCopyData.Format("%d", recset2.GetRecordCount());
	}

	struct _stat buf;
	int nResult;
	nResult = _stat(GetDBName(), &buf);
	if(nResult == 0)
		m_eDatabaseSize.Format("%d KB", (buf.st_size/1024));

	UpdateData(FALSE);
		
	return TRUE;
}

void COptionsStats::OnResetCounts() 
{
	CGetSetOptions::SetTripCopyCount(0);
	CGetSetOptions::SetTripPasteCount(0);
	CGetSetOptions::SetTripDate(0);

	m_eTripDate.Empty();
	m_eTripCopies.Empty();
	m_eTripPastes.Empty();

	UpdateData(FALSE);
}

void COptionsStats::OnRemoveAll() 
{
	if(MessageBox("This will remove all Copy Entries!\n\nContinue?", "Warning", MB_YESNO) == IDYES)
	{
		CMainTable MainTable;
		if(MainTable.DeleteAll())
		{
			CDataTable DataTable;
			if(DataTable.DeleteAll())
			{
				if(CompactDatabase())
				{
					RepairDatabase();

					m_eSavedCopies.Empty();
					m_eSavedCopyData.Empty();

					struct _stat buf;
					int nResult;
					nResult = _stat(GetDBName(), &buf);
					if(nResult == 0)
						m_eDatabaseSize.Format("%d KB", (buf.st_size/1024));

					UpdateData(FALSE);
				}
			}
		}
	}
}