#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

/**
 * @class Process
 * @brief Handles process attachment and management using Windows API
 * 
 * This class provides functionality to attach to a running process by name,
 * manage process handles, and obtain module information. It follows RAII
 * principles for automatic resource cleanup.
 */
class Process {
private:
    HANDLE m_processHandle;     ///< Handle to the attached process
    DWORD m_processId;          ///< Process ID of the attached process
    std::string m_processName;  ///< Name of the process

public:
    /**
     * @brief Default constructor initializes an invalid process state
     */
    Process();

    /**
     * @brief Constructor that immediately attaches to a process by name
     * @param processName The name of the process to attach to (e.g., "notepad.exe")
     * @throws std::runtime_error if the process cannot be found or opened
     */
    explicit Process(const std::string& processName);

    /**
     * @brief Destructor automatically closes the process handle
     */
    ~Process();

    // Delete copy constructor and assignment operator to prevent handle duplication
    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    // Enable move semantics for efficient transfer of ownership
    Process(Process&& other) noexcept;
    Process& operator=(Process&& other) noexcept;

    /**
     * @brief Attach to a running process by name
     * @param processName The name of the process to attach to
     * @return true if successfully attached, false otherwise
     */
    bool attachToProcess(const std::string& processName);

    /**
     * @brief Detach from the current process and close handle
     */
    void detach();

    /**
     * @brief Get the base address of a module within the process
     * @param moduleName The name of the module (e.g., "client.dll")
     * @return The base address of the module, or nullptr if not found
     */
    uintptr_t getModuleBaseAddress(const std::string& moduleName) const;

    /**
     * @brief Check if the process is currently attached and valid
     * @return true if attached to a valid process, false otherwise
     */
    bool isAttached() const;

    /**
     * @brief Get the process handle
     * @return The Windows process handle
     */
    HANDLE getHandle() const;

    /**
     * @brief Get the process ID
     * @return The process ID
     */
    DWORD getProcessId() const;

    /**
     * @brief Get the process name
     * @return The name of the attached process
     */
    const std::string& getProcessName() const;

private:
    /**
     * @brief Find a process ID by name
     * @param processName The name of the process to find
     * @return The process ID, or 0 if not found
     */
    DWORD findProcessId(const std::string& processName) const;

    /**
     * @brief Clean up resources and reset to invalid state
     */
    void cleanup();
};
