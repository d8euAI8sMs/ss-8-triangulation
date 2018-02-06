// triangulationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "triangulation.h"
#include "triangulationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTriangulationDlg dialog

static double angle_between(const geom::point2d_t & p0,
                            const geom::point2d_t & p1,
                            const geom::point2d_t & p2)
{
    auto v1 = p1 - p0;
    auto v2 = p2 - p0;
    return std::acos((v1.x * v2.x + v1.y * v2.y) / math::norm(v1) / math::norm(v2));
}

enum mesh_type : size_t
{
    mesh_type_random,
    mesh_type_random_rect,
    mesh_type_rect,
    mesh_type_square,
    mesh_type_hex,
    mesh_type_hex_in_hex
};

static const LPCTSTR mesh_type_names[] =
{
    _T("Random Mesh"),
    _T("Random Mesh (Nearly Rectangular)"),
    _T("Rectangular Mesh"),
    _T("Square"),
    _T("Hexagon"),
    _T("Hexagon with Hexagonal Superstructure")
};

CTriangulationDlg::CTriangulationDlg(CWnd* pParent /*=NULL*/)
    : CSimulationDialog(CTriangulationDlg::IDD, pParent)
    , mSelectedMesh(0)
    , mbPaintSuperstructure(TRUE)
    , mbPaintDirichletCells(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTriangulationDlg::DoDataExchange(CDataExchange* pDX)
{
    CSimulationDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PLOT, mPlotCtrl);
    DDX_Control(pDX, IDC_COMBO1, mMeshSelectCtrl);
    DDX_CBIndex(pDX, IDC_COMBO1, mSelectedMesh);
    DDX_Check(pDX, IDC_CHECK3, mbPaintSuperstructure);
    DDX_Check(pDX, IDC_CHECK2, mbPaintDirichletCells);
    DDX_Control(pDX, IDC_Comment, mComment);
}

BEGIN_MESSAGE_MAP(CTriangulationDlg, CSimulationDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CTriangulationDlg::OnBnClickedButton1)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CTriangulationDlg::OnCbnSelchangeCombo1)
    ON_BN_CLICKED(IDC_CHECK2, &CTriangulationDlg::OnBnClickedCheck2)
    ON_BN_CLICKED(IDC_CHECK3, &CTriangulationDlg::OnBnClickedCheck3)
END_MESSAGE_MAP()


// CTriangulationDlg message handlers

BOOL CTriangulationDlg::OnInitDialog()
{
    CSimulationDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

    mMesh = util::create < geom::mesh > ();

    mWorld = plot::world_t::create(-11, 11, -11, 11);

    mPlot = plot::triangulation_drawable::create
    (
        plot::make_data_source(mMesh),
        plot::custom_drawable::create
        (
            plot::circle_painter(5, plot::palette::brush(RGB(200, 200, 200)))
        ),
        plot::palette::pen(RGB(180, 180, 180))
    );

    mPlotDirichlet = plot::dirichlet_cell_drawable::create
    (
        plot::make_data_source(mMesh),
        nullptr,
        plot::palette::pen(RGB(255, 100, 100), 2)
    );

    mPlot->superstructure_visible = (mbPaintSuperstructure != 0);
    mpPaintSuperstructure = &mPlot->superstructure_visible;

    mPlotDirichlet->visible = (mbPaintDirichletCells != 0);
    mpPaintDirichletCells = &mPlotDirichlet->visible;

    auto root = plot::viewporter::create(
        plot::tick_drawable::create(
            plot::layer_drawable::create(
                std::vector < plot::drawable::ptr_t > (
                    { mPlot, mPlotDirichlet }
                )
            ),
            plot::const_n_tick_factory<plot::axe::x>::create(
                plot::make_simple_tick_formatter(2, 5),
                0,
                5
            ),
            plot::const_n_tick_factory<plot::axe::y>::create(
                plot::make_simple_tick_formatter(2, 5),
                0,
                5
            ),
            plot::palette::pen(RGB(80, 80, 80)),
            RGB(200, 200, 200)
        ),
        plot::make_viewport_mapper(mWorld)
    );

    mPlotCtrl.plot_layer.with(root);

    mPlotCtrl.background = plot::palette::brush();
    mPlotCtrl.triple_buffered = true;

    mPlotCtrl.RedrawBuffer();
    mPlotCtrl.SwapBuffers();
    mPlotCtrl.RedrawWindow();

    mMeshSelectCtrl.AddString(mesh_type_names[mesh_type_random]);
    mMeshSelectCtrl.AddString(mesh_type_names[mesh_type_random_rect]);
    mMeshSelectCtrl.AddString(mesh_type_names[mesh_type_rect]);
    mMeshSelectCtrl.AddString(mesh_type_names[mesh_type_square]);
    mMeshSelectCtrl.AddString(mesh_type_names[mesh_type_hex]);
    mMeshSelectCtrl.AddString(mesh_type_names[mesh_type_hex_in_hex]);

    mMeshSelectCtrl.SetCurSel(0);

    OnBnClickedButton1();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTriangulationDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CSimulationDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTriangulationDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CTriangulationDlg::OnBnClickedButton1()
{
    UpdateData(TRUE);

    StartSimulationThread();
}


void CTriangulationDlg::OnSimulation()
{
    mComment.SetWindowText(_T(""));

    srand(clock() % UINT_MAX);

    /* clear existing mesh */
    mMesh->clear();

    std::vector < plot::point < double > > superstruct;
    std::vector < plot::point < double > > points;

    switch (mSelectedMesh)
    {
    case mesh_type_hex_in_hex:
        superstruct.emplace_back(-1 * 6, 0);
        superstruct.emplace_back(-0.5 * 6, std::sqrt(3) / 2 * 6);
        superstruct.emplace_back(0.5 * 6, std::sqrt(3) / 2 * 6);
        superstruct.emplace_back(1 * 6, 0);
        superstruct.emplace_back(0.5 * 6, - std::sqrt(3) / 2 * 6);
        superstruct.emplace_back(-0.5 * 6, - std::sqrt(3) / 2 * 6);
        break;
    default:
        superstruct.emplace_back(-10, -10);
        superstruct.emplace_back(10, 10);
        superstruct.emplace_back(10, -10);
        superstruct.emplace_back(-10, 10);
        break;
    }

    switch (mSelectedMesh)
    {
    case mesh_type_random:
        for (size_t i = 0; i < 100; ++i)
        {
            points.emplace_back((double)rand() / RAND_MAX * 16 - 8,
                                (double)rand() / RAND_MAX * 16 - 8);
        }
        break;
    case mesh_type_random_rect:
        for (size_t i = 0; i < 100; ++i)
        {
            double r1 = ((double)rand() / RAND_MAX * 0.2 - 0.1) * i / 100.;
            double r2 = ((double)rand() / RAND_MAX * 0.2 - 0.1) * i / 100.;
            points.emplace_back((i / 10 + r1) - 4.5, (i % 10 + r2) - 4.5);
        }
        break;
    case mesh_type_rect:
        for (size_t i = 0; i < 100; ++i)
        {
            points.emplace_back(i / 10 - 4.5, i % 10 - 4.5);
        }
        break;
    case mesh_type_square:
        points.emplace_back(-5, -5);
        points.emplace_back(5, 5);
        points.emplace_back(5, -5);
        points.emplace_back(-5, 5);
        break;
    case mesh_type_hex:
    case mesh_type_hex_in_hex:
        points.emplace_back(-1 * 3, 0);
        points.emplace_back(-0.5 * 3, std::sqrt(3) / 2 * 3);
        points.emplace_back(0.5 * 3, std::sqrt(3) / 2 * 3);
        points.emplace_back(1 * 3, 0);
        points.emplace_back(0.5 * 3, - std::sqrt(3) / 2 * 3);
        points.emplace_back(-0.5 * 3, - std::sqrt(3) / 2 * 3);
        break;
    default:
        break;
    }

    mMesh->init(superstruct);
    mMesh->add(points);
    mMesh->finish_mesh();

    mPlotCtrl.RedrawBuffer();
    mPlotCtrl.SwapBuffers();
    mPlotCtrl.RedrawWindow();

    const double threshold1_deg = 180 - 180 / 4.;
    const double threshold2_deg = 180 - 180 / 8.;
    const double threshold1 = threshold1_deg / 180 * M_PI;
    const double threshold2 = threshold2_deg / 180 * M_PI;
    size_t count1 = 0;
    size_t count2 = 0;

    for (size_t i = 0; i < mMesh->triangles().size(); ++i)
    {
        if (mMesh->triangles()[i].flags & geom::mesh::phantom) continue;
        auto t = mMesh->triangle_at(i);
        auto a1 = angle_between(t.points[0], t.points[1], t.points[2]);
        auto a2 = angle_between(t.points[1], t.points[0], t.points[2]);
        auto a3 = angle_between(t.points[2], t.points[1], t.points[0]);

        if ((a1 > threshold1) || (a2 > threshold1) || (a3 > threshold1))
        {
            ++count1;
        }
        if ((a1 > threshold2) || (a2 > threshold2) || (a3 > threshold2))
        {
            ++count2;
        }
    }

    CString fmt;
    fmt.Format(_T("total %d points and %d triangles;    ")
        _T("%d triangles have max angle > %.1lf deg;    ")
        _T("%d triangles - > %.1lf deg"),
        mMesh->vertices().size(),
        mMesh->triangles().size(),
        count1,
        threshold1_deg,
        count2,
        threshold2_deg);
    mComment.SetWindowText(fmt);

    CSimulationDialog::OnSimulation();
}


void CTriangulationDlg::OnCbnSelchangeCombo1()
{
    OnBnClickedButton1();
}


void CTriangulationDlg::OnBnClickedCheck2()
{
    UpdateData(TRUE);

    *mpPaintDirichletCells = (mbPaintDirichletCells != 0);

    mPlotCtrl.RedrawBuffer();
    mPlotCtrl.SwapBuffers();
    mPlotCtrl.RedrawWindow();
}


void CTriangulationDlg::OnBnClickedCheck3()
{
    UpdateData(TRUE);

    *mpPaintSuperstructure = (mbPaintSuperstructure != 0);

    mPlotCtrl.RedrawBuffer();
    mPlotCtrl.SwapBuffers();
    mPlotCtrl.RedrawWindow();
}
