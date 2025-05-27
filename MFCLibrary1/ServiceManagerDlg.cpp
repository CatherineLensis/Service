#include "pch.h"
#include "ServiceManagerDlg.h"
#include "ServiceControl.h"
#include <winsvc.h>
#include <vector>
#include <string>
#include <afxwin.h>

IMPLEMENT_DYNAMIC(CServiceManagerDlg, CDialogEx)

CServiceManagerDlg::CServiceManagerDlg(CWnd* pParent)
    : CDialogEx(IDD_SERVICE_MANAGER_DIALOG, pParent) {
}

CServiceManagerDlg::~CServiceManagerDlg() {
}

void CServiceManagerDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    // Привязываем элементы управления диалога к переменным класса
    DDX_Control(pDX, IDC_SERVICE_LIST, m_listServices);
    DDX_Control(pDX, IDC_START_SERVICE, m_btnStart);
    DDX_Control(pDX, IDC_STOP_SERVICE, m_btnStop);
    DDX_Control(pDX, IDC_RESTART_SERVICE, m_btnRestart);
    DDX_Control(pDX, IDC_REFRESH, m_btnRefresh);
    DDX_Control(pDX, IDC_SEARCH_SERVICE, m_editSearch);
    DDX_Control(pDX, IDC_SERVICE_DETAILS, m_editDetails);
    DDX_Control(pDX, IDC_SERVICE_ICON, m_staticIcon);
}

BEGIN_MESSAGE_MAP(CServiceManagerDlg, CDialogEx)

    ON_BN_CLICKED(IDC_START_SERVICE, &CServiceManagerDlg::OnBnClickedStartService)
    ON_BN_CLICKED(IDC_STOP_SERVICE, &CServiceManagerDlg::OnBnClickedStopService)
    ON_BN_CLICKED(IDC_RESTART_SERVICE, &CServiceManagerDlg::OnBnClickedRestartService)
    ON_BN_CLICKED(IDC_REFRESH, &CServiceManagerDlg::OnBnClickedRefresh)
    ON_EN_CHANGE(IDC_SEARCH_SERVICE, &CServiceManagerDlg::OnEnChangeSearchService)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVICE_LIST, &CServiceManagerDlg::OnLvnItemchangedServiceList)
END_MESSAGE_MAP()

BOOL CServiceManagerDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // Устанавливаем стиль LVS_REPORT для списка служб программно
    m_listServices.ModifyStyle(0, LVS_REPORT);
    m_listServices.InsertColumn(0, _T("Имя службы"), LVCFMT_LEFT, 150);
    m_listServices.InsertColumn(1, _T("Описание"), LVCFMT_LEFT, 200);
    m_listServices.InsertColumn(2, _T("Состояние"), LVCFMT_LEFT, 100);
    m_listServices.InsertColumn(3, _T("Тип запуска"), LVCFMT_LEFT, 150);

    // Устанавливаем стиль SS_ICON для статического элемента иконки программно
    m_staticIcon.ModifyStyle(0, SS_ICON);

    HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_APPLET_ICON));
    if (hIcon) {
        m_staticIcon.SetIcon(hIcon);
        DestroyIcon(hIcon); // Освобождаем иконку
    }
    else {
        MessageBox(_T("Не удалось загрузить иконку апплета"), _T("Ошибка"), MB_OK | MB_ICONERROR);
    }
    // Инициализация CImageList
    m_imageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1);  // Размер иконок 16x16
    m_listServices.SetImageList(&m_imageList, LVSIL_SMALL);   // Привязка к CListCtrl
    // Инициализируем список служб
    PopulateServiceList();
    return TRUE;
}


void CServiceManagerDlg::PopulateServiceList(const CString& filter) {
    m_listServices.DeleteAllItems();       // Очищаем элементы списка управления
    m_imageList.DeleteImageList();         // Очищаем список изображений CImageList

    std::vector<ServiceInfo> services = GetServiceList(); // Получаем список служб
    for (size_t i = 0; i < services.size(); ++i) {
        if (filter.IsEmpty() || services[i].name.Find(filter) != -1) {
            int iconIndex = -1;
            // Извлекаем иконку с помощью SHGetFileInfo
            SHFILEINFO sfi = { 0 };
            if (!services[i].path.IsEmpty()) {
                SHGetFileInfo(services[i].path, 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON);
                if (sfi.hIcon) {
                    iconIndex = m_imageList.Add(sfi.hIcon);
                    DestroyIcon(sfi.hIcon); // Освобождаем иконку
                }
            }
            // Добавляем элемент в список управления
            int index = m_listServices.InsertItem(i, services[i].name, iconIndex);
            m_listServices.SetItemText(index, 1, services[i].description);
            m_listServices.SetItemText(index, 2, services[i].status);
            m_listServices.SetItemText(index, 3, services[i].startupType);
        }
    }
}

//путь и пользователь не отображаются
void CServiceManagerDlg::UpdateDetails(const CString& serviceName) {
    ServiceInfo info = GetServiceInfo(serviceName);
    ServiceDetails details = GetServiceDetails(serviceName);
    CString text;
    text.Format(_T("Имя:\t%s\n Описание:\t%s\n Путь:\t%s\n Пользователь:\t%s\n Память:\t%s\n"),
        info.name, info.description, info.path, details.user, details.memory);
    m_editDetails.SetWindowText(text);
}

void CServiceManagerDlg::OnBnClickedStartService()
{
    POSITION pos = m_listServices.GetFirstSelectedItemPosition();
    if (pos) {
        int nItem = m_listServices.GetNextSelectedItem(pos);
        CString serviceName = m_listServices.GetItemText(nItem, 0);
        if (StartService(serviceName)) {
            PopulateServiceList();
            UpdateDetails(serviceName);
        }
        else {
            MessageBox(_T("Не удалось запустить службу"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        }
    }
	else {
		MessageBox(_T("Выберите службу для запуска"), _T("Ошибка"), MB_OK | MB_ICONWARNING);
	}
}


void CServiceManagerDlg::OnBnClickedStopService()
{
    POSITION pos = m_listServices.GetFirstSelectedItemPosition();
    if (pos) {
        int nItem = m_listServices.GetNextSelectedItem(pos);
        CString serviceName = m_listServices.GetItemText(nItem, 0);
        if (StopService(serviceName)) {
            PopulateServiceList();
            UpdateDetails(serviceName);
        }
        else {
            MessageBox(_T("Не удалось остановить службу"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        }
    }
}


void CServiceManagerDlg::OnBnClickedRestartService()
{
    POSITION pos = m_listServices.GetFirstSelectedItemPosition();
    if (pos) {
        int nItem = m_listServices.GetNextSelectedItem(pos);
        CString serviceName = m_listServices.GetItemText(nItem, 0);
        if (StopService(serviceName) && StartService(serviceName)) {
            PopulateServiceList();
            UpdateDetails(serviceName);
        }
        else {
            MessageBox(_T("Не удалось перезапустить службу"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        }
    }
}


void CServiceManagerDlg::OnBnClickedRefresh()
{
    CString filter;
    m_editSearch.GetWindowText(filter);
    PopulateServiceList(filter);
}


void CServiceManagerDlg::OnEnChangeSearchService()
{
    // TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // функция и вызов CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    CString filter;
    m_editSearch.GetWindowText(filter);
    PopulateServiceList(filter);
}


void CServiceManagerDlg::OnLvnItemchangedServiceList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    if (pNMLV->uNewState & LVIS_SELECTED) {
        int nItem = pNMLV->iItem;
        CString serviceName = m_listServices.GetItemText(nItem, 0);
        UpdateDetails(serviceName);
    }
    *pResult = 0;
}
