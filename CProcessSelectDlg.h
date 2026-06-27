#pragma once
#include <afxdialogex.h>
class CProcessSelectDlg :
    public CDialogEx
{
    DECLARE_DYNAMIC(CProcessSelectDlg)

public:
    CProcessSelectDlg(CWnd* pParent = nullptr);
    virtual ~CProcessSelectDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG_SELECT_PROCESS };
#endif

    // 获取选中的进程名称
    CString GetSelectedProcessName() const { return m_strSelectedProcessName; }
    DWORD GetSelectedProcessID() const { return m_dwSelectedPID; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

private:
    CListCtrl m_listProcess;        // 进程列表控件
    CButton m_btnRefresh;            // 刷新按钮
    CButton m_btnSelect;             // 选择按钮
    CButton m_btnCancel;             // 取消按钮

    CString m_strSelectedProcessName;  // 选中的进程名
    DWORD m_dwSelectedPID;             // 选中的进程PID

    // 函数声明
    void RefreshProcessList();           // 刷新进程列表
    void AddProcessToList(DWORD dwPID, LPCTSTR szProcessName);  // 添加进程到列表
    CString GetProcessFileDescription(DWORD dwPID);  // 获取进程描述

    // 消息映射
    afx_msg void OnBnClickedBtnRefresh();
    afx_msg void OnBnClickedBtnSelect();
    afx_msg void OnBnClickedBtnCancel();
    afx_msg void OnLvnItemchangedListProcess(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMDblclkListProcess(NMHDR* pNMHDR, LRESULT* pResult);
};

