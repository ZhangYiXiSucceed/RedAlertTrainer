
// RedAlertTrainerDlg.h: 头文件
//

#pragma once


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

	CEdit m_editCurrentMoneyValue;     // 阳光值显示编辑框
	DWORD_PTR GetModuleBaseAddress();
	DWORD_PTR ReadPointerChain(DWORD_PTR baseAddress, DWORD_PTR offset1);
};
