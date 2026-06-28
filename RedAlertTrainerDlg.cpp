
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
	
}

BEGIN_MESSAGE_MAP(CRedAlertTrainerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()  // 添加这一行：定时器消息映射
	ON_BN_CLICKED(IDC_SelectProcess, &CRedAlertTrainerDlg::OnBnClickedSelectprocess)
	ON_BN_CLICKED(IDC_AttachProcess, &CRedAlertTrainerDlg::OnBnClickedAttachprocess)

	ON_EN_CHANGE(IDC_CurrentMoney,   &CRedAlertTrainerDlg::OnEnChangeCurrentmoney)
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
		DWORD_PTR dwModuleBase = GetModuleBaseAddress();
		if (dwModuleBase)
		{
			
		}

		DWORD dwCurrenMenoy = ReadValue(0xA35DB4, 0x24C);
		CString strValue;
		strValue.Format(_T("%d"), dwCurrenMenoy);
		m_editCurrentMoneyValue.SetWindowText(strValue);

		DWORD dwCurrentElectricity = ReadValue(0xA35DB4, 0x52D0);
		strValue.Format(_T("%d"), dwCurrentElectricity);
		m_editCurrentElectricityValue.SetWindowText(strValue);

		DWORD dwCurrentPayload = ReadValue(0xA35DB4, 0x52D4);
		strValue.Format(_T("%d"), dwCurrentPayload);
		m_editCurrentPayloaValue.SetWindowText(strValue);
		//// 自动读取一次阳光值
		//OnBnClickedReadSunValue();
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

	AddLog(_T("读取基址[0x%08X] -> 指针1: 0x%08X"), baseAddress, ptr1);

	// 第一次偏移计算
	DWORD_PTR addr1 = ptr1 + offset1;

	// 第二次偏移计算，得到最终阳光地址
	DWORD_PTR finalAddr = addr1;

	AddLog(_T("最终地址: 0x%08X"), finalAddr);

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
			AddLog(_T("当前值: %d"), dwMenoy);
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
			ReadValue(BaseAddr, offset) - dwNewValue + dwNewValue, dwNewValue);

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

