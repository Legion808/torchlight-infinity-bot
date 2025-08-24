#include "OffsetManager.h"
#include "Memory.h"
#include <iostream>
#include <fstream>
#include <algorithm>

OffsetManager::OffsetManager(const Memory* memory) 
    : m_memory(memory), m_gameBaseAddress(0) {
    // Initialize with default offsets
    initializeDefaultOffsets();
}

void OffsetManager::initializeDefaultOffsets() {
    // Player offsets
    registerOffset("player_position", 0x0, OffsetType::PLAYER);
    registerOffset("player_health", 0x10, OffsetType::PLAYER);
    registerOffset("player_max_health", 0x14, OffsetType::PLAYER);
    registerOffset("player_mana", 0x18, OffsetType::PLAYER);
    registerOffset("player_max_mana", 0x1C, OffsetType::PLAYER);
    registerOffset("player_level", 0x20, OffsetType::PLAYER);
    registerOffset("player_in_combat", 0x24, OffsetType::PLAYER);
    registerOffset("player_is_dead", 0x28, OffsetType::PLAYER);
    registerOffset("player_movement_speed", 0x2C, OffsetType::PLAYER);
    registerOffset("player_class", 0x30, OffsetType::PLAYER);
    
    // Inventory offsets (dynamic - depend on slot index)
    registerDynamicOffset("inventory_slot", 0x100, OffsetType::INVENTORY);
    registerOffset("inventory_item_count", 0x8, OffsetType::INVENTORY);
    registerOffset("inventory_item_quality", 0xC, OffsetType::INVENTORY);
    
    // Monster/Entity offsets
    registerOffset("entity_position", 0x0, OffsetType::MONSTER);
    registerOffset("entity_health", 0x20, OffsetType::MONSTER);
    registerOffset("entity_type", 0x40, OffsetType::MONSTER);
    registerOffset("entity_is_alive", 0x44, OffsetType::MONSTER);
}

// ============ BASIC OFFSET OPERATIONS ============

void OffsetManager::registerOffset(const std::string& name, uintptr_t offset, OffsetType type) {
    m_offsets[name] = OffsetEntry(name, offset, type, false);
}

void OffsetManager::registerDynamicOffset(const std::string& name, uintptr_t baseOffset, OffsetType type) {
    m_offsets[name] = OffsetEntry(name, baseOffset, type, true);
}

uintptr_t OffsetManager::getOffset(const std::string& name) const {
    auto it = m_offsets.find(name);
    if (it != m_offsets.end()) {
        // Formula: final_offset = static_value + dynamic_adjustment
        return it->second.staticValue + it->second.dynamicAdjustment;
    }
    return 0;
}

void OffsetManager::updateDynamicOffset(const std::string& name, uintptr_t adjustment) {
    auto it = m_offsets.find(name);
    if (it != m_offsets.end() && it->second.isDynamic) {
        it->second.dynamicAdjustment = adjustment;
    }
}

// ============ ADDRESS CALCULATION HELPERS ============

uintptr_t OffsetManager::calculateAddress(uintptr_t baseAddress, const std::string& offsetName) const {
    uintptr_t offset = getOffset(offsetName);
    // Formula: final_address = base_address + offset
    return baseAddress + offset;
}

uintptr_t OffsetManager::calculateAddress(uintptr_t baseAddress, uintptr_t offset) {
    // Formula: final_address = base_address + offset
    return baseAddress + offset;
}

uintptr_t OffsetManager::calculateArrayAddress(uintptr_t baseAddress, size_t index, size_t elementSize, const std::string& offsetName) const {
    uintptr_t offset = getOffset(offsetName);
    // Formula: final_address = base_address + (index * element_size) + offset
    return baseAddress + (index * elementSize) + offset;
}

// ============ MULTI-LEVEL POINTER CHAINS ============

uintptr_t OffsetManager::calculatePointerChain(uintptr_t startAddress, const std::vector<std::string>& offsetChain) const {
    if (!m_memory || offsetChain.empty()) {
        return 0;
    }
    
    uintptr_t currentAddress = startAddress;
    
    // Follow the pointer chain
    for (size_t i = 0; i < offsetChain.size(); ++i) {
        uintptr_t offset = getOffset(offsetChain[i]);
        uintptr_t nextAddress = currentAddress + offset;
        
        // If this is not the last offset, read the pointer at this address
        if (i < offsetChain.size() - 1) {
            currentAddress = m_memory->read<uintptr_t>(nextAddress);
            if (currentAddress == 0) {
                return 0; // Null pointer in chain
            }
        } else {
            // Last offset - return the final address
            currentAddress = nextAddress;
        }
    }
    
    return currentAddress;
}

// ============ UTILITY METHODS ============

std::vector<OffsetManager::OffsetEntry> OffsetManager::getOffsetsByType(OffsetType type) const {
    std::vector<OffsetEntry> result;
    for (const auto& pair : m_offsets) {
        if (pair.second.type == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool OffsetManager::hasOffset(const std::string& name) const {
    return m_offsets.find(name) != m_offsets.end();
}

void OffsetManager::removeOffset(const std::string& name) {
    m_offsets.erase(name);
}

void OffsetManager::clearOffsets() {
    m_offsets.clear();
}

bool OffsetManager::validateAddress(uintptr_t address) const {
    if (!m_memory) {
        return false;
    }
    return m_memory->isValidAddress(address);
}

void OffsetManager::printAllOffsets() const {
    std::cout << "=== Registered Offsets ===" << std::endl;
    for (const auto& pair : m_offsets) {
        const auto& entry = pair.second;
        std::cout << "Name: " << entry.name 
                  << ", Static: 0x" << std::hex << entry.staticValue
                  << ", Dynamic: 0x" << entry.dynamicAdjustment
                  << ", Final: 0x" << (entry.staticValue + entry.dynamicAdjustment)
                  << ", Type: " << static_cast<int>(entry.type)
                  << ", Dynamic: " << (entry.isDynamic ? "Yes" : "No") << std::endl;
    }
    std::cout << std::dec; // Reset to decimal
}

// ============ FILE OPERATIONS ============

bool OffsetManager::loadOffsetsFromFile(const std::string& filename) {
    // Implementation for loading offsets from JSON file
    // This would use nlohmann::json to load offset configurations
    return true; // Placeholder
}

bool OffsetManager::saveOffsetsToFile(const std::string& filename) const {
    // Implementation for saving offsets to JSON file
    // This would use nlohmann::json to save offset configurations
    return true; // Placeholder
}
