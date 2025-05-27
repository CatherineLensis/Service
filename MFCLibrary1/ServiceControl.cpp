#include "pch.h"
#include "ServiceControl.h"
#include "ProcessMemory.h"
#include <winsvc.h>
#include <string>
#include <vector>


// Функция для получения информации о конкретной службе по имени
ServiceInfo GetServiceInfo(const CString& serviceName) {
    ServiceInfo info;
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!hSCManager) {
        MessageBox(NULL, L"Не удалось открыть SCManager", L"Ошибка", MB_OK | MB_ICONERROR);
        return info; // Вернём пустую структуру в случае ошибки
    }

    DWORD dwBytesNeeded = 0, dwServicesReturned = 0;
    DWORD bufferSize = 1024; // Начальный размер буфера
    std::vector<BYTE> buffer(bufferSize);
    LPBYTE pBuffer = buffer.data();

    while (true) {
        if (EnumServicesStatusEx(hSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
            pBuffer, bufferSize, &dwBytesNeeded, &dwServicesReturned, NULL, NULL)) {
            ENUM_SERVICE_STATUS_PROCESS* pServices = (ENUM_SERVICE_STATUS_PROCESS*)pBuffer;
            for (DWORD i = 0; i < dwServicesReturned; ++i) {
                // Сравниваем имя службы (без учёта регистра)
                if (_wcsicmp(pServices[i].lpServiceName, serviceName) == 0) {
                    info.name = pServices[i].lpServiceName;
                    info.description = pServices[i].lpDisplayName;
                    info.status = (pServices[i].ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
                        ? L"Запущена" : L"Остановлена";

                    // Получение пути к исполняемому файлу
                    SC_HANDLE hService = OpenService(hSCManager, pServices[i].lpServiceName, SERVICE_QUERY_CONFIG);
                    if (hService) {
                        QUERY_SERVICE_CONFIG qsc;
                        DWORD dwNeeded;
                        if (QueryServiceConfig(hService, &qsc, sizeof(qsc), &dwNeeded)) {
                            info.path = qsc.lpBinaryPathName;  // Заполняем поле path
                            switch (qsc.dwStartType) {
                            case SERVICE_AUTO_START: info.startupType = L"Автоматический"; break;
                            case SERVICE_DEMAND_START: info.startupType = L"Ручной"; break;
                            case SERVICE_DISABLED: info.startupType = L"Отключён"; break;
                            default: info.startupType = L"Неизвестно";
                            }
                        }
                        CloseServiceHandle(hService);
                    }
                    break; // Нашли нужную службу, выходим из цикла
                }
            }
            break; // Завершаем цикл после обработки всех служб
        }
        else {
            DWORD error = GetLastError();
            if (error == ERROR_MORE_DATA) {
                bufferSize = dwBytesNeeded + 1024; // Увеличиваем буфер с запасом
                buffer.resize(bufferSize);
                pBuffer = buffer.data();
            }
            else {
                break; // Прерываем цикл в случае другой ошибки
            }
        }
    }
    CloseServiceHandle(hSCManager);
    return info; // Возвращаем информацию о выбранной службе
}


// Функция для получения списка всех служб (оставляем для совместимости)
std::vector<ServiceInfo> GetServiceList() {
    std::vector<ServiceInfo> services;
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!hSCManager) {
        MessageBox(NULL, L"Не удалось открыть SCManager", L"Ошибка", MB_OK | MB_ICONERROR);
        return services;
    }

    DWORD dwBytesNeeded = 0, dwServicesReturned = 0;
    DWORD bufferSize = 1024;
    std::vector<BYTE> buffer(bufferSize);
    LPBYTE pBuffer = buffer.data();

    while (true) {
        if (EnumServicesStatusEx(hSCManager, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
                                 pBuffer, bufferSize, &dwBytesNeeded, &dwServicesReturned, NULL, NULL)) {
            ENUM_SERVICE_STATUS_PROCESS* pServices = (ENUM_SERVICE_STATUS_PROCESS*)pBuffer;
            for (DWORD i = 0; i < dwServicesReturned; ++i) {
                ServiceInfo info;
                info.name = pServices[i].lpServiceName;
                info.description = pServices[i].lpDisplayName;
                info.status = (pServices[i].ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
                                  ? L"Запущена" : L"Остановлена";

                SC_HANDLE hService = OpenService(hSCManager, pServices[i].lpServiceName, SERVICE_QUERY_CONFIG);
                if (hService) {
                    QUERY_SERVICE_CONFIG qsc;
                    DWORD dwNeeded;
                    if (QueryServiceConfig(hService, &qsc, sizeof(qsc), &dwNeeded)) {
                        info.path = qsc.lpBinaryPathName;
                        switch (qsc.dwStartType) {
                            case SERVICE_AUTO_START: info.startupType = L"Автоматический"; break;
                            case SERVICE_DEMAND_START: info.startupType = L"Ручной"; break;
                            case SERVICE_DISABLED: info.startupType = L"Отключён"; break;
                            default: info.startupType = L"Неизвестно";
                        }
                    }
                    CloseServiceHandle(hService);
                }
                services.push_back(info);
            }
            break;
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_MORE_DATA) {
                bufferSize = dwBytesNeeded + 1024;
                buffer.resize(bufferSize);
                pBuffer = buffer.data();
            } else {
                break;
            }
        }
    }
    CloseServiceHandle(hSCManager);
    return services;
}


ServiceDetails GetServiceDetails(const CString& serviceName) {
    ServiceDetails details;
    details.name = serviceName;
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager) {
        SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);
        if (hService) {
            QUERY_SERVICE_CONFIG qsc;
            DWORD dwNeeded;
            if (QueryServiceConfig(hService, &qsc, sizeof(qsc), &dwNeeded)) {
                details.description = qsc.lpDisplayName;
                details.path = qsc.lpBinaryPathName;
                details.user = qsc.lpServiceStartName;
            }
            SERVICE_STATUS_PROCESS ssp;
            if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), &dwNeeded)) {
                DWORD pid = ssp.dwProcessId;
                CString memory;
                memory.Format(_T("%u KB"), GetProcessMemoryUsage(pid));
                details.memory = memory;
            }
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCManager);
    }
    return details;
}

bool StartService(const CString& serviceName) {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager) {
        SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_START);
        if (hService) {
            bool result = ::StartService(hService, 0, NULL);
            CloseServiceHandle(hService);
            CloseServiceHandle(hSCManager);
            return result;
        }
        CloseServiceHandle(hSCManager);
    }
    return false;
}

bool StopService(const CString& serviceName) {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager) {
        SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP);
        if (hService) {
            SERVICE_STATUS status;
            bool result = ControlService(hService, SERVICE_CONTROL_STOP, &status);
            CloseServiceHandle(hService);
            CloseServiceHandle(hSCManager);
            return result;
        }
        CloseServiceHandle(hSCManager);
    }
    return false;
}