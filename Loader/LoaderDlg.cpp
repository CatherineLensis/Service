
// LoaderDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "Loader.h"
#include "LoaderDlg.h"
#include "afxdialogex.h"
#include <Shellapi.h>
#include <cpl.h>
#include <windows.h>
#include <vector>
#include <VersionHelpers.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	// Реализация
protected:
	DECLARE_MESSAGE_MAP()
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


// Диалоговое окно CLoaderDlg



CLoaderDlg::CLoaderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOADER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PATH, m_editPath);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_editDescription);
	DDX_Control(pDX, IDC_STATIC_ICON, m_staticIcon);
}

BEGIN_MESSAGE_MAP(CLoaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CLoaderDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CLoaderDlg::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// Обработчики сообщений CLoaderDlg

BOOL CLoaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.


	// IDM_ABOUTBOX должен быть в пределах системной команды.
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
	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок
	m_staticIcon.ModifyStyle(0, SS_ICON);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CLoaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CLoaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CLoaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLoaderDlg::OnBnClickedButtonBrowse()
{
	CFileDialog fileDlg(TRUE, _T("cpl"), NULL, OFN_FILEMUSTEXIST, _T("CPL Files (*.cpl)|*.cpl|DLL Files (*.dll)|*.dll|All Files (*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString dllPath = fileDlg.GetPathName();
		m_editPath.SetWindowText(dllPath);

		HMODULE hDll = LoadLibrary(dllPath);
		if (hDll == NULL)
		{
			CString errorMsg;
			errorMsg.Format(_T("Не удалось загрузить %s. Код ошибки: %d"), dllPath, GetLastError());
			MessageBox(errorMsg, _T("Ошибка"), MB_OK | MB_ICONERROR);
			m_editName.SetWindowText(_T(""));
			m_editDescription.SetWindowText(_T(""));
			if (m_hModuleIcon)
			{
				DestroyIcon(m_hModuleIcon);
				m_staticIcon.SetIcon(NULL);
				m_hModuleIcon = NULL;
			}
			return;
		}
		else {
			// Получение параметров модуля (для CPL)
			typedef LONG(WINAPI* CPLAPPLET)(HWND, UINT, LPARAM, LPARAM);
			CPLAPPLET pCplApplet = (CPLAPPLET)GetProcAddress(hDll, "CPlApplet");
			LONG initResult = pCplApplet(NULL, CPL_INIT, 0, 0);

			if (initResult != 1)
			{
				MessageBox(_T("CPL_INIT завершился с ошибкой"), _T("Ошибка"), MB_OK | MB_ICONERROR);
				FreeLibrary(hDll);
				return;
			}

			if (pCplApplet)
			{
				// Получаем информацию о модуле
				// NEWCPLINFOA - структура, содержащая информацию о модуле CPL
				// CPL_NEWINQUIRE - запрос информации о модуле
				// hIcon - иконка модуля
				// szName - название модуля
				// szInfo - описание модуля
				initResult = pCplApplet(NULL, CPL_GETCOUNT, 0, 0);
				if (initResult == 0)
				{
					MessageBox(_T("CPL_GETCOUNT завершился с ошибкой"), _T("Ошибка"), MB_OK | MB_ICONERROR);
					FreeLibrary(hDll);
					return;
				}
				// Используем CPLINFO с сообщением CPL_INQUIRE
				CPLINFO cplInfo = { 0 };
				pCplApplet(NULL, CPL_INQUIRE, 0, (LPARAM)&cplInfo); // Запрашиваем информацию о первом апплете
				// Загрузка имени
				TCHAR szName[256] = { 0 };
				if (LoadString(hDll, cplInfo.idName, szName, _countof(szName)))
				{
					m_editName.SetWindowText(szName);
				}
				else
				{
					TCHAR szFileName[MAX_PATH];
					GetModuleFileName(hDll, szFileName, MAX_PATH);
					m_editName.SetWindowText(PathFindFileName(szFileName)); // Фallback на имя файла
				}

				// Загрузка описания
				TCHAR szInfo[256] = { 0 };
				if (LoadString(hDll, cplInfo.idInfo, szInfo, _countof(szInfo)))
				{
					m_editDescription.SetWindowText(szInfo);
				}
				else
				{
					// Пробуем извлечь описание из версии файла
					DWORD dummy;
					DWORD size = GetFileVersionInfoSize(dllPath, &dummy);
					if (size > 0)
					{
						std::vector<BYTE> buffer(size);
						if (GetFileVersionInfo(dllPath, 0, size, buffer.data()))
						{
							LPVOID value;
							UINT len;
							static const LPCTSTR Sub_Blocks[] = {
								_T("\\StringFileInfo\\040904b0\\FileDescription"),
								_T("\\StringFileInfo\\040904E4\\FileDescription"),
								_T("\\StringFileInfo\\040904B0\\FileDescription"),
								_T("\\StringFileInfo\\000004B0\\FileDescription")
							};
							bool descriptionFound = false;
							for (int i = 0; i < _countof(Sub_Blocks); ++i)
							{
								if (VerQueryValue(buffer.data(), Sub_Blocks[i], &value, &len) && len > 0)
								{
									m_editDescription.SetWindowText((LPCTSTR)value);
									descriptionFound = true;
									break;
								}
							}
							if (!descriptionFound)
							{
								m_editDescription.SetWindowText(_T("Описание не найдено"));
							}
						}
					}
					else
					{
						m_editDescription.SetWindowText(_T("Описание не найдено"));
					}
				}

				// Загрузка иконки
				HICON hIcon = LoadIcon(hDll, MAKEINTRESOURCE(cplInfo.idIcon));
				if (hIcon)
				{
					if (m_hModuleIcon)
					{
						DestroyIcon(m_hModuleIcon);
					}
					m_hModuleIcon = CopyIcon(hIcon);
					DestroyIcon(hIcon);
					if (m_hModuleIcon)
					{
						m_staticIcon.SetIcon(m_hModuleIcon);
					}
				}
				else
				{
					// Fallback на извлечение иконки из файла
					HICON hFallbackIcon = ExtractIcon(NULL, dllPath, 0);
					if (hFallbackIcon)
					{
						if (m_hModuleIcon)
						{
							DestroyIcon(m_hModuleIcon);
						}
						m_hModuleIcon = CopyIcon(hFallbackIcon);
						DestroyIcon(hFallbackIcon);
						if (m_hModuleIcon)
						{
							m_staticIcon.SetIcon(m_hModuleIcon);
						}
					}
					else
					{
						m_staticIcon.SetIcon(NULL);
						if (m_hModuleIcon)
						{
							DestroyIcon(m_hModuleIcon);
							m_hModuleIcon = NULL;
						}
					}
				}
			}
		}
	}
}

void CLoaderDlg::OnBnClickedButtonStart()
{
	CString dllPath;
	m_editPath.GetWindowText(dllPath);
	if (!dllPath.IsEmpty())
	{
		if (PathFileExists(dllPath))
		{
			ShellExecute(NULL, _T("open"), _T("control.exe"), dllPath, NULL, SW_SHOWNORMAL);
		}
		else
		{
			MessageBox(_T("Файл не найден. Укажите существующий файл .cpl"), _T("Ошибка"), MB_OK | MB_ICONERROR);
		}
	}
	else
	{
		MessageBox(_T("Пожалуйста, выберите файл .cpl"), _T("Ошибка"), MB_OK | MB_ICONERROR);
	}
}