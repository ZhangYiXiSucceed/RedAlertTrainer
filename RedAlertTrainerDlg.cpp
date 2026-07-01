
// RedAlertTrainerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "RedAlertTrainer.h"
#include "RedAlertTrainerDlg.h"
#include "CProcessSelectDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRedAlertTrainerDlg 对话框



CRedAlertTrainerDlg::CRedAlertTrainerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REDALERTTRAINER_DIALOG, pParent)
	, m_hProcess(NULL)
	, m_dwProcessId(0)
	, m_bAttached(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRedAlertTrainerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PictureShow, m_PictureShow);
	DDX_Control(pDX, IDC_SelectProcess, m_btnSelectProcess);
	DDX_Control(pDX, IDC_AttachProcess, m_btnAttachProcess);
	DDX_Control(pDX, IDC_ListLog, m_listLog);
	DDX_Control(pDX, IDC_CurrentMoney, m_editCurrentMoneyValue);
	DDX_Control(pDX, IDC_CurrentElectricity, m_editCurrentElectricityValue);
	DDX_Control(pDX, IDC_CurrentPayload, m_editCurrentPayloaValue);
	DDX_Control(pDX, IDC_NoMoneyConsume, m_checkNoMoneyConsume);
	DDX_Control(pDX, IDC_NoPowerConsume, m_checkNoPowerConsume);
	DDX_Control(pDX, IDC_FastBuild, m_checkFastBuild);
	DDX_Control(pDX, IDC_FreeBuild, m_checkFreeBuild);
}

BEGIN_MESSAGE_MAP(CRedAlertTrainerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()  // 添加这一行：定时器消息映射
	ON_BN_CLICKED(IDC_SelectProcess, &CRedAlertTrainerDlg::OnBnClickedSelectprocess)
	ON_BN_CLICKED(IDC_AttachProcess, &CRedAlertTrainerDlg::OnBnClickedAttachprocess)

	ON_EN_CHANGE(IDC_CurrentMoney,   &CRedAlertTrainerDlg::OnEnChangeCurrentmoney)
	ON_BN_CLICKED(IDC_NoMoneyConsume, &CRedAlertTrainerDlg::OnBnClickedCheckNoMoneyConsume)
	ON_BN_CLICKED(IDC_NoPowerConsume, &CRedAlertTrainerDlg::OnBnClickedCheckNoPowerConsume)
	ON_BN_CLICKED(IDC_FastBuild, &CRedAlertTrainerDlg::OnBnClickedCheckFastBuild)
	ON_BN_CLICKED(IDC_FreeBuild, &CRedAlertTrainerDlg::OnBnClickedCheckFreeBuild)

	
END_MESSAGE_MAP()


// CRedAlertTrainerDlg 消息处理程序

BOOL CRedAlertTrainerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_SHOW);

	// TODO: 在此添加额外的初始化代码
	// 设置定时器，等待控件完全创建图片
	SetTimer(1, 100, NULL);  // 100毫秒后触发

	// 初始化不消化金钱复选框
	m_checkNoMoneyConsume.SetCheck(BST_UNCHECKED);
	m_bNoMoneyConsumeEnabled = FALSE;
	m_dwNoMoneyConsumeAddress = 0;
	m_dwNoMoneyConsumeAllocated = 0;
	m_bNoMoneyConsumeMemoryAllocated = FALSE;
	m_checkNoMoneyConsume.EnableWindow(FALSE);

	AddLog(_T("基地址偏移: 0x%X"), RED_ALERT_BASE_ADDR);
	AddLog(_T("金钱地址偏移: 0x%X"), MONEY_OFFSET);
	AddLog(_T("电力地址偏移: 0x%X"), ELECTRICITY_OFFSET);
	AddLog(_T("负载地址偏移: 0x%X"), PAYLOAD_OFFSET);

	AddLog(_T("不消化金钱功能已加载"));
	AddLog(_T("不消化金钱地址偏移: 0x%X"), NO_MONEY_CONSUME_OFFSET);

	// 初始化不消耗电力复选框
	m_checkNoPowerConsume.SetCheck(BST_UNCHECKED);
	m_bNoPowerConsumeEnabled = FALSE;
	m_dwNoPowerConsumeAddress = 0;
	m_dwNoPowerConsumeAllocated = 0;
	m_bNoPowerConsumeMemoryAllocated = FALSE;
	m_checkNoPowerConsume.EnableWindow(FALSE);

	AddLog(_T("不消耗电力功能已加载"));
	AddLog(_T("不消耗电力地址偏移: 0x%X"), NO_POWER_CONSUME_OFFSET);

	// 初始化快速建筑复选框
	m_checkFastBuild.SetCheck(BST_UNCHECKED);
	m_bFastBuildEnabled = FALSE;
	m_dwFastBuildAddress = 0;
	m_dwFastBuildAllocated = 0;
	m_bFastBuildMemoryAllocated = FALSE;
	m_checkFastBuild.EnableWindow(FALSE);

	AddLog(_T("快速建筑功能已加载"));
	AddLog(_T("快速建筑地址偏移: 0x%X"), FAST_BUILD_OFFSET);

	// 初始化随意建筑复选框
	m_checkFreeBuild.SetCheck(BST_UNCHECKED);
	m_bFreeBuildEnabled = FALSE;
	m_dwFreeBuildAddress = 0;
	m_dwFreeBuildAllocated = 0;
	m_bFreeBuildMemoryAllocated = FALSE;
	m_checkFreeBuild.EnableWindow(FALSE);

	AddLog(_T("随意建筑功能已加载"));
	AddLog(_T("随意建筑地址偏移: 0x%X"), FREE_BUILD_OFFSET);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRedAlertTrainerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRedAlertTrainerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRedAlertTrainerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 添加定时器响应
void CRedAlertTrainerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		//KillTimer(1);  // 停止定时器

		// 在这里加载默认图片
		CString defaultPath = _T("background.jpg");  // 设置你的默认图片路径
		if (PathFileExists(defaultPath))
		{
			ShowBackgroundPicture(defaultPath);
		}

		if (m_bAttached)
		{
			ReadBaseValue();
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CRedAlertTrainerDlg::ShowBackgroundPicture(CString m_JpgPathName)
{
	// 保存图片路径
	m_strCurrentImagePath = m_JpgPathName;
	// 立即绘制
	DrawPictureToControl();
}

// 新增：实际的绘制函数
void CRedAlertTrainerDlg::DrawPictureToControl()
{
	if (m_strCurrentImagePath.IsEmpty())
		return;

	if (!PathFileExists(m_strCurrentImagePath))
		return;

	CRect rect;
	m_PictureShow.GetClientRect(&rect);

	if (rect.IsRectEmpty())
		return;

	CImage image;
	HRESULT hr = image.Load(m_strCurrentImagePath);
	if (FAILED(hr))
		return;

	CDC* pDC = m_PictureShow.GetDC();
	if (pDC && pDC->GetSafeHdc())
	{
		SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);

		// 直接拉伸到整个控件区域，不保持比例（可能会变形）
		image.StretchBlt(pDC->m_hDC, rect, SRCCOPY);

		m_PictureShow.ReleaseDC(pDC);
	}
}

void CRedAlertTrainerDlg::OnBnClickedSelectprocess()
{
	// TODO: 在此添加控件通知处理程序代码
	// 如果已经附加进程，先分离
	if (m_bAttached)
	{
		AddLog(_T("[选择进程] 请先分离当前进程"));
		MessageBox(_T("请先点击【附加进程】按钮分离当前进程！"),
			_T("提示"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	// 打开进程选择对话框
	CProcessSelectDlg dlg;

	if (dlg.DoModal() == IDOK)
	{
		CString strProcessName = dlg.GetSelectedProcessName();
		DWORD dwPID = dlg.GetSelectedProcessID();

		AddLog(_T("[选择进程] 已选择进程: %s (PID: %d)"), strProcessName, dwPID);

		// 保存进程名称和PID
		m_strProcessName = strProcessName;
		m_dwProcessId = dwPID;

		// 更新按钮文字
		CString strBtnText;
		strBtnText.Format(_T("选择进程 [%s]"), m_strProcessName);
		m_btnSelectProcess.SetWindowText(strBtnText);

		// 保存配置
		//SaveConfigToIni();
	}
}


void CRedAlertTrainerDlg::OnBnClickedAttachprocess()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	if (m_bAttached)
	{
		DetachFromProcess();
		return;
	}

	if (AttachToProcess())
	{
		m_btnAttachProcess.SetWindowText(_T("分离进程"));

		// 获取模块基址并显示初始值功能地址
		// 获取模块基址
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
		{
			// 不消化金钱功能地址
			m_dwNoMoneyConsumeAddress = dwModuleBase + NO_MONEY_CONSUME_OFFSET;
			AddLog(_T("[不消化金钱] 目标地址: 0x%08X"), m_dwNoMoneyConsumeAddress);

			// 不消耗电力功能地址
			m_dwNoPowerConsumeAddress = dwModuleBase + NO_POWER_CONSUME_OFFSET;
			AddLog(_T("[不消耗电力] 目标地址: 0x%08X"), m_dwNoPowerConsumeAddress);
		
			// 快速建筑功能地址
			m_dwFastBuildAddress = dwModuleBase + FAST_BUILD_OFFSET;
			AddLog(_T("[快速建筑] 目标地址: 0x%08X"), m_dwFastBuildAddress);
		
			// 随意建筑功能地址
			m_dwFreeBuildAddress = dwModuleBase + FREE_BUILD_OFFSET;
			AddLog(_T("[随意建筑] 目标地址: 0x%08X"), m_dwFreeBuildAddress);
		}

		// 启用控件
		m_checkFastBuild.EnableWindow(TRUE);
		m_checkNoMoneyConsume.EnableWindow(TRUE);
		m_checkNoPowerConsume.EnableWindow(TRUE);
		m_checkFreeBuild.EnableWindow(TRUE);

		//// 自动读取一次数值
		ReadBaseValue();
	}
}

// 分离进程
void CRedAlertTrainerDlg::DetachFromProcess()
{

	if (m_hProcess)
	{
		CloseHandle(m_hProcess);
		m_hProcess = NULL;
	}
	m_bAttached = FALSE;
	m_dwProcessId = 0;

	// 恢复按钮文字
	m_btnAttachProcess.SetWindowText(_T("附加进程"));
	// 禁用控件
	m_checkFastBuild.EnableWindow(FALSE);
	m_checkNoMoneyConsume.EnableWindow(FALSE);
	m_checkNoPowerConsume.EnableWindow(FALSE);
	m_checkFreeBuild.EnableWindow(FALSE);

	AddLog(_T("已分离进程"));
}

// 附加进程
BOOL CRedAlertTrainerDlg::AttachToProcess()
{
	if (m_strProcessName.IsEmpty())
	{
		AddLog(_T("[附加进程] 错误: 请先选择进程"));
		MessageBox(_T("请先点击【选择进程】按钮选择游戏进程！"),
			_T("错误"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!FindProcessId(m_strProcessName, m_dwProcessId))
	{
		AddLog(_T("[附加进程] 错误: 未找到进程 %s"), m_strProcessName);
		AddLog(_T("[附加进程] 请确保游戏已经运行，并且进程名称正确"));

		CString strMsg;
		strMsg.Format(_T("未找到进程: %s\n\n请确保:\n1. 游戏已经运行\n2. 进程名称正确\n3. 以管理员权限运行此程序"),
			m_strProcessName);
		MessageBox(strMsg, _T("错误"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	AddLog(_T("[附加进程] 找到进程: %s (PID: %d, 0x%08X)"),
		m_strProcessName, m_dwProcessId, m_dwProcessId);

	// 尝试打开进程
	m_hProcess = OpenProcess(
		PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
		PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION,
		FALSE,
		m_dwProcessId
	);

	if (!m_hProcess)
	{
		DWORD dwError = GetLastError();
		AddLog(_T("[附加进程] 打开进程失败，错误码: %d"), dwError);

		if (dwError == 5)
		{
			AddLog(_T("[附加进程] 权限不足，请以管理员权限运行"));
			MessageBox(_T("无法打开进程！\n\n请以管理员身份运行此程序。"),
				_T("错误"), MB_OK | MB_ICONERROR);
		}
		return FALSE;
	}

	m_bAttached = TRUE;
	AddLog(_T("[附加进程] 成功附加进程 %s"), m_strProcessName);
	AddLog(_T("[附加进程] 进程句柄: 0x%08X"), (DWORD_PTR)m_hProcess);

	return TRUE;
}

// 查找进程ID
BOOL CRedAlertTrainerDlg::FindProcessId(LPCTSTR szProcessName, DWORD& dwProcessId)
{
	AddLog(_T("[查找进程] 正在查找: %s"), szProcessName);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		AddLog(_T("[查找进程] 创建进程快照失败，错误码: %d"), GetLastError());
		return FALSE;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	BOOL bFound = FALSE;
	if (Process32First(hSnapshot, &pe))
	{
		do
		{
			if (_tcsicmp(pe.szExeFile, szProcessName) == 0)
			{
				dwProcessId = pe.th32ProcessID;
				bFound = TRUE;
				AddLog(_T("[查找进程] 找到进程: %s (PID: %d)"), pe.szExeFile, dwProcessId);
				break;
			}
		} while (Process32Next(hSnapshot, &pe));
	}

	CloseHandle(hSnapshot);

	if (!bFound)
	{
		AddLog(_T("[查找进程] 未找到进程: %s"), szProcessName);

		// 列出所有运行中的进程，帮助用户排查
		AddLog(_T("[查找进程] 当前运行的进程列表:"));
		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			pe.dwSize = sizeof(PROCESSENTRY32);
			if (Process32First(hSnapshot, &pe))
			{
				int nCount = 0;
				do
				{
					if (nCount < 20)  // 只显示前20个
					{
						AddLog(_T("[查找进程]   %s (PID: %d)"), pe.szExeFile, pe.th32ProcessID);
					}
					nCount++;
				} while (Process32Next(hSnapshot, &pe));
				if (nCount > 20)
				{
					AddLog(_T("[查找进程]   ... 还有 %d 个进程"), nCount - 20);
				}
			}
			CloseHandle(hSnapshot);
		}
	}

	return bFound;
}

// 获取模块基址
DWORD_PTR CRedAlertTrainerDlg::GetModuleBaseAddress()
{
	if (!m_hProcess || !m_bAttached)
		return 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, m_dwProcessId);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	DWORD_PTR dwModuleBase = 0;
	if (Module32First(hSnapshot, &moduleEntry))
	{
		do
		{
			if (_tcsicmp(moduleEntry.szModule, m_strProcessName) == 0)
			{
				dwModuleBase = (DWORD_PTR)moduleEntry.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &moduleEntry));
	}

	CloseHandle(hSnapshot);
	return dwModuleBase;
}

// 添加日志
void CRedAlertTrainerDlg::AddLog(LPCTSTR szFormat, ...)
{
	CString strLog;
	va_list args;
	va_start(args, szFormat);
	strLog.FormatV(szFormat, args);
	va_end(args);

	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("[%H:%M:%S] "));

	m_listLog.AddString(strTime + strLog);

	int nCount = m_listLog.GetCount();
	if (nCount > 0)
		m_listLog.SetCurSel(nCount - 1);
}

// 读取指针链：[baseAddress] + offset1
DWORD_PTR CRedAlertTrainerDlg::ReadPointerChain(DWORD_PTR baseAddress, DWORD_PTR offset1)
{
	if (!m_hProcess)
		return 0;

	DWORD_PTR ptr1 = 0;
	SIZE_T bytesRead = 0;

	// 第一次读取：基址 -> 指针1
	if (!ReadProcessMemory(m_hProcess, (LPCVOID)baseAddress, &ptr1, sizeof(DWORD_PTR), &bytesRead))
	{
		AddLog(_T("读取基址失败，错误码: %d"), GetLastError());
		return 0;
	}

	if (bytesRead != sizeof(DWORD_PTR))
	{
		AddLog(_T("读取基址数据不完整"));
		return 0;
	}

	// 第一次偏移计算
	DWORD_PTR addr1 = ptr1 + offset1;

	// 第二次偏移计算，得到最终阳光地址
	DWORD_PTR finalAddr = addr1;
	return finalAddr;
}

DWORD CRedAlertTrainerDlg::ReadValue(DWORD BaseAddr, DWORD offset)
{
	// 获取最终金钱地址
	DWORD finalAddress = ReadPointerChain(BaseAddr, offset);

	if (finalAddress == 0)
	{
		AddLog(_T("无法获取地址"));
		return 0;
	}

	// 读读取金钱值
	DWORD dwMenoy = 0;
	SIZE_T bytesRead = 0;

	if (ReadProcessMemory(m_hProcess, (LPCVOID)finalAddress,
		&dwMenoy, sizeof(DWORD), &bytesRead))
	{
		if (bytesRead == sizeof(DWORD))
		{
			return dwMenoy;
		}
	}
	else
	{
		AddLog(_T("读取地址失败"));
	}
	return 0;
}


// 写入值
BOOL CRedAlertTrainerDlg::WriteValue(DWORD dwNewValue, DWORD BaseAddr, DWORD offset)
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("错误: 未附加进程"));
		return FALSE;
	}

	// 获取最终金钱地址
	DWORD finalAddress = ReadPointerChain(BaseAddr, offset);
	if (finalAddress == 0)
	{
		AddLog(_T("错误: 未获取地址"));
		return FALSE;
	}

	// 读出上一次值
	DWORD LastValue = ReadValue(BaseAddr, offset);

	// 修改内存保护属性
	DWORD dwOldProtect = 0;
	if (!VirtualProtectEx(m_hProcess, (LPVOID)finalAddress,
		sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		AddLog(_T("修改内存保护失败，错误码: %d"), GetLastError());
		// 继续尝试写入
	}

	// 写入新值
	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)finalAddress,
		&dwNewValue, sizeof(DWORD), &bytesWritten);

	// 恢复内存保护属性
	VirtualProtectEx(m_hProcess, (LPVOID)finalAddress,
		sizeof(DWORD), dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == sizeof(DWORD))
	{
		AddLog(_T("成功修改值: %d -> %d"),
			LastValue, dwNewValue);

		// 验证修改
		DWORD dwVerify = ReadValue(BaseAddr, offset);
		if (dwVerify == dwNewValue)
		{
			AddLog(_T("验证成功: 值已更新为 %d"), dwVerify);
			return TRUE;
		}
		else
		{
			AddLog(_T("警告: 验证失败，当前值为 %d"), dwVerify);
			return FALSE;
		}
	}
	else
	{
		AddLog(_T("写入失败，错误码: %d"), GetLastError());
		return FALSE;
	}
}

void CRedAlertTrainerDlg::OnEnChangeCurrentmoney()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	 // 1. 获取编辑框中的文本
	CString strText;
	GetDlgItemText(IDC_CurrentMoney, strText); 

	if (strText == m_strLastEditText)
	{
		// 文本没有实质变化，直接返回，不执行任何动作
		return;
	}

	if (strText.IsEmpty())
	{
		return;
	}

	m_strLastEditText = strText;

	int dwNewValue = _ttoi(strText);
	//AddLog(_T("准备修改金钱值为: %d"), dwNewValue);
	if (WriteValue(dwNewValue, 0xA35DB4, 0x24C))
	{
		// 刷新显示
		DWORD dwCurrent = ReadValue(0xA35DB4, 0x24C);
		CString strCurrent;
		strCurrent.Format(_T("%d"), dwCurrent);
		m_editCurrentMoneyValue.SetWindowText(strCurrent);
	}
}


void CRedAlertTrainerDlg::ReadBaseValue()
{
	DWORD dwCurrenMenoy = ReadValue(0xA35DB4, 0x24C);
	CString strValue;
	strValue.Format(_T("%d"), dwCurrenMenoy);
	m_editCurrentMoneyValue.SetWindowText(strValue);

	DWORD dwCurrentElectricity = ReadValue(0xA35DB4, 0x52D0);
	strValue.Format(_T("%d"), dwCurrentElectricity);
	m_editCurrentElectricityValue.SetWindowText(strValue);

	DWORD dwCurrentPayload = ReadValue(0xA35DB4, PAYLOAD_OFFSET);
	strValue.Format(_T("%d"), dwCurrentPayload);
	m_editCurrentPayloaValue.SetWindowText(strValue);
}

// 原始字节码: sub eax,edi; mov [esp+24],edi (6字节)
const BYTE NO_MONEY_CONSUME_ORIGINAL_BYTES[] = {
	0x2B, 0xC7,        // sub eax, edi
	0x89, 0x7C, 0x24, 0x24  // mov [esp+24], edi
};

// 自定义代码: nop; mov [esp+24],edi (6字节)
// 将 sub eax,edi 替换为 nop，不消耗金钱
const BYTE NO_MONEY_CONSUME_NEW_CODE[] = {
	0x90,              // nop (替换 sub eax, edi)
	0x90,              // nop (额外填充)
	0x89, 0x7C, 0x24, 0x24,  // mov [esp+24], edi
	0xE9               // jmp返回
};

// 分配内存
BOOL CRedAlertTrainerDlg::AllocateMemoryForNoMoneyConsume()
{
	if (m_bNoMoneyConsumeMemoryAllocated && m_dwNoMoneyConsumeAllocated)
	{
		return TRUE;
	}

	AddLog(_T("[不消化金钱] 正在分配内存..."));

	m_dwNoMoneyConsumeAllocated = (DWORD_PTR)VirtualAllocEx(
		m_hProcess,
		NULL,
		2048,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE
	);

	if (!m_dwNoMoneyConsumeAllocated)
	{
		AddLog(_T("[不消化金钱] 分配内存失败，错误码: %d"), GetLastError());
		return FALSE;
	}

	AddLog(_T("[不消化金钱] 内存分配成功: 0x%08X"), m_dwNoMoneyConsumeAllocated);
	m_bNoMoneyConsumeMemoryAllocated = TRUE;
	return TRUE;
}

// 写入自定义代码
BOOL CRedAlertTrainerDlg::WriteCustomCodeForNoMoneyConsume()
{
	if (!m_bNoMoneyConsumeMemoryAllocated)
		return FALSE;

	// 计算返回地址：原地址 + 6 (原始指令6字节)
	DWORD_PTR returnAddress = m_dwNoMoneyConsumeAddress + 6;

	// 计算JMP位置和偏移
	DWORD_PTR jmpPosition = m_dwNoMoneyConsumeAllocated + sizeof(NO_MONEY_CONSUME_NEW_CODE);
	DWORD jmpOffset = (DWORD)(returnAddress - (jmpPosition + 4));

	const size_t codeSize = sizeof(NO_MONEY_CONSUME_NEW_CODE) + 4;
	BYTE* fullCode = new BYTE[codeSize];

	memcpy(fullCode, NO_MONEY_CONSUME_NEW_CODE, sizeof(NO_MONEY_CONSUME_NEW_CODE));

	// 设置JMP偏移
	size_t jmpPos = sizeof(NO_MONEY_CONSUME_NEW_CODE) - 1;
	fullCode[jmpPos] = 0xE9;
	fullCode[sizeof(NO_MONEY_CONSUME_NEW_CODE)] = (BYTE)(jmpOffset & 0xFF);
	fullCode[sizeof(NO_MONEY_CONSUME_NEW_CODE) + 1] = (BYTE)((jmpOffset >> 8) & 0xFF);
	fullCode[sizeof(NO_MONEY_CONSUME_NEW_CODE) + 2] = (BYTE)((jmpOffset >> 16) & 0xFF);
	fullCode[sizeof(NO_MONEY_CONSUME_NEW_CODE) + 3] = (BYTE)((jmpOffset >> 24) & 0xFF);

	AddLog(_T("[不消化金钱] 分配地址: 0x%08X"), m_dwNoMoneyConsumeAllocated);
	AddLog(_T("[不消化金钱] 目标地址: 0x%08X"), m_dwNoMoneyConsumeAddress);
	AddLog(_T("[不消化金钱] 返回地址: 0x%08X (原地址+6)"), returnAddress);
	AddLog(_T("[不消化金钱] JMP位置: 0x%08X"), jmpPosition);
	AddLog(_T("[不消化金钱] JMP偏移: 0x%08X"), jmpOffset);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwNoMoneyConsumeAllocated,
		fullCode, codeSize, &bytesWritten);

	if (bResult && bytesWritten == codeSize)
	{
		CString strCode;
		for (size_t i = 0; i < codeSize; i++)
		{
			strCode.AppendFormat(_T("%02X "), fullCode[i]);
		}
		AddLog(_T("[不消化金钱] 写入代码: %s"), strCode);
		AddLog(_T("[不消化金钱] 自定义代码写入成功"));
	}
	else
	{
		AddLog(_T("[不消化金钱] 写入失败，错误码: %d"), GetLastError());
	}

	delete[] fullCode;
	return (bResult && bytesWritten == codeSize);
}

// 安装Hook
BOOL CRedAlertTrainerDlg::InstallHookForNoMoneyConsume()
{
	AddLog(_T("[不消化金钱] 正在安装Hook..."));

	// 原始指令6字节，JMP指令5字节，需要1个NOP填充保持6字节
	DWORD jmpOffset = (DWORD)(m_dwNoMoneyConsumeAllocated - (m_dwNoMoneyConsumeAddress + 5));

	// 6字节指令: JMP(5字节) + NOP(1字节)
	BYTE jmpInstruction[6] = { 0xE9 };
	jmpInstruction[1] = (BYTE)(jmpOffset & 0xFF);
	jmpInstruction[2] = (BYTE)((jmpOffset >> 8) & 0xFF);
	jmpInstruction[3] = (BYTE)((jmpOffset >> 16) & 0xFF);
	jmpInstruction[4] = (BYTE)((jmpOffset >> 24) & 0xFF);
	jmpInstruction[5] = 0x90;  // NOP填充，保持6字节

	AddLog(_T("[不消化金钱] JMP从 0x%08X 到 0x%08X"),
		m_dwNoMoneyConsumeAddress, m_dwNoMoneyConsumeAllocated);
	AddLog(_T("[不消化金钱] JMP偏移: 0x%08X"), jmpOffset);
	AddLog(_T("[不消化金钱] JMP指令: %02X %02X %02X %02X %02X %02X"),
		jmpInstruction[0], jmpInstruction[1], jmpInstruction[2],
		jmpInstruction[3], jmpInstruction[4], jmpInstruction[5]);

	DWORD dwOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)m_dwNoMoneyConsumeAddress, 6,
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwNoMoneyConsumeAddress,
		jmpInstruction, 6, &bytesWritten);

	VirtualProtectEx(m_hProcess, (LPVOID)m_dwNoMoneyConsumeAddress, 6, dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == 6)
	{
		AddLog(_T("[不消化金钱] Hook安装成功！"));
		return TRUE;
	}
	else
	{
		AddLog(_T("[不消化金钱] Hook安装失败，错误码: %d"), GetLastError());
		return FALSE;
	}
}

// 释放内存
void CRedAlertTrainerDlg::FreeNoMoneyConsumeMemory()
{
	if (m_bNoMoneyConsumeMemoryAllocated && m_dwNoMoneyConsumeAllocated)
	{
		VirtualFreeEx(m_hProcess, (LPVOID)m_dwNoMoneyConsumeAllocated, 0, MEM_RELEASE);
		AddLog(_T("[不消化金钱] 内存已释放"));
		m_bNoMoneyConsumeMemoryAllocated = FALSE;
		m_dwNoMoneyConsumeAllocated = 0;
	}
}

// 启用不消化金钱
void CRedAlertTrainerDlg::EnableNoMoneyConsume()
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("[不消化金钱] 错误: 未附加进程"));
		return;
	}

	if (m_bNoMoneyConsumeEnabled)
	{
		AddLog(_T("[不消化金钱] 已经启用"));
		return;
	}

	AddLog(_T("[不消化金钱] 正在启用..."));

	// 获取目标地址
	if (m_dwNoMoneyConsumeAddress == 0)
	{
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
		{
			m_dwNoMoneyConsumeAddress = dwModuleBase + NO_MONEY_CONSUME_OFFSET;
			AddLog(_T("[不消化金钱] 目标地址: 0x%08X"), m_dwNoMoneyConsumeAddress);
		}
		else
		{
			AddLog(_T("[不消化金钱] 无法获取模块基址"));
			return;
		}
	}

	// 读取当前字节码
	BYTE currentBytes[6] = { 0 };
	SIZE_T bytesRead = 0;
	if (ReadProcessMemory(m_hProcess, (LPCVOID)m_dwNoMoneyConsumeAddress, currentBytes, 6, &bytesRead))
	{
		AddLog(_T("[不消化金钱] 当前字节码: %02X %02X %02X %02X %02X %02X"),
			currentBytes[0], currentBytes[1], currentBytes[2],
			currentBytes[3], currentBytes[4], currentBytes[5]);
	}

	// 1. 分配内存
	if (!AllocateMemoryForNoMoneyConsume())
		return;

	// 2. 写入自定义代码
	if (!WriteCustomCodeForNoMoneyConsume())
	{
		FreeNoMoneyConsumeMemory();
		return;
	}

	// 3. 安装Hook
	if (!InstallHookForNoMoneyConsume())
	{
		FreeNoMoneyConsumeMemory();
		return;
	}

	m_bNoMoneyConsumeEnabled = TRUE;
	AddLog(_T("[不消化金钱] 成功启用！购买物品不消耗金钱"));
}

// 禁用不消化金钱
void CRedAlertTrainerDlg::DisableNoMoneyConsume()
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("[不消化金钱] 错误: 未附加进程"));
		return;
	}

	if (!m_bNoMoneyConsumeEnabled)
	{
		AddLog(_T("[不消化金钱] 已经禁用"));
		return;
	}

	AddLog(_T("[不消化金钱] 正在禁用..."));

	if (m_dwNoMoneyConsumeAddress == 0)
	{
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
			m_dwNoMoneyConsumeAddress = dwModuleBase + NO_MONEY_CONSUME_OFFSET;
	}

	// 恢复原始字节码 (6字节)
	DWORD dwOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)m_dwNoMoneyConsumeAddress, sizeof(NO_MONEY_CONSUME_ORIGINAL_BYTES),
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwNoMoneyConsumeAddress,
		NO_MONEY_CONSUME_ORIGINAL_BYTES, sizeof(NO_MONEY_CONSUME_ORIGINAL_BYTES), &bytesWritten);

	VirtualProtectEx(m_hProcess, (LPVOID)m_dwNoMoneyConsumeAddress, sizeof(NO_MONEY_CONSUME_ORIGINAL_BYTES),
		dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == sizeof(NO_MONEY_CONSUME_ORIGINAL_BYTES))
	{
		AddLog(_T("[不消化金钱] 成功禁用！恢复正常金钱消耗"));
		m_bNoMoneyConsumeEnabled = FALSE;
		FreeNoMoneyConsumeMemory();
	}
	else
	{
		AddLog(_T("[不消化金钱] 恢复失败，错误码: %d"), GetLastError());
	}
}

// CheckBox点击响应
void CRedAlertTrainerDlg::OnBnClickedCheckNoMoneyConsume()
{
	int nCheck = m_checkNoMoneyConsume.GetCheck();

	if (!m_bAttached || !m_hProcess)
	{
		AddLog(_T("[不消化金钱] 错误: 请先附加进程"));
		m_checkNoMoneyConsume.SetCheck(m_bNoMoneyConsumeEnabled ? BST_CHECKED : BST_UNCHECKED);
		return;
	}

	if (nCheck == BST_CHECKED)
	{
		EnableNoMoneyConsume();
	}
	else
	{
		DisableNoMoneyConsume();
	}
}

// 原始字节码: add ecx,eax; mov [esi+000052D4],ecx (8字节)
const BYTE NO_POWER_CONSUME_ORIGINAL_BYTES[] = {
	0x03, 0xC8,                          // add ecx, eax
	0x89, 0x8E, 0xD4, 0x52, 0x00, 0x00   // mov [esi+000052D4], ecx
};

// 自定义代码: nop; mov [esi+000052D4],ecx (8字节)
// 将 add ecx,eax 替换为 nop，不消耗电力
const BYTE NO_POWER_CONSUME_NEW_CODE[] = {
	0x90,              // nop (替换 add ecx, eax)
	0x90,              // nop (额外填充)
	0x89, 0x8E, 0xD4, 0x52, 0x00, 0x00,  // mov [esi+000052D4], ecx
	0xE9               // jmp返回
};

// ==================== 不消耗电力功能 ====================

// 分配内存
BOOL CRedAlertTrainerDlg::AllocateMemoryForNoPowerConsume()
{
	if (m_bNoPowerConsumeMemoryAllocated && m_dwNoPowerConsumeAllocated)
	{
		return TRUE;
	}

	AddLog(_T("[不消耗电力] 正在分配内存..."));

	m_dwNoPowerConsumeAllocated = (DWORD_PTR)VirtualAllocEx(
		m_hProcess,
		NULL,
		2048,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE
	);

	if (!m_dwNoPowerConsumeAllocated)
	{
		AddLog(_T("[不消耗电力] 分配内存失败，错误码: %d"), GetLastError());
		return FALSE;
	}

	AddLog(_T("[不消耗电力] 内存分配成功: 0x%08X"), m_dwNoPowerConsumeAllocated);
	m_bNoPowerConsumeMemoryAllocated = TRUE;
	return TRUE;
}

// 写入自定义代码
BOOL CRedAlertTrainerDlg::WriteCustomCodeForNoPowerConsume()
{
	if (!m_bNoPowerConsumeMemoryAllocated)
		return FALSE;

	// 计算返回地址：原地址 + 8 (原始指令8字节)
	DWORD_PTR returnAddress = m_dwNoPowerConsumeAddress + 8;

	// 计算JMP位置和偏移
	DWORD_PTR jmpPosition = m_dwNoPowerConsumeAllocated + sizeof(NO_POWER_CONSUME_NEW_CODE);
	DWORD jmpOffset = (DWORD)(returnAddress - (jmpPosition + 4));

	const size_t codeSize = sizeof(NO_POWER_CONSUME_NEW_CODE) + 4;
	BYTE* fullCode = new BYTE[codeSize];

	memcpy(fullCode, NO_POWER_CONSUME_NEW_CODE, sizeof(NO_POWER_CONSUME_NEW_CODE));

	// 设置JMP偏移
	size_t jmpPos = sizeof(NO_POWER_CONSUME_NEW_CODE) - 1;
	fullCode[jmpPos] = 0xE9;
	fullCode[sizeof(NO_POWER_CONSUME_NEW_CODE)] = (BYTE)(jmpOffset & 0xFF);
	fullCode[sizeof(NO_POWER_CONSUME_NEW_CODE) + 1] = (BYTE)((jmpOffset >> 8) & 0xFF);
	fullCode[sizeof(NO_POWER_CONSUME_NEW_CODE) + 2] = (BYTE)((jmpOffset >> 16) & 0xFF);
	fullCode[sizeof(NO_POWER_CONSUME_NEW_CODE) + 3] = (BYTE)((jmpOffset >> 24) & 0xFF);

	AddLog(_T("[不消耗电力] 分配地址: 0x%08X"), m_dwNoPowerConsumeAllocated);
	AddLog(_T("[不消耗电力] 目标地址: 0x%08X"), m_dwNoPowerConsumeAddress);
	AddLog(_T("[不消耗电力] 返回地址: 0x%08X (原地址+8)"), returnAddress);
	AddLog(_T("[不消耗电力] JMP位置: 0x%08X"), jmpPosition);
	AddLog(_T("[不消耗电力] JMP偏移: 0x%08X"), jmpOffset);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwNoPowerConsumeAllocated,
		fullCode, codeSize, &bytesWritten);

	if (bResult && bytesWritten == codeSize)
	{
		CString strCode;
		for (size_t i = 0; i < codeSize; i++)
		{
			strCode.AppendFormat(_T("%02X "), fullCode[i]);
		}
		AddLog(_T("[不消耗电力] 写入代码: %s"), strCode);
		AddLog(_T("[不消耗电力] 自定义代码写入成功"));
	}
	else
	{
		AddLog(_T("[不消耗电力] 写入失败，错误码: %d"), GetLastError());
	}

	delete[] fullCode;
	return (bResult && bytesWritten == codeSize);
}

// 安装Hook
BOOL CRedAlertTrainerDlg::InstallHookForNoPowerConsume()
{
	AddLog(_T("[不消耗电力] 正在安装Hook..."));

	// 原始指令8字节，JMP指令5字节，需要3个NOP填充保持8字节
	DWORD jmpOffset = (DWORD)(m_dwNoPowerConsumeAllocated - (m_dwNoPowerConsumeAddress + 5));

	// 8字节指令: JMP(5字节) + NOP(3字节)
	BYTE jmpInstruction[8] = { 0xE9 };
	jmpInstruction[1] = (BYTE)(jmpOffset & 0xFF);
	jmpInstruction[2] = (BYTE)((jmpOffset >> 8) & 0xFF);
	jmpInstruction[3] = (BYTE)((jmpOffset >> 16) & 0xFF);
	jmpInstruction[4] = (BYTE)((jmpOffset >> 24) & 0xFF);
	jmpInstruction[5] = 0x90;  // NOP填充
	jmpInstruction[6] = 0x90;  // NOP填充
	jmpInstruction[7] = 0x90;  // NOP填充

	AddLog(_T("[不消耗电力] JMP从 0x%08X 到 0x%08X"),
		m_dwNoPowerConsumeAddress, m_dwNoPowerConsumeAllocated);
	AddLog(_T("[不消耗电力] JMP偏移: 0x%08X"), jmpOffset);
	AddLog(_T("[不消耗电力] JMP指令: %02X %02X %02X %02X %02X %02X %02X %02X"),
		jmpInstruction[0], jmpInstruction[1], jmpInstruction[2],
		jmpInstruction[3], jmpInstruction[4], jmpInstruction[5],
		jmpInstruction[6], jmpInstruction[7]);

	DWORD dwOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)m_dwNoPowerConsumeAddress, 8,
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwNoPowerConsumeAddress,
		jmpInstruction, 8, &bytesWritten);

	VirtualProtectEx(m_hProcess, (LPVOID)m_dwNoPowerConsumeAddress, 8, dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == 8)
	{
		AddLog(_T("[不消耗电力] Hook安装成功！"));
		return TRUE;
	}
	else
	{
		AddLog(_T("[不消耗电力] Hook安装失败，错误码: %d"), GetLastError());
		return FALSE;
	}
}

// 释放内存
void CRedAlertTrainerDlg::FreeNoPowerConsumeMemory()
{
	if (m_bNoPowerConsumeMemoryAllocated && m_dwNoPowerConsumeAllocated)
	{
		VirtualFreeEx(m_hProcess, (LPVOID)m_dwNoPowerConsumeAllocated, 0, MEM_RELEASE);
		AddLog(_T("[不消耗电力] 内存已释放"));
		m_bNoPowerConsumeMemoryAllocated = FALSE;
		m_dwNoPowerConsumeAllocated = 0;
	}
}

// 启用不消耗电力
void CRedAlertTrainerDlg::EnableNoPowerConsume()
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("[不消耗电力] 错误: 未附加进程"));
		return;
	}

	if (m_bNoPowerConsumeEnabled)
	{
		AddLog(_T("[不消耗电力] 已经启用"));
		return;
	}

	AddLog(_T("[不消耗电力] 正在启用..."));

	// 获取目标地址
	if (m_dwNoPowerConsumeAddress == 0)
	{
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
		{
			m_dwNoPowerConsumeAddress = dwModuleBase + NO_POWER_CONSUME_OFFSET;
			AddLog(_T("[不消耗电力] 目标地址: 0x%08X"), m_dwNoPowerConsumeAddress);
		}
		else
		{
			AddLog(_T("[不消耗电力] 无法获取模块基址"));
			return;
		}
	}

	// 读取当前字节码
	BYTE currentBytes[8] = { 0 };
	SIZE_T bytesRead = 0;
	if (ReadProcessMemory(m_hProcess, (LPCVOID)m_dwNoPowerConsumeAddress, currentBytes, 8, &bytesRead))
	{
		AddLog(_T("[不消耗电力] 当前字节码: %02X %02X %02X %02X %02X %02X %02X %02X"),
			currentBytes[0], currentBytes[1], currentBytes[2], currentBytes[3],
			currentBytes[4], currentBytes[5], currentBytes[6], currentBytes[7]);
	}

	// 1. 分配内存
	if (!AllocateMemoryForNoPowerConsume())
		return;

	// 2. 写入自定义代码
	if (!WriteCustomCodeForNoPowerConsume())
	{
		FreeNoPowerConsumeMemory();
		return;
	}

	// 3. 安装Hook
	if (!InstallHookForNoPowerConsume())
	{
		FreeNoPowerConsumeMemory();
		return;
	}

	m_bNoPowerConsumeEnabled = TRUE;
	AddLog(_T("[不消耗电力] 成功启用！使用电力时不消耗电力"));
}

// 禁用不消耗电力
void CRedAlertTrainerDlg::DisableNoPowerConsume()
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("[不消耗电力] 错误: 未附加进程"));
		return;
	}

	if (!m_bNoPowerConsumeEnabled)
	{
		AddLog(_T("[不消耗电力] 已经禁用"));
		return;
	}

	AddLog(_T("[不消耗电力] 正在禁用..."));

	if (m_dwNoPowerConsumeAddress == 0)
	{
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
			m_dwNoPowerConsumeAddress = dwModuleBase + NO_POWER_CONSUME_OFFSET;
	}

	// 恢复原始字节码 (8字节)
	DWORD dwOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)m_dwNoPowerConsumeAddress, sizeof(NO_POWER_CONSUME_ORIGINAL_BYTES),
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwNoPowerConsumeAddress,
		NO_POWER_CONSUME_ORIGINAL_BYTES, sizeof(NO_POWER_CONSUME_ORIGINAL_BYTES), &bytesWritten);

	VirtualProtectEx(m_hProcess, (LPVOID)m_dwNoPowerConsumeAddress, sizeof(NO_POWER_CONSUME_ORIGINAL_BYTES),
		dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == sizeof(NO_POWER_CONSUME_ORIGINAL_BYTES))
	{
		AddLog(_T("[不消耗电力] 成功禁用！恢复正常电力消耗"));
		m_bNoPowerConsumeEnabled = FALSE;
		FreeNoPowerConsumeMemory();
	}
	else
	{
		AddLog(_T("[不消耗电力] 恢复失败，错误码: %d"), GetLastError());
	}
}

// CheckBox点击响应
void CRedAlertTrainerDlg::OnBnClickedCheckNoPowerConsume()
{
	int nCheck = m_checkNoPowerConsume.GetCheck();

	if (!m_bAttached || !m_hProcess)
	{
		AddLog(_T("[不消耗电力] 错误: 请先附加进程"));
		m_checkNoPowerConsume.SetCheck(m_bNoPowerConsumeEnabled ? BST_CHECKED : BST_UNCHECKED);
		return;
	}

	if (nCheck == BST_CHECKED)
	{
		EnableNoPowerConsume();
	}
	else
	{
		DisableNoPowerConsume();
	}
}

// 原始字节码: mov eax,[game.exe+640D2C] (5字节)
const BYTE FAST_BUILD_ORIGINAL_BYTES[] = {
	0xA1, 0x2C, 0x0D, 0xA4, 0x00  // mov eax, [game.exe+640D2C]
};

// 自定义代码: nop (5字节)
const BYTE FAST_BUILD_NEW_CODE[] = {
	0x90, 0x90, 0x90, 0x90, 0x90,  // 5个nop
	0xE9                          // jmp返回
};

// ==================== 快速建筑功能 ====================

// 分配内存
BOOL CRedAlertTrainerDlg::AllocateMemoryForFastBuild()
{
	if (m_bFastBuildMemoryAllocated && m_dwFastBuildAllocated)
	{
		return TRUE;
	}

	AddLog(_T("[快速建筑] 正在分配内存..."));

	m_dwFastBuildAllocated = (DWORD_PTR)VirtualAllocEx(
		m_hProcess,
		NULL,
		2048,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE
	);

	if (!m_dwFastBuildAllocated)
	{
		AddLog(_T("[快速建筑] 分配内存失败，错误码: %d"), GetLastError());
		return FALSE;
	}

	AddLog(_T("[快速建筑] 内存分配成功: 0x%08X"), m_dwFastBuildAllocated);
	m_bFastBuildMemoryAllocated = TRUE;
	return TRUE;
}

// 写入自定义代码
BOOL CRedAlertTrainerDlg::WriteCustomCodeForFastBuild()
{
	if (!m_bFastBuildMemoryAllocated)
		return FALSE;

	// 计算返回地址：原地址 + 5 (原始指令5字节)
	DWORD_PTR returnAddress = m_dwFastBuildAddress + 5;

	// 计算JMP位置和偏移
	DWORD_PTR jmpPosition = m_dwFastBuildAllocated + sizeof(FAST_BUILD_NEW_CODE);
	DWORD jmpOffset = (DWORD)(returnAddress - (jmpPosition + 4));

	const size_t codeSize = sizeof(FAST_BUILD_NEW_CODE) + 4;
	BYTE* fullCode = new BYTE[codeSize];

	memcpy(fullCode, FAST_BUILD_NEW_CODE, sizeof(FAST_BUILD_NEW_CODE));

	// 设置JMP偏移
	size_t jmpPos = sizeof(FAST_BUILD_NEW_CODE) - 1;
	fullCode[jmpPos] = 0xE9;
	fullCode[sizeof(FAST_BUILD_NEW_CODE)] = (BYTE)(jmpOffset & 0xFF);
	fullCode[sizeof(FAST_BUILD_NEW_CODE) + 1] = (BYTE)((jmpOffset >> 8) & 0xFF);
	fullCode[sizeof(FAST_BUILD_NEW_CODE) + 2] = (BYTE)((jmpOffset >> 16) & 0xFF);
	fullCode[sizeof(FAST_BUILD_NEW_CODE) + 3] = (BYTE)((jmpOffset >> 24) & 0xFF);

	AddLog(_T("[快速建筑] 分配地址: 0x%08X"), m_dwFastBuildAllocated);
	AddLog(_T("[快速建筑] 目标地址: 0x%08X"), m_dwFastBuildAddress);
	AddLog(_T("[快速建筑] 返回地址: 0x%08X (原地址+5)"), returnAddress);
	AddLog(_T("[快速建筑] JMP位置: 0x%08X"), jmpPosition);
	AddLog(_T("[快速建筑] JMP偏移: 0x%08X"), jmpOffset);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwFastBuildAllocated,
		fullCode, codeSize, &bytesWritten);

	if (bResult && bytesWritten == codeSize)
	{
		CString strCode;
		for (size_t i = 0; i < codeSize; i++)
		{
			strCode.AppendFormat(_T("%02X "), fullCode[i]);
		}
		AddLog(_T("[快速建筑] 写入代码: %s"), strCode);
		AddLog(_T("[快速建筑] 自定义代码写入成功"));
	}
	else
	{
		AddLog(_T("[快速建筑] 写入失败，错误码: %d"), GetLastError());
	}

	delete[] fullCode;
	return (bResult && bytesWritten == codeSize);
}

// 安装Hook
BOOL CRedAlertTrainerDlg::InstallHookForFastBuild()
{
	AddLog(_T("[快速建筑] 正在安装Hook..."));

	// 原始指令5字节，JMP指令5字节，长度正好对齐，不需要NOP
	DWORD jmpOffset = (DWORD)(m_dwFastBuildAllocated - (m_dwFastBuildAddress + 5));

	// 5字节JMP指令
	BYTE jmpInstruction[5] = { 0xE9 };
	jmpInstruction[1] = (BYTE)(jmpOffset & 0xFF);
	jmpInstruction[2] = (BYTE)((jmpOffset >> 8) & 0xFF);
	jmpInstruction[3] = (BYTE)((jmpOffset >> 16) & 0xFF);
	jmpInstruction[4] = (BYTE)((jmpOffset >> 24) & 0xFF);

	AddLog(_T("[快速建筑] JMP从 0x%08X 到 0x%08X"),
		m_dwFastBuildAddress, m_dwFastBuildAllocated);
	AddLog(_T("[快速建筑] JMP偏移: 0x%08X"), jmpOffset);
	AddLog(_T("[快速建筑] JMP指令: %02X %02X %02X %02X %02X"),
		jmpInstruction[0], jmpInstruction[1], jmpInstruction[2],
		jmpInstruction[3], jmpInstruction[4]);

	DWORD dwOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)m_dwFastBuildAddress, 5,
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwFastBuildAddress,
		jmpInstruction, 5, &bytesWritten);

	VirtualProtectEx(m_hProcess, (LPVOID)m_dwFastBuildAddress, 5, dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == 5)
	{
		AddLog(_T("[快速建筑] Hook安装成功！"));
		return TRUE;
	}
	else
	{
		AddLog(_T("[快速建筑] Hook安装失败，错误码: %d"), GetLastError());
		return FALSE;
	}
}

// 释放内存
void CRedAlertTrainerDlg::FreeFastBuildMemory()
{
	if (m_bFastBuildMemoryAllocated && m_dwFastBuildAllocated)
	{
		VirtualFreeEx(m_hProcess, (LPVOID)m_dwFastBuildAllocated, 0, MEM_RELEASE);
		AddLog(_T("[快速建筑] 内存已释放"));
		m_bFastBuildMemoryAllocated = FALSE;
		m_dwFastBuildAllocated = 0;
	}
}

// 启用快速建筑
void CRedAlertTrainerDlg::EnableFastBuild()
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("[快速建筑] 错误: 未附加进程"));
		return;
	}

	if (m_bFastBuildEnabled)
	{
		AddLog(_T("[快速建筑] 已经启用"));
		return;
	}

	AddLog(_T("[快速建筑] 正在启用..."));

	// 获取目标地址
	if (m_dwFastBuildAddress == 0)
	{
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
		{
			m_dwFastBuildAddress = dwModuleBase + FAST_BUILD_OFFSET;
			AddLog(_T("[快速建筑] 目标地址: 0x%08X"), m_dwFastBuildAddress);
		}
		else
		{
			AddLog(_T("[快速建筑] 无法获取模块基址"));
			return;
		}
	}

	// 读取当前字节码
	BYTE currentBytes[5] = { 0 };
	SIZE_T bytesRead = 0;
	if (ReadProcessMemory(m_hProcess, (LPCVOID)m_dwFastBuildAddress, currentBytes, 5, &bytesRead))
	{
		AddLog(_T("[快速建筑] 当前字节码: %02X %02X %02X %02X %02X"),
			currentBytes[0], currentBytes[1], currentBytes[2],
			currentBytes[3], currentBytes[4]);
	}

	// 1. 分配内存
	if (!AllocateMemoryForFastBuild())
		return;

	// 2. 写入自定义代码
	if (!WriteCustomCodeForFastBuild())
	{
		FreeFastBuildMemory();
		return;
	}

	// 3. 安装Hook
	if (!InstallHookForFastBuild())
	{
		FreeFastBuildMemory();
		return;
	}

	m_bFastBuildEnabled = TRUE;
	AddLog(_T("[快速建筑] 成功启用！建筑速度大幅提升"));
}

// 禁用快速建筑
void CRedAlertTrainerDlg::DisableFastBuild()
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("[快速建筑] 错误: 未附加进程"));
		return;
	}

	if (!m_bFastBuildEnabled)
	{
		AddLog(_T("[快速建筑] 已经禁用"));
		return;
	}

	AddLog(_T("[快速建筑] 正在禁用..."));

	if (m_dwFastBuildAddress == 0)
	{
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
			m_dwFastBuildAddress = dwModuleBase + FAST_BUILD_OFFSET;
	}

	// 恢复原始字节码 (5字节)
	DWORD dwOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)m_dwFastBuildAddress, sizeof(FAST_BUILD_ORIGINAL_BYTES),
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwFastBuildAddress,
		FAST_BUILD_ORIGINAL_BYTES, sizeof(FAST_BUILD_ORIGINAL_BYTES), &bytesWritten);

	VirtualProtectEx(m_hProcess, (LPVOID)m_dwFastBuildAddress, sizeof(FAST_BUILD_ORIGINAL_BYTES),
		dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == sizeof(FAST_BUILD_ORIGINAL_BYTES))
	{
		AddLog(_T("[快速建筑] 成功禁用！恢复正常建筑速度"));
		m_bFastBuildEnabled = FALSE;
		FreeFastBuildMemory();
	}
	else
	{
		AddLog(_T("[快速建筑] 恢复失败，错误码: %d"), GetLastError());
	}
}

// CheckBox点击响应
void CRedAlertTrainerDlg::OnBnClickedCheckFastBuild()
{
	int nCheck = m_checkFastBuild.GetCheck();

	if (!m_bAttached || !m_hProcess)
	{
		AddLog(_T("[快速建筑] 错误: 请先附加进程"));
		m_checkFastBuild.SetCheck(m_bFastBuildEnabled ? BST_CHECKED : BST_UNCHECKED);
		return;
	}

	if (nCheck == BST_CHECKED)
	{
		EnableFastBuild();
	}
	else
	{
		DisableFastBuild();
	}
}

// 原始字节码 (20字节)
// je game.exe+9BDE6
// mov al,[ebp+0000117D]
// test al,al
// je game.exe+9BDE6
const BYTE FREE_BUILD_ORIGINAL_BYTES[] = {
	0x0F, 0x84, 0xC4, 0x01, 0x00, 0x00,  // je game.exe+9BDE6
	0x8A, 0x85, 0x7D, 0x11, 0x00, 0x00,  // mov al, [ebp+0000117D]
	0x84, 0xC0,                          // test al, al
	0x0F, 0x84, 0xB6, 0x01, 0x00, 0x00   // je game.exe+9BDE6
};

// 自定义代码: nop; mov al,[ebp+0000117D]; test al,al; nop (20字节)
// 将条件跳转替换为nop，实现随意建筑
const BYTE FREE_BUILD_NEW_CODE[] = {
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90,  // 6个nop (替换第一个je)
	0x8A, 0x85, 0x7D, 0x11, 0x00, 0x00,  // mov al, [ebp+0000117D]
	0x84, 0xC0,                          // test al, al
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90,  // 6个nop (替换第二个je)
	0xE9                                 // jmp返回
};

// ==================== 随意建筑功能 ====================

// 分配内存
BOOL CRedAlertTrainerDlg::AllocateMemoryForFreeBuild()
{
	if (m_bFreeBuildMemoryAllocated && m_dwFreeBuildAllocated)
	{
		return TRUE;
	}

	AddLog(_T("[随意建筑] 正在分配内存..."));

	m_dwFreeBuildAllocated = (DWORD_PTR)VirtualAllocEx(
		m_hProcess,
		NULL,
		2048,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE
	);

	if (!m_dwFreeBuildAllocated)
	{
		AddLog(_T("[随意建筑] 分配内存失败，错误码: %d"), GetLastError());
		return FALSE;
	}

	AddLog(_T("[随意建筑] 内存分配成功: 0x%08X"), m_dwFreeBuildAllocated);
	m_bFreeBuildMemoryAllocated = TRUE;
	return TRUE;
}

// 写入自定义代码
BOOL CRedAlertTrainerDlg::WriteCustomCodeForFreeBuild()
{
	if (!m_bFreeBuildMemoryAllocated)
		return FALSE;

	// 计算返回地址：原地址 + 20 (原始指令20字节)
	DWORD_PTR returnAddress = m_dwFreeBuildAddress + 20;

	// 计算JMP位置和偏移
	DWORD_PTR jmpPosition = m_dwFreeBuildAllocated + sizeof(FREE_BUILD_NEW_CODE);
	DWORD jmpOffset = (DWORD)(returnAddress - (jmpPosition + 4));

	const size_t codeSize = sizeof(FREE_BUILD_NEW_CODE) + 4;
	BYTE* fullCode = new BYTE[codeSize];

	memcpy(fullCode, FREE_BUILD_NEW_CODE, sizeof(FREE_BUILD_NEW_CODE));

	// 设置JMP偏移
	size_t jmpPos = sizeof(FREE_BUILD_NEW_CODE) - 1;
	fullCode[jmpPos] = 0xE9;
	fullCode[sizeof(FREE_BUILD_NEW_CODE)] = (BYTE)(jmpOffset & 0xFF);
	fullCode[sizeof(FREE_BUILD_NEW_CODE) + 1] = (BYTE)((jmpOffset >> 8) & 0xFF);
	fullCode[sizeof(FREE_BUILD_NEW_CODE) + 2] = (BYTE)((jmpOffset >> 16) & 0xFF);
	fullCode[sizeof(FREE_BUILD_NEW_CODE) + 3] = (BYTE)((jmpOffset >> 24) & 0xFF);

	AddLog(_T("[随意建筑] 分配地址: 0x%08X"), m_dwFreeBuildAllocated);
	AddLog(_T("[随意建筑] 目标地址: 0x%08X"), m_dwFreeBuildAddress);
	AddLog(_T("[随意建筑] 返回地址: 0x%08X (原地址+20)"), returnAddress);
	AddLog(_T("[随意建筑] JMP位置: 0x%08X"), jmpPosition);
	AddLog(_T("[随意建筑] JMP偏移: 0x%08X"), jmpOffset);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwFreeBuildAllocated,
		fullCode, codeSize, &bytesWritten);

	if (bResult && bytesWritten == codeSize)
	{
		CString strCode;
		for (size_t i = 0; i < codeSize; i++)
		{
			strCode.AppendFormat(_T("%02X "), fullCode[i]);
		}
		AddLog(_T("[随意建筑] 写入代码: %s"), strCode);
		AddLog(_T("[随意建筑] 自定义代码写入成功"));
	}
	else
	{
		AddLog(_T("[随意建筑] 写入失败，错误码: %d"), GetLastError());
	}

	delete[] fullCode;
	return (bResult && bytesWritten == codeSize);
}

// 安装Hook
BOOL CRedAlertTrainerDlg::InstallHookForFreeBuild()
{
	AddLog(_T("[随意建筑] 正在安装Hook..."));

	// 原始指令20字节，JMP指令5字节，需要15个NOP填充保持20字节
	DWORD jmpOffset = (DWORD)(m_dwFreeBuildAllocated - (m_dwFreeBuildAddress + 5));

	// 20字节指令: JMP(5字节) + NOP(15字节)
	BYTE jmpInstruction[20] = { 0xE9 };
	jmpInstruction[1] = (BYTE)(jmpOffset & 0xFF);
	jmpInstruction[2] = (BYTE)((jmpOffset >> 8) & 0xFF);
	jmpInstruction[3] = (BYTE)((jmpOffset >> 16) & 0xFF);
	jmpInstruction[4] = (BYTE)((jmpOffset >> 24) & 0xFF);

	// 填充NOP
	for (int i = 5; i < 20; i++)
	{
		jmpInstruction[i] = 0x90;
	}

	AddLog(_T("[随意建筑] JMP从 0x%08X 到 0x%08X"),
		m_dwFreeBuildAddress, m_dwFreeBuildAllocated);
	AddLog(_T("[随意建筑] JMP偏移: 0x%08X"), jmpOffset);
	AddLog(_T("[随意建筑] JMP指令: %02X %02X %02X %02X %02X + 15个NOP"),
		jmpInstruction[0], jmpInstruction[1], jmpInstruction[2],
		jmpInstruction[3], jmpInstruction[4]);

	DWORD dwOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)m_dwFreeBuildAddress, 20,
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwFreeBuildAddress,
		jmpInstruction, 20, &bytesWritten);

	VirtualProtectEx(m_hProcess, (LPVOID)m_dwFreeBuildAddress, 20, dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == 20)
	{
		AddLog(_T("[随意建筑] Hook安装成功！"));
		return TRUE;
	}
	else
	{
		AddLog(_T("[随意建筑] Hook安装失败，错误码: %d"), GetLastError());
		return FALSE;
	}
}

// 释放内存
void CRedAlertTrainerDlg::FreeFreeBuildMemory()
{
	if (m_bFreeBuildMemoryAllocated && m_dwFreeBuildAllocated)
	{
		VirtualFreeEx(m_hProcess, (LPVOID)m_dwFreeBuildAllocated, 0, MEM_RELEASE);
		AddLog(_T("[随意建筑] 内存已释放"));
		m_bFreeBuildMemoryAllocated = FALSE;
		m_dwFreeBuildAllocated = 0;
	}
}

// 启用随意建筑
void CRedAlertTrainerDlg::EnableFreeBuild()
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("[随意建筑] 错误: 未附加进程"));
		return;
	}

	if (m_bFreeBuildEnabled)
	{
		AddLog(_T("[随意建筑] 已经启用"));
		return;
	}

	AddLog(_T("[随意建筑] 正在启用..."));

	// 获取目标地址
	if (m_dwFreeBuildAddress == 0)
	{
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
		{
			m_dwFreeBuildAddress = dwModuleBase + FREE_BUILD_OFFSET;
			AddLog(_T("[随意建筑] 目标地址: 0x%08X"), m_dwFreeBuildAddress);
		}
		else
		{
			AddLog(_T("[随意建筑] 无法获取模块基址"));
			return;
		}
	}

	// 读取当前字节码
	BYTE currentBytes[20] = { 0 };
	SIZE_T bytesRead = 0;
	if (ReadProcessMemory(m_hProcess, (LPCVOID)m_dwFreeBuildAddress, currentBytes, 20, &bytesRead))
	{
		CString strCode;
		for (int i = 0; i < 20; i++)
		{
			strCode.AppendFormat(_T("%02X "), currentBytes[i]);
			if ((i + 1) % 16 == 0) strCode.AppendFormat(_T("\n        "));
		}
		AddLog(_T("[随意建筑] 当前字节码:\n        %s"), strCode);
	}

	// 1. 分配内存
	if (!AllocateMemoryForFreeBuild())
		return;

	// 2. 写入自定义代码
	if (!WriteCustomCodeForFreeBuild())
	{
		FreeFreeBuildMemory();
		return;
	}

	// 3. 安装Hook
	if (!InstallHookForFreeBuild())
	{
		FreeFreeBuildMemory();
		return;
	}

	m_bFreeBuildEnabled = TRUE;
	AddLog(_T("[随意建筑] 成功启用！可以随意建筑"));
}

// 禁用随意建筑
void CRedAlertTrainerDlg::DisableFreeBuild()
{
	if (!m_hProcess || !m_bAttached)
	{
		AddLog(_T("[随意建筑] 错误: 未附加进程"));
		return;
	}

	if (!m_bFreeBuildEnabled)
	{
		AddLog(_T("[随意建筑] 已经禁用"));
		return;
	}

	AddLog(_T("[随意建筑] 正在禁用..."));

	if (m_dwFreeBuildAddress == 0)
	{
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
			m_dwFreeBuildAddress = dwModuleBase + FREE_BUILD_OFFSET;
	}

	// 恢复原始字节码 (20字节)
	DWORD dwOldProtect = 0;
	VirtualProtectEx(m_hProcess, (LPVOID)m_dwFreeBuildAddress, sizeof(FREE_BUILD_ORIGINAL_BYTES),
		PAGE_EXECUTE_READWRITE, &dwOldProtect);

	SIZE_T bytesWritten = 0;
	BOOL bResult = WriteProcessMemory(m_hProcess, (LPVOID)m_dwFreeBuildAddress,
		FREE_BUILD_ORIGINAL_BYTES, sizeof(FREE_BUILD_ORIGINAL_BYTES), &bytesWritten);

	VirtualProtectEx(m_hProcess, (LPVOID)m_dwFreeBuildAddress, sizeof(FREE_BUILD_ORIGINAL_BYTES),
		dwOldProtect, &dwOldProtect);

	if (bResult && bytesWritten == sizeof(FREE_BUILD_ORIGINAL_BYTES))
	{
		AddLog(_T("[随意建筑] 成功禁用！恢复正常建筑限制"));
		m_bFreeBuildEnabled = FALSE;
		FreeFreeBuildMemory();
	}
	else
	{
		AddLog(_T("[随意建筑] 恢复失败，错误码: %d"), GetLastError());
	}
}

// CheckBox点击响应
void CRedAlertTrainerDlg::OnBnClickedCheckFreeBuild()
{
	int nCheck = m_checkFreeBuild.GetCheck();

	if (!m_bAttached || !m_hProcess)
	{
		AddLog(_T("[随意建筑] 错误: 请先附加进程"));
		m_checkFreeBuild.SetCheck(m_bFreeBuildEnabled ? BST_CHECKED : BST_UNCHECKED);
		return;
	}

	if (nCheck == BST_CHECKED)
	{
		EnableFreeBuild();
	}
	else
	{
		DisableFreeBuild();
	}
}