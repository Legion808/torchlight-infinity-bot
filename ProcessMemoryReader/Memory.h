#pragma once

#include <Windows.h>
#include <memory>
#include <vector>
#include <stdexcept>

// Forward declaration
class Process;

/**
 * @class Memory
 * @brief Provides safe memory reading operations for external processes
 * 
 * This class wraps Windows API memory reading functions and provides
 * type-safe methods for reading different data types from process memory.
 * It includes bounds checking and error handling for robust operation.
 */
class Memory {
private:
    const Process* m_process;  ///< Pointer to the associated process

public:
    /**
     * @brief Constructor that associates this Memory instance with a Process
     * @param process Pointer to a valid, attached Process object
     * @throws std::invalid_argument if process is null or not attached
     */
    explicit Memory(const Process* process);

    /**
     * @brief Read a value of type T from the specified address
     * @tparam T The type of data to read (must be trivially copyable)
     * @param address The memory address to read from
     * @return The value read from memory
     * @throws std::runtime_error if the read operation fails
     */
    template<typename T>
    T read(uintptr_t address) const {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
        
        T value{};
        if (!readMemory(address, &value, sizeof(T))) {
            throw std::runtime_error("Failed to read memory at address: 0x" + 
                                    std::to_string(address));
        }
        return value;
    }

    /**
     * @brief Read a null-terminated string from the specified address
     * @param address The memory address to read from
     * @param maxLength Maximum length to read to prevent infinite loops
     * @return The string read from memory
     * @throws std::runtime_error if the read operation fails
     */
    std::string readString(uintptr_t address, size_t maxLength = 256) const;

    /**
     * @brief Read raw bytes from memory into a buffer
     * @param address The memory address to read from
     * @param buffer Pointer to the destination buffer
     * @param size Number of bytes to read
     * @return true if successful, false otherwise
     */
    bool readMemory(uintptr_t address, void* buffer, size_t size) const;

    /**
     * @brief Read an array of values from memory
     * @tparam T The type of elements in the array
     * @param address The starting memory address
     * @param count Number of elements to read
     * @return Vector containing the read elements
     * @throws std::runtime_error if the read operation fails
     */
    template<typename T>
    std::vector<T> readArray(uintptr_t address, size_t count) const {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");
        
        std::vector<T> result(count);
        if (!readMemory(address, result.data(), sizeof(T) * count)) {
            throw std::runtime_error("Failed to read array from address: 0x" + 
                                    std::to_string(address));
        }
        return result;
    }

    /**
     * @brief Follow a pointer chain to read the final value
     * @tparam T The type of the final value to read
     * @param baseAddress The starting address
     * @param offsets Vector of offsets to follow
     * @return The value at the end of the pointer chain
     * @throws std::runtime_error if any step in the chain fails
     */
    template<typename T>
    T readPointerChain(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets) const {
        uintptr_t currentAddress = baseAddress;
        
        // Follow the pointer chain, except for the last offset
        for (size_t i = 0; i < offsets.size() - 1; ++i) {
            currentAddress = read<uintptr_t>(currentAddress + offsets[i]);
        }
        
        // Read the final value
        if (!offsets.empty()) {
            currentAddress += offsets.back();
        }
        
        return read<T>(currentAddress);
    }

    /**
     * @brief Check if a memory address is readable
     * @param address The address to check
     * @param size The size of the memory region to check
     * @return true if the memory region is readable, false otherwise
     */
    bool isValidAddress(uintptr_t address, size_t size = sizeof(void*)) const;

private:
    /**
     * @brief Validate that the associated process is valid and attached
     * @throws std::runtime_error if the process is invalid
     */
    void validateProcess() const;
};
