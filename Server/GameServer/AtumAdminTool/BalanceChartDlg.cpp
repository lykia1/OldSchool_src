// helloworldDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "..\atumadmintool.h"
#include "AtumAdminToolDlg.h"
#include "BalanceChartDlg.h"
#include "chartdir.h"


CBalanceChartDlg* global = NULL;
/////////////////////////////////////////////////////////////////////////////
// CBalanceChartDlg dialog

CBalanceChartDlg::CBalanceChartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBalanceChartDlg::IDD, pParent)
{
    
    global = this;
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pMainDlg = (CAtumAdminToolDlg*)AfxGetMainWnd();
    m_FromDate = (CTime::GetCurrentTime() - CTimeSpan(60, 0, 0, 0));
    m_ToDate = CTime::GetCurrentTime();
  
}

void CBalanceChartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBalanceChartDlg)
	DDX_Control(pDX, IDC_CHART, m_ChartViewer);
    DDX_Control(pDX, IDC_BUTTON_CHART_RLD, m_RldBtn);
    DDX_Control(pDX, IDC_BUTTON_CHART_RLD2, m_LoadExpBtn);
    DDX_Control(pDX, IDC_BUTTON_CHART_RLD3, m_LoadMarketBtn);
    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1_CHART, m_FromDate);

    DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2_CHART, m_ToDate);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBalanceChartDlg, CDialog)
	//{{AFX_MSG_MAP(CBalanceChartDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_CHART_RLD, &CBalanceChartDlg::OnBnClickedButton1)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1_CHART, &CBalanceChartDlg::OnDtnDatetimechangeDatetimepicker1)
    ON_BN_CLICKED(IDC_BUTTON_CHART_RLD2, &CBalanceChartDlg::OnBnClickedButtonChartRld2)
    ON_BN_CLICKED(IDC_BUTTON_CHART_RLD3, &CBalanceChartDlg::OnBnClickedButtonChartRld3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBalanceChartDlg message handlers
#include <cstddef>
template <typename T, std::size_t N>
constexpr std::size_t size(T(&)[N]) {
    return N;
}

BOOL CBalanceChartDlg::DBConn()
{
    UpdateData();
    vectDateUserCntList.clear();
    if (FALSE == m_odbcStmt2.Init(m_pMainDlg->m_pServerInfo4Admin->DBIP, m_pMainDlg->m_pServerInfo4Admin->DBPort, m_pMainDlg->m_pServerInfo4Admin->DBName,
        m_pMainDlg->m_pServerInfo4Admin->DBUID, m_pMainDlg->m_pServerInfo4Admin->DBPWD, GetSafeHwnd()))
    {
        char szTemp[1024];
        sprintf(szTemp, "Can not connect DBServer<%s(%s:%d)> !!"
            , m_pMainDlg->m_pServerInfo4Admin->DBName, m_pMainDlg->m_pServerInfo4Admin->DBIP
            , m_pMainDlg->m_pServerInfo4Admin->DBPort);
        MessageBox(szTemp);
        EndDialog(-1);
        return FALSE;
    }
    BOOL bRet = FALSE;

    SQLHSTMT m_pODBCStmt1 = m_odbcStmt2.GetSTMTHandle();
  
    bRet = m_odbcStmt2.ExecuteQuery("if object_id('tm_user_connection','v') is not null drop view tm_user_connection");

    if (!bRet)
    {
        m_odbcStmt2.FreeStatement();		// clean up
        MessageBox("ExecuteQuery Error 0");
        return TRUE;
    }

    CString szStartDate, szEndDate;
    CTime tmStartDate;
    CString cs;

    szStartDate.Format("%s", (LPCSTR)m_FromDate.Format("%Y/%m/%d 0:0:0.0"));
    szEndDate.Format("%s", (LPCSTR)m_ToDate.Format("%Y/%m/%d 23:59:59.0"));
   
    char szTemp[1024];

    sprintf(szTemp, "dates chart \r\n %s \r\n %s", szStartDate, szEndDate);
  //  MessageBox(szTemp);
    CString szFinalQuery;
    szFinalQuery.Format("CREATE VIEW tm_user_connection AS \
        SELECT dbo.atum_getonlydate(Time) AS conntedDate, c.AccountUniqueNumber, c.SelectableInfluenceMask \
        FROM dbo.atum_log_user_game_start_end l WITH(NOLOCK) INNER JOIN td_character c ON c.AccountUniqueNumber = l.AccountUniqueNumber \
        WHERE(Race & 0x4000 = 0) AND Time >= '%s' and Time <= '%s' \
        GROUP BY dbo.atum_getonlydate(Time), c.AccountUniqueNumber, c.SelectableInfluenceMask \
        UNION(SELECT dbo.atum_getonlydate(Time) AS conntedDate, c.AccountUniqueNumber, c.SelectableInfluenceMask \
            FROM dbo.atum_backup_log_user_game_start_end l WITH(NOLOCK) INNER JOIN td_character c ON c.AccountUniqueNumber = l.AccountUniqueNumber \
            WHERE(Race & 0x4000 = 0) AND Time >= '%s' and Time <= '%s' \
            GROUP BY dbo.atum_getonlydate(Time), c.AccountUniqueNumber, c.SelectableInfluenceMask) ", szStartDate, szEndDate, szStartDate, szEndDate);


    bRet = m_odbcStmt2.ExecuteQuery((LPSTR)(LPCSTR)szFinalQuery);

    if (!bRet)
    {
        m_odbcStmt2.FreeStatement();		// clean up
      // m_odbcStmt2.Clean();
        MessageBox("ExecuteQuery Error 1");
        return TRUE;
    }
    CString szSQLQuery;
    bRet = m_odbcStmt2.ExecuteQuery("SELECT l.conntedDate, l.SelectableInfluenceMask, COUNT(l.AccountUniqueNumber) FROM tm_user_connection l WITH(NOLOCK) GROUP BY l.conntedDate, l.SelectableInfluenceMask ORDER BY l.conntedDate");

    if (!bRet)
    {
        m_odbcStmt2.FreeStatement();		// clean up
     //   m_odbcStmt2.Clean();
        MessageBox("ExecuteQuery Error 2");
        return TRUE;
    }
    
    SQLLEN arrCB[4] = { SQL_NTS, SQL_NTS, SQL_NTS, SQL_NTS };
    char	tmDate[SIZE_DATE];
    BYTE	byInflMask;
    int		nUserCnts;
    SQLBindCol(m_odbcStmt2.m_hstmt, 1, SQL_C_CHAR, &tmDate, SIZE_DATE, &arrCB[1]);
    SQLBindCol(m_odbcStmt2.m_hstmt, 2, SQL_C_UTINYINT, &byInflMask, 0, &arrCB[2]);
    SQLBindCol(m_odbcStmt2.m_hstmt, 3, SQL_C_LONG, &nUserCnts, 0, &arrCB[3]);

    while ((bRet = SQLFetch(m_odbcStmt2.m_hstmt)) != SQL_NO_DATA)
    {


        vectSDATE_USER_COUNT::iterator itr = find_if(vectDateUserCntList.begin(), vectDateUserCntList.end(), find_if_SDATE_USER_COUNT_BY_Date(tmDate));
        if (itr == vectDateUserCntList.end())
        {
            SDATE_USER_COUNT tmDUserCnt;
            memset(&tmDUserCnt, 0x00, sizeof(SDATE_USER_COUNT));
            strncpy(tmDUserCnt.Date, tmDate, SIZE_DATE);
            switch (byInflMask)
            {
            case INFLUENCE_TYPE_VCN:
            case INFLUENCE_TYPE_ANI:
            {
                tmDUserCnt.arrUserCnts[GetArrayIndexByInfluenceType(byInflMask)] = nUserCnts;
            }
            break;
            default:
            {
                tmDUserCnt.arrUserCnts[0] = nUserCnts;	// 2008-01-14 by cmkwon, 老馆 技仿
            }
            }
            vectDateUserCntList.push_back(tmDUserCnt);
        }
        else
        {
            switch (byInflMask)
            {
            case INFLUENCE_TYPE_VCN:
            case INFLUENCE_TYPE_ANI:
            {
                itr->arrUserCnts[GetArrayIndexByInfluenceType(byInflMask)] = nUserCnts;
            }
            break;
            default:
            {
                itr->arrUserCnts[0] = nUserCnts;	// 2008-01-14 by cmkwon, 老馆 技仿
            }
            }
        }
        memset(tmDate, 0x00, SIZE_DATE);
        byInflMask = 0;
        nUserCnts = 0;
    }
    m_odbcStmt2.FreeStatement();
   // m_odbcStmt2.Clean();
    return FALSE;
}
BOOL CBalanceChartDlg::GetExpChart()
{
    UpdateData();
    vectDateExpList.clear();
    if (FALSE == m_odbcStmt2.Init(m_pMainDlg->m_pServerInfo4Admin->DBIP, m_pMainDlg->m_pServerInfo4Admin->DBPort, m_pMainDlg->m_pServerInfo4Admin->DBName,
        m_pMainDlg->m_pServerInfo4Admin->DBUID, m_pMainDlg->m_pServerInfo4Admin->DBPWD, GetSafeHwnd()))
    {
        char szTemp[1024];
        sprintf(szTemp, "Can not connect DBServer<%s(%s:%d)> !!"
            , m_pMainDlg->m_pServerInfo4Admin->DBName, m_pMainDlg->m_pServerInfo4Admin->DBIP
            , m_pMainDlg->m_pServerInfo4Admin->DBPort);
        MessageBox(szTemp);
        EndDialog(-1);
        return FALSE;
    }
    BOOL bRet = FALSE;

    SQLHSTMT m_pODBCStmt1 = m_odbcStmt2.GetSTMTHandle();

    bRet = m_odbcStmt2.ExecuteQuery("if object_id('tm_user_exp','v') is not null drop view tm_user_exp");

    if (!bRet)
    {
        m_odbcStmt2.FreeStatement();		// clean up
        MessageBox("ExecuteQuery Error 0");
        return TRUE;
    }

    CString szStartDate, szEndDate;
    CTime tmStartDate;
    CString cs;
    CString cs_charuid;

    GetDlgItem(IDC_EDIT1)->GetWindowText(cs_charuid);
    UID32_t CharUID = 10675;

      if(cs_charuid.GetLength() > 2)
        CharUID =  atol(cs_charuid);

    szStartDate.Format("%s", (LPCSTR)m_FromDate.Format("%Y/%m/%d 0:0:0.0"));
    szEndDate.Format("%s", (LPCSTR)m_ToDate.Format("%Y/%m/%d 23:59:59.0"));

    char szTemp[1024];

    sprintf(szTemp, "dates chart \r\n %s \r\n %s", szStartDate, szEndDate);
    //  MessageBox(szTemp);
    CString szFinalQuery;
    szFinalQuery.Format("CREATE VIEW tm_user_exp AS \
        SELECT Time AS conntedDate, c.uniquenumber, c.CharacterName, l.mapindex, l.Param1, l.Param2\
        FROM dbo.atum_log_user_exp l WITH(NOLOCK) INNER JOIN td_character c ON c.uniquenumber = l.CharacterUniqueNumber\
        WHERE(Race & 0x4000 = 0) AND Time >= '%s' and Time <= '%s' and CharacterUniqueNumber = %d\
        GROUP BY Time, c.uniquenumber, c.CharacterName, l.mapindex, l.Param1, l.Param2\
        UNION(SELECT Time AS conntedDate, c.uniquenumber, c.CharacterName, l.mapindex, l.Param1, l.Param2\
            FROM dbo.atum_log_user_exp l WITH(NOLOCK) INNER JOIN td_character c ON c.uniquenumber = l.CharacterUniqueNumber\
            WHERE(Race & 0x4000 = 0) AND Time >= '%s' and Time <= '%s'  and CharacterUniqueNumber = %d\
            GROUP BY Time, c.uniquenumber, c.CharacterName, l.mapindex, l.Param1, l.Param2) ", szStartDate, szEndDate, CharUID, szStartDate, szEndDate, CharUID);


    bRet = m_odbcStmt2.ExecuteQuery((LPSTR)(LPCSTR)szFinalQuery);

    if (!bRet)
    {
        m_odbcStmt2.FreeStatement();		// clean up
        // m_odbcStmt2.Clean();
        MessageBox("ExecuteQuery Error 1");
        return TRUE;
    }
    CString szSQLQuery;
    bRet = m_odbcStmt2.ExecuteQuery("select conntedDate,CharacterName,Param2,Param1\
      /*  from(select*, row_number() over(partition by convert(date, conntedDate) order by(select null)) rn from tm_user_exp) tt\
    where tt.rn = 1*/\
\
from tm_user_exp\
        GROUP BY conntedDate, CharacterName, Param2,Param1\
        ORDER BY conntedDate");

    if (!bRet)
    {
        m_odbcStmt2.FreeStatement();		// clean up
        //   m_odbcStmt2.Clean();
        MessageBox("ExecuteQuery Error 2");
        return TRUE;
    }

    SQLLEN arrCB[5] = { SQL_NTS, SQL_NTS, SQL_NTS, SQL_NTS , SQL_NTS };
    char	tmDate[SIZE_DATE];
    char	szCharName[20];
    long long 		nNowExp;
    float		fChange;
    SQLBindCol(m_odbcStmt2.m_hstmt, 1, SQL_C_CHAR, &tmDate, SIZE_DATE, &arrCB[1]);
    SQLBindCol(m_odbcStmt2.m_hstmt, 2, SQL_C_CHAR, &szCharName, 20, &arrCB[2]);
    SQLBindCol(m_odbcStmt2.m_hstmt, 3, SQL_C_UBIGINT, &nNowExp, 0, &arrCB[3]);
    SQLBindCol(m_odbcStmt2.m_hstmt, 4, SQL_C_FLOAT, &fChange, 0, &arrCB[4]);

    while ((bRet = SQLFetch(m_odbcStmt2.m_hstmt)) != SQL_NO_DATA)
    {
        SDATE_EXP tmDUserCnt;
        memset(&tmDUserCnt, 0x00, sizeof(SDATE_EXP));
        strncpy(tmDUserCnt.Date, tmDate, SIZE_DATE);
        tmDUserCnt.EXP = nNowExp;
        tmDUserCnt.fChangeVal = fChange;


        vectDateExpList.push_back(tmDUserCnt);      
    }
    m_odbcStmt2.FreeStatement();
    // m_odbcStmt2.Clean();
    return FALSE;
}
BOOL CBalanceChartDlg::DrawGraph()
{
    const char* labels[1024];
    double data0[1024];
    double data1[1024];
    double data2[1024];
    char szTemp[1024];
   
    int nSize = vectDateUserCntList.size();
    sprintf(szTemp, "Update chart with %d size data", nSize);
  //  MessageBox(szTemp);
    int i = 0;
    for (auto& el : vectDateUserCntList) {
        labels[i] = el.Date;
        data2[i] = /*pDateUserCnt->arrUserCnts[0] +*/(double)(el.arrUserCnts[1] + el.arrUserCnts[2]) * 1.0f; //total
        data0[i] = el.arrUserCnts[1] * 1.0f;
        data1[i] = el.arrUserCnts[2] * 1.0f;
        i++;
    }

    const int labels_size = nSize;
    const int data0_size = nSize;
    const int data1_size = nSize;
    const int data2_size = nSize;
    // Create a XYChart object of size 450 x 450 pixels
    LONG x, y;
    int nChartSizeX = 750;
    int nChartSizeY = 450;
    int nMainWndSizeX = nChartSizeX + 230;
    int nMainWndSizeY = nChartSizeY + 30;
    x = LONG((GetSystemMetrics(SM_CXSCREEN) - nMainWndSizeX) / 2);
    y = LONG((GetSystemMetrics(SM_CYSCREEN) - nMainWndSizeY) / 2);
    
    XYChart* c = new XYChart(nChartSizeX, nChartSizeY);


    MoveWindow(x, y, nMainWndSizeX, nMainWndSizeY);
    GetDlgItem(IDC_CHART)->MoveWindow(nMainWndSizeX - nChartSizeX - 30, nMainWndSizeY - nChartSizeY - 30, nChartSizeX, nChartSizeY);
    c->setBackground(0x202124);
    // Add a title to the chart using 15pt Arial Italic font.
    TextBox* title = c->addTitle("Players total and nation balance", "Arial Italic", 15, 0x9c9c9c);

    // Add a legend box where the top-center is anchored to the horizontal center of the chart, just
    // under the title. Use horizontal layout and 10 points Arial Bold font, and transparent
    // background and border. Use line style legend key.
    LegendBox* legendBox = c->addLegend(c->getWidth() / 2, title->getHeight(), false,
        "Arial Bold Italic", 10);
    legendBox->setBackground(Chart::Transparent, Chart::Transparent);
    legendBox->setAlignment(Chart::TopCenter);
    legendBox->setLineStyleKey();
    legendBox->setFontColor(0x525354);
    // Tentatively set the plotarea at (70, 65) and of (chart_width - 100) x (chart_height - 110) in
    // size. Use light grey (c0c0c0) border and horizontal and vertical grid lines.
    PlotArea* plotArea = c->setPlotArea(70, 65, c->getWidth() - 100, c->getHeight() - 110, -1, -1,
        0xc0c0c0, 0x525354, -1);

    // Add a title to the y axis using 12pt Arial Bold Italic font
    c->yAxis()->setTitle("Player amount", "Arial Bold Italic", 12, 0x9c9c9c);
    c->yAxis()->setColors(0x525354, 0x525354, 0x9c9c9c);
    // Add a title to the x axis using 12pt Arial Bold Italic font
    c->xAxis()->setTitle("Date", "Arial Bold Italic", 12, 0x9c9c9c);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, labels_size));

    // Use 8 points Arial rotated by 90 degrees as the x-axis label font
    c->xAxis()->setLabelStyle("Arial", 8, 0x525354, 90);

    // Add a spline curve to the chart
    SplineLayer* layer0 = c->addSplineLayer(DoubleArray(data0, data0_size), 0xff8500, "BCU");
    layer0->setLineWidth(2);

    // Add a normal line to the chart
    LineLayer* layer1 = c->addLineLayer(DoubleArray(data1, data1_size), 0x00FFFF, "ANI");
    layer1->setLineWidth(2);

    // Color the region between the above spline curve and normal line. Use the semi-transparent red
    // (80ff000000) if the spline curve is higher than the normal line, otherwise use
    // semi-transparent green (80008800)
    c->addInterLineLayer(layer0->getLine(), layer1->getLine(), 0x80ff0000, 0x80008800);

    // Add another normal line to the chart
    LineLayer* layer2 = c->addLineLayer(DoubleArray(data2, data2_size), 0x11408c, "TOTAL");
    layer2->setLineWidth(2);

    // Add a horizontal mark line to the chart at y = 40
   /* Mark* mark = c->yAxis()->addMark(40, -1, "Total count");
    mark->setLineWidth(2);

    // Set the mark line to purple (880088) dash line. Use white (ffffff) for the mark label.
    mark->setMarkColor(c->dashLineColor(0x880088), 0xffffff);

    // Put the mark label at the left side of the mark, with a purple (880088) background.
    mark->setAlignment(Chart::Left);
    mark->setBackground(0x880088);

    // Color the region between the above normal line and mark line. Use the semi-transparent blue
    // (800000ff) if the normal line is higher than the mark line, otherwise use semi-transparent
    // purple (80880088)
    c->addInterLineLayer(layer2->getLine(), mark->getLine(), 0x800000ff, 0x80880088);
    */
    // Layout the legend box, so we can get its height
    c->layoutLegend();

    // Adjust the plot area size, such that the bounding box (inclusive of axes) is 10 pixels from
    // the left edge, just under the legend box, 25 pixels from the right edge, and 10 pixels from
    // the bottom edge.
    c->packPlotArea(10, legendBox->getTopY() + legendBox->getHeight(), c->getWidth() - 25,
        c->getHeight() - 10);

    // After determining the exact plot area position, we may adjust the legend box and the title
    // positions so that they are centered relative to the plot area (instead of the chart)
    legendBox->setPos(plotArea->getLeftX() + (plotArea->getWidth() - legendBox->getWidth()) / 2,
        legendBox->getTopY());
    title->setPos(plotArea->getLeftX() + (plotArea->getWidth() - title->getWidth()) / 2,
        title->getTopY());

    // Output the chart
    m_ChartViewer.setChart(c);
    m_ChartViewer.setUpdateInterval(20);
    //include tool tip for the chart
    m_ChartViewer.setImageMap(c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} in {xLabel}: {value}'"));
    // In this sample project, we do not need to chart object any more, so we 
    // delete it now.
    delete c;
  
    return TRUE;
}
BOOL CBalanceChartDlg::DrawGraph2()
{
    const char* labels[1024];
    double data0[1024];
    double data1[1024];
    double data2[1024];
    char szTemp[1024];

    int nSize = vectDateExpList.size();
    sprintf(szTemp, "Update chart with %d size data", nSize);
    //  MessageBox(szTemp);
    int i = 0;

    for (auto& el : vectDateExpList) {
        labels[i] = el.Date;
      //  data2[i] = /*pDateUserCnt->arrUserCnts[0] +*/(double)(el.arrUserCnts[1] + el.arrUserCnts[2]) * 1.0f; //total
        data2[i] = el.fChangeVal;
        //data1[i] = el.EXP;
        data0[i] = el.EXP;
       // data1[i] = el.arrUserCnts[2] * 1.0f;
        i++;
    }

    const int labels_size = nSize;
    const int data0_size = nSize;
    const int data1_size = nSize;
    const int data2_size = nSize;
    // Create a XYChart object of size 450 x 450 pixels
    LONG x, y;
    int nChartSizeX = 750;
    int nChartSizeY = 450;
    int nMainWndSizeX = nChartSizeX + 230;
    int nMainWndSizeY = nChartSizeY + 30;
    x = LONG((GetSystemMetrics(SM_CXSCREEN) - nMainWndSizeX) / 2);
    y = LONG((GetSystemMetrics(SM_CYSCREEN) - nMainWndSizeY) / 2);

    XYChart* c = new XYChart(nChartSizeX, nChartSizeY);

    MoveWindow(x, y, nMainWndSizeX, nMainWndSizeY);
    GetDlgItem(IDC_CHART)->MoveWindow(nMainWndSizeX - nChartSizeX - 30, nMainWndSizeY - nChartSizeY - 30, nChartSizeX, nChartSizeY);
    c->setBackground(0x202124);
    // Add a title to the chart using 15pt Arial Italic font.
    TextBox* title = c->addTitle("Player exp changes", "Arial Italic", 15, 0x9c9c9c);

    // Add a legend box where the top-center is anchored to the horizontal center of the chart, just
    // under the title. Use horizontal layout and 10 points Arial Bold font, and transparent
    // background and border. Use line style legend key.
    LegendBox* legendBox = c->addLegend(c->getWidth() / 2, title->getHeight(), false,
        "Arial Bold Italic", 10);
    legendBox->setBackground(Chart::Transparent, Chart::Transparent);
    legendBox->setAlignment(Chart::TopCenter);
    legendBox->setLineStyleKey();
    legendBox->setFontColor(0x525354);
    // Tentatively set the plotarea at (70, 65) and of (chart_width - 100) x (chart_height - 110) in
    // size. Use light grey (c0c0c0) border and horizontal and vertical grid lines.
    PlotArea* plotArea = c->setPlotArea(70, 65, c->getWidth() - 100, c->getHeight() - 110, -1, -1,
        0xc0c0c0, 0x525354, -1);

    // Add a title to the y axis using 12pt Arial Bold Italic font
    c->yAxis()->setTitle("EXP points", "Arial Bold Italic", 12, 0x9c9c9c);
    c->yAxis()->setColors(0x525354, 0x525354, 0x9c9c9c);
    // Add a title to the x axis using 12pt Arial Bold Italic font
    c->xAxis()->setTitle("Date / map", "Arial Bold Italic", 12, 0x9c9c9c);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);

    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, labels_size));

    // Use 8 points Arial rotated by 90 degrees as the x-axis label font
    c->xAxis()->setLabelStyle("Arial", 8, 0x525354, 90);

    // Add a spline curve to the chart
    SplineLayer* layer0 = c->addSplineLayer(DoubleArray(data0, data0_size), 0xff8500, "TOTAL");
    layer0->setLineWidth(2);

    // Add a normal line to the chart
 //   LineLayer* layer1 = c->addLineLayer(DoubleArray(data1, data1_size), 0x00FFFF, "TOTAL");
   // layer1->setLineWidth(2);

    // Color the region between the above spline curve and normal line. Use the semi-transparent red
    // (80ff000000) if the spline curve is higher than the normal line, otherwise use
    // semi-transparent green (80008800)
   // c->addInterLineLayer(layer0->getLine(), layer1->getLine(), 0x80ff0000, 0x80008800);

    // Add another normal line to the chart
   LineLayer* layer2 = c->addLineLayer(DoubleArray(data2, data2_size), 0x11408c, "CHANGE VALUE");
    layer2->setLineWidth(2);

    // Add a horizontal mark line to the chart at y = 40
   /* Mark* mark = c->yAxis()->addMark(40, -1, "Total count");
    mark->setLineWidth(2);

    // Set the mark line to purple (880088) dash line. Use white (ffffff) for the mark label.
    mark->setMarkColor(c->dashLineColor(0x880088), 0xffffff);

    // Put the mark label at the left side of the mark, with a purple (880088) background.
    mark->setAlignment(Chart::Left);
    mark->setBackground(0x880088);

    // Color the region between the above normal line and mark line. Use the semi-transparent blue
    // (800000ff) if the normal line is higher than the mark line, otherwise use semi-transparent
    // purple (80880088)
    c->addInterLineLayer(layer2->getLine(), mark->getLine(), 0x800000ff, 0x80880088);
    */
    // Layout the legend box, so we can get its height
    c->layoutLegend();

    // Adjust the plot area size, such that the bounding box (inclusive of axes) is 10 pixels from
    // the left edge, just under the legend box, 25 pixels from the right edge, and 10 pixels from
    // the bottom edge.
    c->packPlotArea(10, legendBox->getTopY() + legendBox->getHeight(), c->getWidth() - 25,
        c->getHeight() - 10);

    // After determining the exact plot area position, we may adjust the legend box and the title
    // positions so that they are centered relative to the plot area (instead of the chart)
    legendBox->setPos(plotArea->getLeftX() + (plotArea->getWidth() - legendBox->getWidth()) / 2,
        legendBox->getTopY());
    title->setPos(plotArea->getLeftX() + (plotArea->getWidth() - title->getWidth()) / 2,
        title->getTopY());

    // Output the chart
    m_ChartViewer.setChart(c);
    m_ChartViewer.setUpdateInterval(20);
    //include tool tip for the chart
    m_ChartViewer.setImageMap(c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} in {xLabel}: {value}'"));
    // In this sample project, we do not need to chart object any more, so we 
    // delete it now.
    delete c;

    return TRUE;
}
BOOL CBalanceChartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    mMainBrush.CreateSolidBrush(RGB(32, 33, 36));
    m_RldBtn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;
    m_RldBtn.SetFaceColor(RGB(50, 50, 50), true);
    m_RldBtn.SetTextColor(RGB(156, 156, 156));
    // Set the tooltip of the button.
    m_RldBtn.SetTooltip(_T("Reload chart"));

    m_LoadExpBtn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;
    m_LoadExpBtn.SetFaceColor(RGB(50, 50, 50), true);
    m_LoadExpBtn.SetTextColor(RGB(156, 156, 156));
    // Set the tooltip of the button.
    m_LoadExpBtn.SetTooltip(_T("Load Player EXP chart"));

    m_LoadMarketBtn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;
    m_LoadMarketBtn.SetFaceColor(RGB(50, 50, 50), true);
    m_LoadMarketBtn.SetTextColor(RGB(156, 156, 156));
    // Set the tooltip of the button.
    m_LoadMarketBtn.SetTooltip(_T("Load Selected Market chart"));

    chkRegistered = (CButton*)GetDlgItem(IDC_RADIO1);
    chkRecalled = (CButton*)GetDlgItem(IDC_RADIO2);
    chkTraded = (CButton*)GetDlgItem(IDC_RADIO3);

    chkTaxSPI = (CButton*)GetDlgItem(IDC_RADIO4);
    chkTaxWP = (CButton*)GetDlgItem(IDC_RADIO5);
    chkTaxCR = (CButton*)GetDlgItem(IDC_RADIO6); 
    
    chkMoneySPI = (CButton*)GetDlgItem(IDC_RADIO7);
    chkMoneyWP = (CButton*)GetDlgItem(IDC_RADIO8);
    chkMoneyCR = (CButton*)GetDlgItem(IDC_RADIO9);

    chkTransactions = (CButton*)GetDlgItem(IDC_RADIO10);

    chkRegistered->SetCheck(TRUE);

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	return TRUE;
}

// *** code automatically generated by VC++ MFC AppWizard ***
// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CBalanceChartDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// *** code automatically generated by VC++ MFC AppWizard ***
// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CBalanceChartDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

HBRUSH CBalanceChartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    return mMainBrush;
}

void CBalanceChartDlg::OnBnClickedButton1()
{
    UpdateData();

    DBConn();
    DrawGraph();
    m_ChartViewer.updateViewPort(true,false);
    //MessageBox("reloaded");
}


void CBalanceChartDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult)
{
    UpdateData();
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
  //  OnBnClickedButton1();
    *pResult = 0;
}


void CBalanceChartDlg::OnBnClickedButtonChartRld2()
{
    // TODO: Add your control notification handler code here

    UpdateData();

    GetExpChart();
    DrawGraph2();
    m_ChartViewer.updateViewPort(true, false);
}
BOOL CBalanceChartDlg::GetMarketChart()
{
    UpdateData();

    vectMarketCountsXfers.clear();
    vectComputedTransfers.clear();
    vectTransfersCounts.clear();
    vectTaxMoney.clear();


    if (FALSE == m_odbcStmt2.Init(m_pMainDlg->m_pServerInfo4Admin->DBIP, m_pMainDlg->m_pServerInfo4Admin->DBPort, m_pMainDlg->m_pServerInfo4Admin->DBName,
        m_pMainDlg->m_pServerInfo4Admin->DBUID, m_pMainDlg->m_pServerInfo4Admin->DBPWD, GetSafeHwnd()))
    {
        char szTemp[1024];
        sprintf(szTemp, "Can not connect DBServer<%s(%s:%d)> !!"
            , m_pMainDlg->m_pServerInfo4Admin->DBName, m_pMainDlg->m_pServerInfo4Admin->DBIP
            , m_pMainDlg->m_pServerInfo4Admin->DBPort);
        MessageBox(szTemp);
        EndDialog(-1);
        return FALSE;
    }
    BOOL bRet = FALSE;

    SQLHSTMT m_pODBCStmt1 = m_odbcStmt2.GetSTMTHandle();

    CString szStartDate, szEndDate;
    CTime tmStartDate;
    CString cs;
    CString cs_charuid;

    GetDlgItem(IDC_EDIT1)->GetWindowText(cs_charuid);
    UID32_t CharUID = 10675;

    if (cs_charuid.GetLength() > 2)
        CharUID = atol(cs_charuid);

    szStartDate.Format("%s", (LPCSTR)m_FromDate.Format("%Y/%m/%d 0:0:0.0"));
    szEndDate.Format("%s", (LPCSTR)m_ToDate.Format("%Y/%m/%d 23:59:59.0"));

    CString szFinalQuery;
    if (chkRegistered->GetCheck()) {
        szFinalQuery.Format("SELECT\
        COUNT(*) as RegisteredItems,\
        DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as RegisteredDay\
        FROM\
        atum_log_market_register\
 WHERE Time >= '%s' AND Time <= '%s'\
        GROUP BY\
        DATEADD(dd, DATEDIFF(dd, 0, Time), 0)", szStartDate, szEndDate);
    }
    else if (chkRecalled->GetCheck())
    {
        szFinalQuery.Format("SELECT\
        COUNT(*) as Items,\
        DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as Day\
        FROM\
        atum_log_market_recall\
 WHERE Time >= '%s' AND Time <= '%s'\
        GROUP BY\
        DATEADD(dd, DATEDIFF(dd, 0, Time), 0)", szStartDate, szEndDate);
    }
    else if (chkTraded->GetCheck())
    {
        szFinalQuery.Format("SELECT\
        COUNT(*) as Items,\
        DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as Day\
        FROM\
        atum_log_market_trade\
 WHERE Time >= '%s' AND Time <= '%s'\
        GROUP BY\
        DATEADD(dd, DATEDIFF(dd, 0, Time), 0)", szStartDate, szEndDate);
    }
    else if (chkTaxSPI->GetCheck()) 
        {
            szFinalQuery.Format(" SELECT\
                COUNT(*) as Transactions,\
                SUM(ChargePrice) as MarketTax_SPI,\
                /*SUM(CAST(Price as bigint)) as TotalMoneyTransfered,*/\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as BoughtDay\
                FROM\
                atum_log_market_recall\
                where(marketstate = 2 or marketstate = 1) and MoneyType = 'spi' and Time >= '%s' AND Time <= '%s'\
                GROUP BY\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0) ", szStartDate, szEndDate);

        }
    else if (chkTaxWP->GetCheck()) 
        {
            szFinalQuery.Format(" SELECT\
                COUNT(*) as Transactions,\
                SUM(ChargePrice) as MarketTax_SPI,\
                /*SUM(CAST(Price as bigint)) as TotalMoneyTransfered,*/\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as BoughtDay\
                FROM\
                atum_log_market_recall\
                where(marketstate = 2 or marketstate = 1) and MoneyType = 'wp' and Time >= '%s' AND Time <= '%s'\
                GROUP BY\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0);\
            ", szStartDate, szEndDate);
        }
    else if (chkTaxCR->GetCheck()) 
        {
            szFinalQuery.Format(" SELECT\
                COUNT(*) as Transactions,\
                SUM(ChargePrice) as MarketTax_SPI,\
                /*SUM(CAST(Price as bigint)) as TotalMoneyTransfered,*/\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as BoughtDay\
                FROM\
                atum_log_market_recall\
                where(marketstate = 2 or marketstate = 1) and MoneyType = 'credits' and Time >= '%s' AND Time <= '%s'\
                GROUP BY\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0);\
            ", szStartDate, szEndDate);
        }
    else if (chkMoneySPI->GetCheck())
        {
            szFinalQuery.Format(" SELECT\
                COUNT(*) as Transactions,\
                /*SUM(ChargePrice) as MarketTax_SPI,*/\
                SUM(CAST(Price as bigint)) as TotalMoneyTransfered,\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as BoughtDay\
                FROM\
                atum_log_market_recall\
                where(marketstate = 2 or marketstate = 1) and MoneyType = 'spi' and Time >= '%s' AND Time <= '%s'\
                GROUP BY\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0) ", szStartDate, szEndDate);

        }
    else if (chkMoneyWP->GetCheck())
        {
            szFinalQuery.Format(" SELECT\
                COUNT(*) as Transactions,\
                /*SUM(ChargePrice) as MarketTax_SPI,*/\
                SUM(CAST(Price as bigint)) as TotalMoneyTransfered,\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as BoughtDay\
                FROM\
                atum_log_market_recall\
                where(marketstate = 2 or marketstate = 1) and MoneyType = 'wp' and Time >= '%s' AND Time <= '%s'\
                GROUP BY\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0);\
            ", szStartDate, szEndDate);
        }
    else if (chkMoneyCR->GetCheck())
        {
            szFinalQuery.Format(" SELECT\
                COUNT(*) as Transactions,\
               /* SUM(ChargePrice) as MarketTax_SPI,*/\
                SUM(CAST(Price as bigint)) as TotalMoneyTransfered,\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as BoughtDay\
                FROM\
                atum_log_market_recall\
                where(marketstate = 2 or marketstate = 1) and MoneyType = 'credits' and Time >= '%s' AND Time <= '%s'\
                GROUP BY\
                DATEADD(dd, DATEDIFF(dd, 0, Time), 0);\
            ", szStartDate, szEndDate);
        }
    else if (chkTransactions->GetCheck())
    {
        szFinalQuery.Format(" (SELECT\
            1, COUNT(*) as RegisteredItems,\
            DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as RegisteredDay\
            FROM\
            atum_log_market_register\
 WHERE Time >= '%s' AND Time <= '%s'\
            GROUP BY\
            DATEADD(dd, DATEDIFF(dd, 0, Time), 0)\
            union\
            SELECT\
            2, COUNT(*) as RecalledItems,\
            DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as RecalledDay\
            FROM\
            atum_log_market_recall\
 WHERE Time >= '%s' AND Time <= '%s'\
            GROUP BY\
            DATEADD(dd, DATEDIFF(dd, 0, Time), 0)\
            union\
            SELECT\
            3, COUNT(*) as TradedItems,\
            DATEADD(dd, DATEDIFF(dd, 0, Time), 0) as TRADEDay\
            FROM\
            atum_log_market_trade\
 WHERE Time >= '%s' AND Time <= '%s'\
            GROUP BY\
            DATEADD(dd, DATEDIFF(dd, 0, Time), 0)\
        ) ", szStartDate, szEndDate, szStartDate, szEndDate, szStartDate, szEndDate);

    }
    bRet = m_odbcStmt2.ExecuteQuery((LPSTR)(LPCSTR)szFinalQuery);

    if (!bRet)
    {
        m_odbcStmt2.FreeStatement();		// clean up
        // m_odbcStmt2.Clean();
        MessageBox("ExecuteQuery Error 1");
        return TRUE;
    }

    SQLLEN arrCB[5] = { SQL_NTS, SQL_NTS, SQL_NTS, SQL_NTS , SQL_NTS };
    char	tmDate[SIZE_DATE];
    int		nCount;
    int		nXferCount;
    int		nCharge;
    long long nTotal=0;

    if (chkTransactions->GetCheck()) {
        SQLBindCol(m_odbcStmt2.m_hstmt, 1, SQL_C_LONG, &nCount, 0, &arrCB[1]);
        SQLBindCol(m_odbcStmt2.m_hstmt, 2, SQL_C_LONG, &nXferCount, 0, &arrCB[2]);
        SQLBindCol(m_odbcStmt2.m_hstmt, 3, SQL_C_CHAR, &tmDate, SIZE_DATE, &arrCB[3]);

        while ((bRet = SQLFetch(m_odbcStmt2.m_hstmt)) != SQL_NO_DATA)
        {
            SDATE_MARKET_TRANSFERS_COUNTS tmMarketCnt;
            memset(&tmMarketCnt, 0x00, sizeof(SDATE_MARKET_TRANSFERS_COUNTS));
            strncpy(tmMarketCnt.Date, tmDate, SIZE_DATE);
            tmMarketCnt.nType = nCount;
            tmMarketCnt.nXfersCount = nXferCount;

            vectTransfersCounts.push_back(tmMarketCnt);
        }
    }
    else {

        SQLBindCol(m_odbcStmt2.m_hstmt, 1, SQL_C_LONG, &nCount, 0, &arrCB[1]);

        if (chkTaxSPI->GetCheck() || chkTaxWP->GetCheck() || chkTaxCR->GetCheck())
        {
            SQLBindCol(m_odbcStmt2.m_hstmt, 2, SQL_C_LONG, &nCharge, 0, &arrCB[2]);
            //  SQLBindCol(m_odbcStmt2.m_hstmt, 3, SQL_C_UBIGINT, &nTotal, 0, &arrCB[3]);
            SQLBindCol(m_odbcStmt2.m_hstmt, 3, SQL_C_CHAR, &tmDate, SIZE_DATE, &arrCB[3]);
        }
        else if (chkMoneySPI->GetCheck() || chkMoneyWP->GetCheck() || chkMoneyCR->GetCheck())
        {
            SQLBindCol(m_odbcStmt2.m_hstmt, 2, SQL_C_UBIGINT, &nTotal, 0, &arrCB[2]);
            //  SQLBindCol(m_odbcStmt2.m_hstmt, 3, SQL_C_UBIGINT, &nTotal, 0, &arrCB[3]);
            SQLBindCol(m_odbcStmt2.m_hstmt, 3, SQL_C_CHAR, &tmDate, SIZE_DATE, &arrCB[3]);
        }
        else
        {
            SQLBindCol(m_odbcStmt2.m_hstmt, 2, SQL_C_CHAR, &tmDate, SIZE_DATE, &arrCB[2]);
        }

        while ((bRet = SQLFetch(m_odbcStmt2.m_hstmt)) != SQL_NO_DATA)
        {
            if (chkTaxSPI->GetCheck() || chkTaxWP->GetCheck() || chkTaxCR->GetCheck() || chkMoneySPI->GetCheck() || chkMoneyWP->GetCheck() || chkMoneyCR->GetCheck())
            {
                SDATE_MARKET_TAX tmMarketCnt;
                memset(&tmMarketCnt, 0x00, sizeof(SDATE_MARKET_TAX));
                strncpy(tmMarketCnt.Date, tmDate, SIZE_DATE);
                tmMarketCnt.nTransfersCount = nCount;
                tmMarketCnt.nTax = nCharge;
                tmMarketCnt.nTotal = nTotal;

                vectTaxMoney.push_back(tmMarketCnt);
            }
            else
            {
                SDATE_MARKET_COUNTS tmMarketCnt;
                memset(&tmMarketCnt, 0x00, sizeof(SDATE_MARKET_COUNTS));
                strncpy(tmMarketCnt.Date, tmDate, SIZE_DATE);
                tmMarketCnt.nCount = nCount;

                vectMarketCountsXfers.push_back(tmMarketCnt);
            }
        }
    }
    m_odbcStmt2.FreeStatement();
    // m_odbcStmt2.Clean();
    return FALSE;
}

void CBalanceChartDlg::OnBnClickedButtonChartRld3()
{
    UpdateData();

    GetMarketChart();
    DrawMarketCntGraph();
    m_ChartViewer.updateViewPort(true, false);
}
BOOL CBalanceChartDlg::DrawMarketCntGraph()
{
    const char* labels[1024];
    double data0[1024];
    double data1[1024];
    double data2[1024];
    double data3[1024];

    char szTemp[1024];

    int nSize = 0;
    if (chkTaxSPI->GetCheck() || chkTaxWP->GetCheck() || chkTaxCR->GetCheck() || chkMoneySPI->GetCheck() || chkMoneyWP->GetCheck() || chkMoneyCR->GetCheck())
        nSize = vectTaxMoney.size();
    else
        nSize = vectMarketCountsXfers.size();

    
    sprintf(szTemp, "Update chart with %d size data", nSize);
    // MessageBox(szTemp);
    int i = 0;
    if (chkTransactions->GetCheck())
    {     
        for (auto& el : vectTransfersCounts) {

            SDATE_MARKET_COMPUTED_3_VEC tmp;
            memset(&tmp, 0x00, sizeof(SDATE_MARKET_COMPUTED_3_VEC));
            bool bExist = false;
            for (auto& el2 : vectComputedTransfers) {
                if (strcmp(el.Date, el2.Date) == 0)
                    bExist = true;
            }
            if (!bExist || vectComputedTransfers.empty())
            {
                strncpy(tmp.Date, el.Date, SIZE_DATE);
                tmp.nXfersCountRegist = 0;
                tmp.nXfersCountRecall = 0;
                tmp.nXfersCountTrade = 0;
                if (el.nType == 1)
                    tmp.nXfersCountRegist = el.nXfersCount;
                if (el.nType == 2)
                    tmp.nXfersCountRecall = el.nXfersCount;
                if (el.nType == 3)
                    tmp.nXfersCountTrade = el.nXfersCount;

                vectComputedTransfers.push_back(tmp);
            }      
        }

        for (auto& el : vectTransfersCounts) {
            for (auto& el2 : vectComputedTransfers) {
                if (strcmp(el.Date, el2.Date) == 0) {

                    if (el.nType == 1)
                        el2.nXfersCountRegist = el.nXfersCount;
                    if (el.nType == 2)
                        el2.nXfersCountRecall = el.nXfersCount;
                    if (el.nType == 3)
                        el2.nXfersCountTrade = el.nXfersCount;

                }
            }
        }
        nSize = vectComputedTransfers.size();
        for (auto& el : vectComputedTransfers) {
            labels[i] = el.Date;
            data0[i] = el.nXfersCountRegist;
            data1[i] = el.nXfersCountRecall;
            data2[i] = el.nXfersCountTrade;
            i++;
        }
    }
    else
    {
        if (chkTaxSPI->GetCheck() || chkTaxWP->GetCheck() || chkTaxCR->GetCheck())
        {
            for (auto& el : vectTaxMoney) {
                labels[i] = el.Date;

                data0[i] = el.nTotal;
                data1[i] = el.nTax;
                data2[i] = el.nTransfersCount;

                i++;
            }
        }
        else if (chkMoneySPI->GetCheck() || chkMoneyWP->GetCheck() || chkMoneyCR->GetCheck())
        {
            for (auto& el : vectTaxMoney) {
                labels[i] = el.Date;

                data0[i] = el.nTotal;
                data1[i] = el.nTotal;
                data2[i] = el.nTransfersCount;

                i++;
            }
        }
        else
        {
            for (auto& el : vectMarketCountsXfers) {
                labels[i] = el.Date;

                data0[i] = el.nCount;
                data1[i] = el.nCount;
                data2[i] = el.nCount;

                i++;
            }
        }
    }
    const int labels_size = nSize;
    const int data0_size = nSize;
    const int data1_size = nSize;
    const int data2_size = nSize;
    const int data3_size = nSize;
  //  const int data2_size = nSize;
    // Create a XYChart object of size 450 x 450 pixels
    LONG x, y;
    int nChartSizeX = 750;
    int nChartSizeY = 450;
    int nMainWndSizeX = nChartSizeX + 230;
    int nMainWndSizeY = nChartSizeY + 30;
    x = LONG((GetSystemMetrics(SM_CXSCREEN) - nMainWndSizeX) / 2);
    y = LONG((GetSystemMetrics(SM_CYSCREEN) - nMainWndSizeY) / 2);

    XYChart* c = new XYChart(nChartSizeX, nChartSizeY);

    MoveWindow(x, y, nMainWndSizeX, nMainWndSizeY);
    GetDlgItem(IDC_CHART)->MoveWindow(nMainWndSizeX - nChartSizeX - 30, nMainWndSizeY - nChartSizeY - 30, nChartSizeX, nChartSizeY);
    c->setBackground(0x202124);
    // Add a title to the chart using 15pt Arial Italic font.
    TextBox* title = c->addTitle("Market Registered Items", "Arial Italic", 15, 0x9c9c9c);

    LegendBox* legendBox = c->addLegend(c->getWidth() / 2, title->getHeight(), false,
        "Arial Bold Italic", 10);
    legendBox->setBackground(Chart::Transparent, Chart::Transparent);
    legendBox->setAlignment(Chart::TopCenter);
    legendBox->setLineStyleKey();
    legendBox->setFontColor(0x525354);
    // Tentatively set the plotarea at (70, 65) and of (chart_width - 100) x (chart_height - 110) in
    // size. Use light grey (c0c0c0) border and horizontal and vertical grid lines.
    PlotArea* plotArea = c->setPlotArea(70, 65, c->getWidth() - 100, c->getHeight() - 110, -1, -1,
        0xc0c0c0, 0x525354, -1);

    // Add a title to the y axis using 12pt Arial Bold Italic font
    c->yAxis()->setTitle("Count", "Arial Bold Italic", 12, 0x9c9c9c);
    c->yAxis()->setColors(0x525354, 0x525354, 0x9c9c9c);
    // Add a title to the x axis using 12pt Arial Bold Italic font
    c->xAxis()->setTitle("Date", "Arial Bold Italic", 12, 0x9c9c9c);

    // Set the axes line width to 3 pixels
    c->xAxis()->setWidth(3);
    c->yAxis()->setWidth(3);
   
    // Set the labels on the x axis.
    c->xAxis()->setLabels(StringArray(labels, labels_size));

    // Use 8 points Arial rotated by 90 degrees as the x-axis label font
    c->xAxis()->setLabelStyle("Arial", 8, 0x525354, 90);
   // ScatterLayer* layersc =  c->addScatterLayer(DoubleArray(data0, data0_size), DoubleArray(data2, data2_size), "Default CDML Tooltip", 1, 19,
      //  0xff3333);
   // layersc->setHTMLImageMap("", "", "title='<*cdml*>{dataSetName}<*br*>X = {x}, Y = {value}'");
    // Add a spline curve to the chart
    if (chkTransactions->GetCheck())
    {
        SplineLayer* layer0 = c->addSplineLayer(DoubleArray(data0, data0_size), 0xff8500, "Registered");
        layer0->setLineWidth(2);
        SplineLayer* layer1 = c->addSplineLayer(DoubleArray(data1, data1_size), 0x11408c, "Recalled");
        layer1->setLineWidth(2);
        SplineLayer* layer2 = c->addSplineLayer(DoubleArray(data2, data2_size), 0x2bff00, "Traded");
        layer2->setLineWidth(2);
    }
    else
    {
        if (chkTaxSPI->GetCheck() || chkTaxWP->GetCheck() || chkTaxCR->GetCheck())
        {
            // SplineLayer* layer0 = c->addSplineLayer(DoubleArray(data0, data0_size), 0xff8500, "TOTAL");
           //  layer0->setLineWidth(2); 

            SplineLayer* layer1 = c->addSplineLayer(DoubleArray(data1, data1_size), 0xff8500, "TAX");
            layer1->setLineWidth(2);
            SplineLayer* layer2 = c->addSplineLayer(DoubleArray(data2, data2_size), 0x11408c, "Transfers");
            layer2->setLineWidth(2);
        }
        else  if (chkMoneyCR->GetCheck() || chkMoneyWP->GetCheck() || chkMoneySPI->GetCheck())
        {
            SplineLayer* layer0 = c->addSplineLayer(DoubleArray(data0, data0_size), 0xff8500, "TOTAL");
            layer0->setLineWidth(2);

            //   SplineLayer* layer1 = c->addSplineLayer(DoubleArray(data1, data1_size), 0xff8500, "TAX");
             //  layer1->setLineWidth(2);
            SplineLayer* layer2 = c->addSplineLayer(DoubleArray(data2, data2_size), 0x11408c, "Transfers");
            layer2->setLineWidth(2);
        }
        else
        {
            SplineLayer* layer0 = c->addSplineLayer(DoubleArray(data0, data0_size), 0xff8500, "Transfers");
            layer0->setLineWidth(2);
        }
    }
   // LineLayer* layer2 = c->addLineLayer(DoubleArray(data2, data2_size), 0x11408c, "CHANGE VALUE");
  //  layer2->setLineWidth(2);
    c->layoutLegend();

    c->packPlotArea(10, legendBox->getTopY() + legendBox->getHeight(), c->getWidth() - 25,
        c->getHeight() - 10);

    legendBox->setPos(plotArea->getLeftX() + (plotArea->getWidth() - legendBox->getWidth()) / 2,
        legendBox->getTopY());
    title->setPos(plotArea->getLeftX() + (plotArea->getWidth() - title->getWidth()) / 2,
        title->getTopY());

    // Output the chart
    m_ChartViewer.setChart(c);
    m_ChartViewer.setUpdateInterval(20);
    //include tool tip for the chart
    m_ChartViewer.setImageMap(c->getHTMLImageMap("clickable", "",
        "title='{dataSetName} in {xLabel}: {value/1000000}'"));
    // In this sample project, we do not need to chart object any more, so we 
    // delete it now.
    delete c;

    return TRUE;
}
