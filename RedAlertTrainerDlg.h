
// RedAlertTrainerDlg.h: 头文件
//

#pragma once


#define NO_MONEY_CONSUME_OFFSET 0xE53A9   // 不消化金钱偏移地址

#define RED_ALERT_BASE_ADDR    0xA35DB4
#define MONEY_OFFSET           0x24C
#define ELECTRICITY_OFFSET     0x52D0
#define PAYLOAD_OFFSET         0x52D4
#define NO_POWER_CONSUME_OFFSET 0xF2D99   // 不消耗电力偏移地址
#define FAST_BUILD_OFFSET 0xB936A        // 快速建筑偏移地址
#define FREE_BUILD_OFFSET 0x9BC1C        // 随意建筑偏移地址

// CRedAlertTrainerDlg 对话框
class CRedAlertTrainerDlg : public CDialogEx
{
// 构造
public:
	CRedAlertTrainerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REDALERTTRAINER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CStatic m_PictureShow;
	void ShowBackgroundPicture(CString m_JpgPathName);

	HANDLE m_hProcess;           // 进程句柄
	DWORD m_dwProcessId;         // 进程ID
	BOOL m_bAttached;             // 是否已附加进程
	
	CListBox m_listLog;           // 日志列表
	void AddLog(LPCTSTR szFormat, ...);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);  // 定时器消息

	CString m_strCurrentImagePath;            // 保存当前图片路径
	void DrawPictureToControl();              // 新增：实际的绘制函数
	
	CButton m_btnSelectProcess;          // 选择进程按钮
	CString m_strProcessName;            // 当前选择的进程名称
	afx_msg void OnBnClickedSelectprocess();
	afx_msg void OnBnClickedAttachprocess();

	CButton m_btnAttachProcess;   // 附加进程按钮
	void DetachFromProcess();
	BOOL AttachToProcess();
	BOOL FindProcessId(LPCTSTR szProcessName, DWORD& dwProcessId);

	CEdit m_editCurrentMoneyValue;     // 金钱值显示编辑框
	CEdit m_editCurrentElectricityValue;     // 金钱值显示编辑框
	CEdit m_editCurrentPayloaValue;     // 金钱值显示编辑框
	DWORD_PTR GetModuleBaseAddress();
	DWORD_PTR ReadPointerChain(DWORD_PTR baseAddress, DWORD_PTR offset1);
	DWORD ReadValue(DWORD BaseAddr, DWORD offset);
	BOOL  WriteValue(DWORD dwNewValue, DWORD BaseAddr, DWORD offset);

	void ReadBaseValue();

	afx_msg void OnEnChangeCurrentmoney();
	CString m_strLastEditText;

	CButton m_checkNoMoneyConsume;       // 不消化金钱复选框
	BOOL m_bNoMoneyConsumeEnabled;       // 功能开关状态
	DWORD_PTR m_dwNoMoneyConsumeAddress; // 目标地址 game.exe+E53A9
	DWORD_PTR m_dwNoMoneyConsumeAllocated; // 分配的内存地址
	BOOL m_bNoMoneyConsumeMemoryAllocated; // 内存是否已分配

	// 函数声明
	void EnableNoMoneyConsume();          // 启用不消化金钱
	void DisableNoMoneyConsume();         // 禁用不消化金钱
	BOOL AllocateMemoryForNoMoneyConsume(); // 分配内存
	BOOL WriteCustomCodeForNoMoneyConsume(); // 写入自定义代码
	BOOL InstallHookForNoMoneyConsume();    // 安装Hook
	void FreeNoMoneyConsumeMemory();        // 释放内存

	// 在消息映射中添加
	afx_msg void OnBnClickedCheckNoMoneyConsume();

	CButton m_checkNoPowerConsume;       // 不消耗电力复选框
	BOOL m_bNoPowerConsumeEnabled;       // 功能开关状态
	DWORD_PTR m_dwNoPowerConsumeAddress; // 目标地址 game.exe+F2D99
	DWORD_PTR m_dwNoPowerConsumeAllocated; // 分配的内存地址
	BOOL m_bNoPowerConsumeMemoryAllocated; // 内存是否已分配

	// 函数声明
	void EnableNoPowerConsume();          // 启用不消耗电力
	void DisableNoPowerConsume();         // 禁用不消耗电力
	BOOL AllocateMemoryForNoPowerConsume(); // 分配内存
	BOOL WriteCustomCodeForNoPowerConsume(); // 写入自定义代码
	BOOL InstallHookForNoPowerConsume();    // 安装Hook
	void FreeNoPowerConsumeMemory();        // 释放内存

	// 在消息映射中添加
	afx_msg void OnBnClickedCheckNoPowerConsume();

	CButton m_checkFastBuild;            // 快速建筑复选框
	BOOL m_bFastBuildEnabled;            // 功能开关状态
	DWORD_PTR m_dwFastBuildAddress;      // 目标地址 game.exe+B936A
	DWORD_PTR m_dwFastBuildAllocated;    // 分配的内存地址
	BOOL m_bFastBuildMemoryAllocated;    // 内存是否已分配

	// 函数声明
	void EnableFastBuild();              // 启用快速建筑
	void DisableFastBuild();             // 禁用快速建筑
	BOOL AllocateMemoryForFastBuild();   // 分配内存
	BOOL WriteCustomCodeForFastBuild();  // 写入自定义代码
	BOOL InstallHookForFastBuild();      // 安装Hook
	void FreeFastBuildMemory();          // 释放内存

	// 在消息映射中添加
	afx_msg void OnBnClickedCheckFastBuild();

	CButton m_checkFreeBuild;            // 随意建筑复选框
	BOOL m_bFreeBuildEnabled;            // 功能开关状态
	DWORD_PTR m_dwFreeBuildAddress;      // 目标地址 game.exe+9BC1C
	DWORD_PTR m_dwFreeBuildAllocated;    // 分配的内存地址
	BOOL m_bFreeBuildMemoryAllocated;    // 内存是否已分配

	// 函数声明
	void EnableFreeBuild();              // 启用随意建筑
	void DisableFreeBuild();             // 禁用随意建筑
	BOOL AllocateMemoryForFreeBuild();   // 分配内存
	BOOL WriteCustomCodeForFreeBuild();  // 写入自定义代码
	BOOL InstallHookForFreeBuild();      // 安装Hook
	void FreeFreeBuildMemory();          // 释放内存

	// 在消息映射中添加
	afx_msg void OnBnClickedCheckFreeBuild();


};
