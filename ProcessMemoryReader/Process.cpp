#include "Process.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <stdexcept>

Process::Process() : m_processHandle(nullptr), m_processId(0) {}

Process::Process(const std::string& processName) : m_processHandle(nullptr), m_processId(0) {
    if (!attachToProcess(processName)) {
        throw std::runtime_error("Failed to attach to process: " + processName);
    }
}

Process::~Process() {
    cleanup();
}

Process::Process(Process&& other) noexcept 
    : m_processHandle(other.m_processHandle)
    , m_processId(other.m_processId)
    , m_processName(std::move(other.m_processName)) {
    other.m_processHandle = nullptr;
    other.m_processId = 0;
}

Process& Process::operator=(Process&& other) noexcept {
    if (this != &other) {
        cleanup();
        
        m_processHandle = other.m_processHandle;
        m_processId = other.m_processId;
        m_processName = std::move(other.m_processName);
        
        other.m_processHandle = nullptr;
        other.m_processId = 0;
    }
    return *this;
}

bool Process::attachToProcess(const std::string& processName) {
    // Clean up any existing attachment
    detach();
    
    // Find the process ID
    DWORD processId = findProcessId(processName);
    if (processId == 0) {
        return false;
    }
    
    // Open the process with required permissions
    HANDLE processHandle = OpenProcess(
        PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
        FALSE,
        processId
    );
    
    if (processHandle == nullptr) {
        return false;
    }
    
    // Store the process information
    m_processHandle = processHandle;
    m_processId = processId;
    m_processName = processName;
    
    return true;
}

void Process::detach() {
    cleanup();
}

uintptr_t Process::getModuleBaseAddress(const std::string& moduleName) const {
    if (!isAttached()) {
        return 0;
    }
    
    HMODULE modules[1024];
    DWORD bytesNeeded;
    
    if (!EnumProcessModules(m_processHandle, modules, sizeof(modules), &bytesNeeded)) {
        return 0;
    }
    
    DWORD moduleCount = bytesNeeded / sizeof(HMODULE);
    
    for (DWORD i = 0; i < moduleCount; i++) {
        char moduleNameBuffer[MAX_PATH];
        if (GetModuleBaseNameA(m_processHandle, modules[i], moduleNameBuffer, sizeof(moduleNameBuffer))) {
            if (std::string(moduleNameBuffer) == moduleName) {
                return reinterpret_cast<uintptr_t>(modules[i]);
            }
        }
    }
    
    return 0;
}

bool Process::isAttached() const {
    if (m_processHandle == nullptr || m_processId == 0) {
        return false;
    }
    
    // Check if the process is still running
    DWORD exitCode;
    if (!GetExitCodeProcess(m_processHandle, &exitCode)) {
        return false;
    }
    
    return exitCode == STILL_ACTIVE;
}

HANDLE Process::getHandle() const {
    return m_processHandle;
}

DWORD Process::getProcessId() const {
    return m_processId;
}

const std::string& Process::getProcessName() const {
    return m_processName;
}

DWORD Process::findProcessId(const std::string& processName) const {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(snapshot, &processEntry)) {
        do {
            if (std::string(processEntry.szExeFile) == processName) {
                CloseHandle(snapshot);
                return processEntry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &processEntry));
    }
    
    CloseHandle(snapshot);
    return 0;
}

void Process::cleanup() {
    if (m_processHandle != nullptr) {
        CloseHandle(m_processHandle);
        m_processHandle = nullptr;
    }
    
    m_processId = 0;
    m_processName.clear();
}
