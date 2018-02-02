// triangulationDlg.h : header file
//

#pragma once

#include <util/common/gui/SimulationDialog.h>
#include <util/common/plot/PlotStatic.h>

// CTriangulationDlg dialog
class CTriangulationDlg : public CSimulationDialog
{
// Construction
public:
    CTriangulationDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    enum { IDD = IDD_TRIANGULATION_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    PlotStatic mPlotCtrl;
};
