#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"
#include <string>

class CServiceManagerDlg : public CDialogEx {
    DECLARE_DYNAMIC(CServiceManagerDlg)

public:
    CServiceManagerDlg(CWnd* pParent = nullptr);
    virtual ~CServiceManagerDlg();

    // Идентификатор диалога, связанный с ресурсом
    enum { IDD = IDD_SERVICE_MANAGER_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    // Элементы управления диалога
    CListCtrl m_listServices;   // Список служб (IDC_SERVICE_LIST)
    CButton m_btnStart;         // Кнопка "Запустить" (IDC_START_SERVICE)
    CButton m_btnStop;          // Кнопка "Остановить" (IDC_STOP_SERVICE)
    CButton m_btnRestart;       // Кнопка "Перезапустить" (IDC_RESTART_SERVICE)
    CButton m_btnRefresh;       // Кнопка "Обновить" (IDC_REFRESH)
    CEdit m_editSearch;         // Поле поиска (IDC_SEARCH_SERVICE)
    CEdit m_editDetails;        // Поле деталей (IDC_SERVICE_DETAILS)
    CStatic m_staticIcon;       // Иконка (IDC_SERVICE_ICON)
    CImageList m_imageList;  // Для хранения иконок


    // Функции для управления службами
private:
    void PopulateServiceList(const CString& filter = _T(""));
    void UpdateDetails(const CString& serviceName);
public:
    afx_msg void OnBnClickedStartService();
    afx_msg void OnBnClickedStopService();
    afx_msg void OnBnClickedRestartService();
    afx_msg void OnBnClickedRefresh();
    afx_msg void OnEnChangeSearchService();
    afx_msg void OnLvnItemchangedServiceList(NMHDR* pNMHDR, LRESULT* pResult);
};