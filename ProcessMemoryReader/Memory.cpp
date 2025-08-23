#include "Memory.h"
#include "Process.h"
#include <stdexcept>
#include <sstream>

Memory::Memory(const Process* process) : m_process(process) {
    if (!process) {
        throw std::invalid_argument("Process pointer cannot be null");
    }
    
    if (!process->isAttached()) {
        throw std::invalid_argument("Process must be attached before creating Memory instance");
    }
}

std::string Memory::readString(uintptr_t address, size_t maxLength) const {
    validateProcess();
    
    std::vector<char> buffer(maxLength + 1, 0);
    if (!readMemory(address, buffer.data(), maxLength)) {
        throw std::runtime_error("Failed to read string from address: 0x" + 
                                std::to_string(address));
    }
    
    // Ensure null termination
    buffer[maxLength] = '\0';
    
    return std::string(buffer.data());
}

bool Memory::readMemory(uintptr_t address, void* buffer, size_t size) const {
    validateProcess();
    
    SIZE_T bytesRead = 0;
    BOOL result = ReadProcessMemory(
        m_process->getHandle(),
        reinterpret_cast<LPCVOID>(address),
        buffer,
        size,
        &bytesRead
    );
    
    return result && (bytesRead == size);
}

bool Memory::isValidAddress(uintptr_t address, size_t size) const {
    validateProcess();
    
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQueryEx(m_process->getHandle(), 
                       reinterpret_cast<LPCVOID>(address), 
                       &mbi, sizeof(mbi)) != sizeof(mbi)) {
        return false;
    }
    
    // Check if the memory region is committed and readable
    if (mbi.State != MEM_COMMIT) {
        return false;
    }
    
    if (!(mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE))) {
        return false;
    }
    
    // Check if the entire requested range is within the same memory region
    uintptr_t regionEnd = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    return (address + size) <= regionEnd;
}

void Memory::validateProcess() const {
    if (!m_process || !m_process->isAttached()) {
        throw std::runtime_error("Process is not attached or invalid");
    }
}
