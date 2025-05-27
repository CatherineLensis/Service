
// LoaderDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CLoaderDlg
class CLoaderDlg : public CDialogEx
{
// Создание
public:
	CLoaderDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOADER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

private:
	CEdit m_editPath;       // Поле для пути к файлу
	CEdit m_editName;       // Поле для названия
	CEdit m_editDescription;// Поле для описания
	CStatic m_staticIcon;   // Секция для иконки
	HICON m_hModuleIcon;    // Иконка модуля
	
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonBrowse();
};
