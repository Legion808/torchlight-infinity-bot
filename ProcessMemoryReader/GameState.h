#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>

// Forward declarations
class Memory;
class OffsetManager;

/**
 * @brief Represents the current state of the game world
 */
class GameState {
public:
    struct PlayerData {
        float x, y, z;                  // Position
        float health, maxHealth;        // Health values
        float mana, maxMana;           // Mana values
        int level;                     // Character level
        bool inCombat;                 // Combat state
        bool isDead;                   // Death state
        float movementSpeed;           // Current movement speed
        int characterClass;            // Class identifier
    };

    struct MapData {
        std::string mapName;           // Current map name
        int mapTier;                   // Map difficulty tier
        bool isCompleted;              // Map completion status
        float completionPercent;       // Progress percentage
        std::vector<std::pair<float, float>> exploredAreas; // Areas already explored
        bool hasBoss;                  // Whether map has boss
        bool bossDefeated;            // Boss status
    };

    struct SeasonData {
        std::string seasonName;        // Current season
        int seasonLevel;              // Season progress level
        bool hasActiveEvent;          // Active seasonal event
        std::string eventType;        // Type of current event
        std::chrono::time_point<std::chrono::steady_clock> eventEndTime; // Event duration
    };

private:
    const Memory* m_memory;
    std::unique_ptr<OffsetManager> m_offsetManager;
    
    // Game state data
    PlayerData m_player;
    MapData m_currentMap;
    SeasonData m_season;
    
    // Memory addresses (these will need to be found via pattern scanning)
    uintptr_t m_playerBaseAddress = 0;
    uintptr_t m_mapDataAddress = 0;
    uintptr_t m_seasonDataAddress = 0;

public:
    explicit GameState(const Memory* memory);
    
    // Update methods
    bool update();
    bool updatePlayerData();
    bool updateMapData();
    bool updateSeasonData();
    
    // Getter methods
    const PlayerData& getPlayer() const { return m_player; }
    const MapData& getCurrentMap() const { return m_currentMap; }
    const SeasonData& getSeason() const { return m_season; }
    
    // Utility methods
    bool isPlayerAlive() const { return !m_player.isDead && m_player.health > 0; }
    bool isPlayerInCombat() const { return m_player.inCombat; }
    bool isMapCompleted() const { return m_currentMap.isCompleted; }
    bool hasActiveSeason() const { return m_season.hasActiveEvent; }
    
    // Position utilities
    float getDistanceToPoint(float x, float y) const;
    bool isPlayerNearPosition(float x, float y, float threshold = 5.0f) const;
    
    // Memory address management
    bool findGameAddresses();
    void setPlayerBaseAddress(uintptr_t address) { m_playerBaseAddress = address; }
    void setMapDataAddress(uintptr_t address) { m_mapDataAddress = address; }
    void setSeasonDataAddress(uintptr_t address) { m_seasonDataAddress = address; }

private:
    bool scanForPlayerBase();
    bool scanForMapData();
    bool scanForSeasonData();
    bool validateAddress(uintptr_t address) const;
};
