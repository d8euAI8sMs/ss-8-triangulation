// triangulationDlg.h : header file
//

#pragma once

#include <util/common/gui/SimulationDialog.h>
#include <util/common/plot/PlotStatic.h>
#include <util/common/ptr.h>

#include "mesh.h"
#include "mesh_drawable.h"
#include "afxwin.h"

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
    util::ptr_t < plot::mesh > mMesh;
    plot::mesh_drawable :: ptr_t mPlot;
    plot::world_t::ptr_t mWorld;
    CComboBox mMeshSelectCtrl;
    afx_msg void OnCbnSelchangeCombo1();
    int mSelectedMesh;
    BOOL mbPaintSuperstructure;
    BOOL mbPaintDirichletCells;
    bool * mpPaintSuperstructure;
    bool * mpPaintDirichletCells;
    afx_msg void OnBnClickedCheck2();
    afx_msg void OnBnClickedCheck3();
};
