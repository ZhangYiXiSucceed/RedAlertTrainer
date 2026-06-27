
// RedAlertTrainerView.cpp: CRedAlertTrainerView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "RedAlertTrainer.h"
#endif

#include "RedAlertTrainerDoc.h"
#include "RedAlertTrainerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRedAlertTrainerView

IMPLEMENT_DYNCREATE(CRedAlertTrainerView, CView)

BEGIN_MESSAGE_MAP(CRedAlertTrainerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRedAlertTrainerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CRedAlertTrainerView 构造/析构

CRedAlertTrainerView::CRedAlertTrainerView() noexcept
{
	// TODO: 在此处添加构造代码

}

CRedAlertTrainerView::~CRedAlertTrainerView()
{
}

BOOL CRedAlertTrainerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CRedAlertTrainerView 绘图

void CRedAlertTrainerView::OnDraw(CDC* /*pDC*/)
{
	CRedAlertTrainerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CRedAlertTrainerView 打印


void CRedAlertTrainerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CRedAlertTrainerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CRedAlertTrainerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CRedAlertTrainerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CRedAlertTrainerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CRedAlertTrainerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CRedAlertTrainerView 诊断

#ifdef _DEBUG
void CRedAlertTrainerView::AssertValid() const
{
	CView::AssertValid();
}

void CRedAlertTrainerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRedAlertTrainerDoc* CRedAlertTrainerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRedAlertTrainerDoc)));
	return (CRedAlertTrainerDoc*)m_pDocument;
}
#endif //_DEBUG


// CRedAlertTrainerView 消息处理程序
