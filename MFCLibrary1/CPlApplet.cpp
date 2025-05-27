#include "pch.h"
#include "MFCLibrary1.h"
#include "ServiceManagerDlg.h"
#include <cpl.h>

extern "C" LONG APIENTRY CPlApplet(HWND hwndCpl, UINT uMsg, LPARAM lParam1, LPARAM lParam2) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    switch (uMsg) {
    case CPL_INIT:
        return 1; // Успешная инициализация
    case CPL_GETCOUNT:
        return 1; // Один апплет
    case CPL_INQUIRE: {
        CPLINFO* pInfo = (CPLINFO*)lParam2;

        pInfo->idIcon = IDI_APPLET_ICON;
        pInfo->idName = IDS_APPLET_NAME;
        pInfo->idInfo = IDS_APPLET_INFO;
        return 0;
    }
    case CPL_DBLCLK:
    {
        //MessageBox(NULL, _T("CPL_DBLCLK вызван, открываем диалог"), _T("Отладка"), MB_OK); // Отладка
        CServiceManagerDlg dlg;
        INT_PTR result = dlg.DoModal();
        if (result == -1) {
            MessageBox(NULL, _T("Ошибка открытия диалога"), _T("Ошибка"), MB_OK | MB_ICONERROR);
        }
        return 0;
    }
    default:
        return 0;
    }
}