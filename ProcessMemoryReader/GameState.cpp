#include "GameState.h"
#include "Memory.h"
#include "OffsetManager.h"
#include <cmath>

GameState::GameState(const Memory* memory) : m_memory(memory) {
    // Initialize offset manager
    m_offsetManager = std::make_unique<OffsetManager>(memory);
    
    // Initialize player data
    m_player = {};
    m_currentMap = {};
    m_season = {};
}

bool GameState::update() {
    bool success = true;
    
    success &= updatePlayerData();
    success &= updateMapData();
    success &= updateSeasonData();
    
    return success;
}

bool GameState::updatePlayerData() {
    if (!m_memory || m_playerBaseAddress == 0) {
        return false;
    }
    
    try {
        // === EXAMPLE: Using Dynamic Offset System ===
        
        // Method 1: Using OffsetManager (recommended)
        auto positionAddress = m_offsetManager->calculateAddress(m_playerBaseAddress, "player_position");
        auto position = m_memory->readArray<float>(positionAddress, 3);
        m_player.x = position[0];
        m_player.y = position[1];
        m_player.z = position[2];
        
        // Method 2: Manual calculation (what you asked about!)
        // Formula: new_address = base_address + offset
        uintptr_t healthOffset = m_offsetManager->getOffset("player_health");
        uintptr_t healthAddress = m_playerBaseAddress + healthOffset;  // <- This is your formula!
        m_player.health = m_memory->read<float>(healthAddress);
        
        // Method 3: Direct offset values (old way)
        m_player.maxHealth = m_memory->read<float>(m_playerBaseAddress + 0x14);
        
        // Using OffsetManager for remaining data
        m_player.mana = m_memory->read<float>(
            m_offsetManager->calculateAddress(m_playerBaseAddress, "player_mana")
        );
        m_player.maxMana = m_memory->read<float>(
            m_offsetManager->calculateAddress(m_playerBaseAddress, "player_max_mana")
        );
        m_player.level = m_memory->read<int>(
            m_offsetManager->calculateAddress(m_playerBaseAddress, "player_level")
        );
        m_player.inCombat = m_memory->read<bool>(
            m_offsetManager->calculateAddress(m_playerBaseAddress, "player_in_combat")
        );
        m_player.isDead = m_memory->read<bool>(
            m_offsetManager->calculateAddress(m_playerBaseAddress, "player_is_dead")
        );
        m_player.movementSpeed = m_memory->read<float>(
            m_offsetManager->calculateAddress(m_playerBaseAddress, "player_movement_speed")
        );
        m_player.characterClass = m_memory->read<int>(
            m_offsetManager->calculateAddress(m_playerBaseAddress, "player_class")
        );
        
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool GameState::updateMapData() {
    if (!m_memory || m_mapDataAddress == 0) {
        return false;
    }
    
    try {
        // This is a placeholder - actual implementation would depend on game's memory structure
        // For now, return basic success
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool GameState::updateSeasonData() {
    if (!m_memory || m_seasonDataAddress == 0) {
        return false;
    }
    
    try {
        // This is a placeholder - actual implementation would depend on game's memory structure
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

float GameState::getDistanceToPoint(float x, float y) const {
    float dx = m_player.x - x;
    float dy = m_player.y - y;
    return std::sqrt(dx * dx + dy * dy);
}

bool GameState::isPlayerNearPosition(float x, float y, float threshold) const {
    return getDistanceToPoint(x, y) <= threshold;
}

bool GameState::findGameAddresses() {
    // This is a critical function that would need reverse engineering
    // For now, return false to indicate addresses need to be found
    return scanForPlayerBase() && scanForMapData() && scanForSeasonData();
}

bool GameState::scanForPlayerBase() {
    // Placeholder for pattern scanning to find player base address
    // This would involve signature scanning or other reverse engineering techniques
    
    // For development purposes, set a dummy address
    m_playerBaseAddress = 0x1000000; // This would be found via scanning
    return m_playerBaseAddress != 0;
}

bool GameState::scanForMapData() {
    // Placeholder for finding map data structures
    m_mapDataAddress = 0x2000000; // This would be found via scanning
    return m_mapDataAddress != 0;
}

bool GameState::scanForSeasonData() {
    // Placeholder for finding seasonal data
    m_seasonDataAddress = 0x3000000; // This would be found via scanning
    return m_seasonDataAddress != 0;
}

bool GameState::validateAddress(uintptr_t address) const {
    return m_memory && m_memory->isValidAddress(address, sizeof(void*));
}
