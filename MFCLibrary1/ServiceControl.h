#pragma once
#include <string>
#include <vector>

struct ServiceInfo {
    CString name;
    CString description;
    CString status;
    CString startupType;
    CString path;
};

struct ServiceDetails {
    CString name;
    CString description;
    CString path;
    CString user;
    CString memory;
};

std::vector<ServiceInfo> GetServiceList();
ServiceInfo GetServiceInfo(const CString& serviceName);
ServiceDetails GetServiceDetails(const CString& serviceName);
bool StartService(const CString& serviceName);
bool StopService(const CString& serviceName);