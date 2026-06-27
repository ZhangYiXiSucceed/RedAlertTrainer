#include "pch.h"
#include "CProcessSelectDlg.h"
#include "RedAlertTrainer.h"
#include <tlhelp32.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Version.lib")  

IMPLEMENT_DYNAMIC(CProcessSelectDlg, CDialogEx)

CProcessSelectDlg::CProcessSelectDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_SELECT_PROCESS, pParent)
    , m_dwSelectedPID(0)
{
}

CProcessSelectDlg::~CProcessSelectDlg()
{
}

void CProcessSelectDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PROCESS, m_listProcess);
    DDX_Control(pDX, IDC_BTN_REFRESH, m_btnRefresh);
    DDX_Control(pDX, IDC_BTN_SELECT, m_btnSelect);
    DDX_Control(pDX, IDC_BTN_CANCEL, m_btnCancel);
}

BEGIN_MESSAGE_MAP(CProcessSelectDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_REFRESH, &CProcessSelectDlg::OnBnClickedBtnRefresh)
    ON_BN_CLICKED(IDC_BTN_SELECT, &CProcessSelectDlg::OnBnClickedBtnSelect)
    ON_BN_CLICKED(IDC_BTN_CANCEL, &CProcessSelectDlg::OnBnClickedBtnCancel)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROCESS, &CProcessSelectDlg::OnLvnItemchangedListProcess)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESS, &CProcessSelectDlg::OnNMDblclkListProcess)
END_MESSAGE_MAP()

BOOL CProcessSelectDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();


    // 设置列表控件样式
    DWORD dwStyle = m_listProcess.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER;

    m_listProcess.SetExtendedStyle(dwStyle);

    // 删除所有现有列
    while (m_listProcess.DeleteColumn(0)) {}

    // 添加列
    m_listProcess.InsertColumn(0, _T("进程名称"), LVCFMT_LEFT, 200);
    m_listProcess.InsertColumn(1, _T("进程ID"), LVCFMT_LEFT, 100);
    m_listProcess.InsertColumn(2, _T("描述"), LVCFMT_LEFT, 250);

    // 刷新进程列表
    RefreshProcessList();

    return TRUE;
}

void CProcessSelectDlg::RefreshProcessList()
{
    // 清空列表
    m_listProcess.DeleteAllItems();

    // 创建进程快照
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(_T("无法创建进程快照！"));
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    int nIndex = 0;
    if (Process32First(hSnapshot, &pe))
    {
        do
        {
            // 添加进程到列表
            AddProcessToList(pe.th32ProcessID, pe.szExeFile);
            nIndex++;
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);

    // 自动选中红警进程
    for (int i = 0; i < m_listProcess.GetItemCount(); i++)
    {
        CString strProcessName = m_listProcess.GetItemText(i, 0);
        if (strProcessName.Find(_T("game")) != -1)
        {
            m_listProcess.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            m_listProcess.EnsureVisible(i, FALSE);
            break;
        }
    }

    // 如果有进程但没有自动选中，选中第一个
    if (m_listProcess.GetSelectedCount() == 0 && m_listProcess.GetItemCount() > 0)
    {
        m_listProcess.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    }
}

void CProcessSelectDlg::AddProcessToList(DWORD dwPID, LPCTSTR szProcessName)
{
    int nItem = m_listProcess.GetItemCount();

    // 插入进程名
    m_listProcess.InsertItem(nItem, szProcessName);

    // 插入PID
    CString strPID;
    strPID.Format(_T("%d"), dwPID);
    m_listProcess.SetItemText(nItem, 1, strPID);

    // 插入描述
    CString strDescription = GetProcessFileDescription(dwPID);
    m_listProcess.SetItemText(nItem, 2, strDescription);
}

CString CProcessSelectDlg::GetProcessFileDescription(DWORD dwPID)
{
    CString strDescription;

    // 打开进程
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
    if (hProcess)
    {
        TCHAR szFilePath[MAX_PATH] = { 0 };
        DWORD dwSize = MAX_PATH;

        // 获取进程执行文件路径
        if (QueryFullProcessImageName(hProcess, 0, szFilePath, &dwSize))
        {
            // 获取文件版本信息
            DWORD dwHandle = 0;
            DWORD dwSize = GetFileVersionInfoSize(szFilePath, &dwHandle);
            if (dwSize > 0)
            {
                BYTE* pBuffer = new BYTE[dwSize];
                if (GetFileVersionInfo(szFilePath, 0, dwSize, pBuffer))
                {
                    LPVOID pValue = NULL;
                    UINT uLen = 0;
                    if (VerQueryValue(pBuffer, _T("\\StringFileInfo\\040904B0\\FileDescription"), &pValue, &uLen))
                    {
                        strDescription = (LPCTSTR)pValue;
                    }
                    else if (VerQueryValue(pBuffer, _T("\\StringFileInfo\\080404B0\\FileDescription"), &pValue, &uLen))
                    {
                        strDescription = (LPCTSTR)pValue;
                    }
                }
                delete[] pBuffer;
            }

            // 如果获取不到描述，使用文件名
            if (strDescription.IsEmpty())
            {
                TCHAR szFileName[MAX_PATH];
                _tsplitpath_s(szFilePath, NULL, 0, NULL, 0, szFileName, MAX_PATH, NULL, 0);
                strDescription = szFileName;
            }
        }
        CloseHandle(hProcess);
    }

    return strDescription;
}

void CProcessSelectDlg::OnBnClickedBtnRefresh()
{
    RefreshProcessList();
}

void CProcessSelectDlg::OnBnClickedBtnSelect()
{
    // 获取选中的项
    POSITION pos = m_listProcess.GetFirstSelectedItemPosition();
    if (pos)
    {
        int nItem = m_listProcess.GetNextSelectedItem(pos);
        m_strSelectedProcessName = m_listProcess.GetItemText(nItem, 0);
        m_dwSelectedPID = _ttoi(m_listProcess.GetItemText(nItem, 1));

        // 验证进程是否还存在
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, m_dwSelectedPID);
        if (hProcess)
        {
            CloseHandle(hProcess);
            EndDialog(IDOK);
        }
        else
        {
            AfxMessageBox(_T("所选进程已不存在，请重新选择！"), MB_OK | MB_ICONWARNING);
            RefreshProcessList();
        }
    }
    else
    {
        AfxMessageBox(_T("请先选择一个进程！"), MB_OK | MB_ICONWARNING);
    }
}

void CProcessSelectDlg::OnBnClickedBtnCancel()
{
    EndDialog(IDCANCEL);
}

void CProcessSelectDlg::OnLvnItemchangedListProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    // 如果有选中项，启用选择按钮
    POSITION pos = m_listProcess.GetFirstSelectedItemPosition();
    m_btnSelect.EnableWindow(pos != NULL);

    *pResult = 0;
}

void CProcessSelectDlg::OnNMDblclkListProcess(NMHDR* pNMHDR, LRESULT* pResult)
{
    // 双击自动选择
    OnBnClickedBtnSelect();
    *pResult = 0;
}
