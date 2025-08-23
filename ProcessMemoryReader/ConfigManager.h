#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

// Forward declare nlohmann::json to avoid including the header
namespace nlohmann {
    class json;
}

/**
 * @brief Manages bot configuration and settings
 */
class ConfigManager {
public:
    struct BotConfig {
        // General settings
        int tickRate = 50;             // Bot update rate in ms
        std::string farmMode = "balanced"; // aggressive, safe, balanced
        bool enableLogging = true;
        std::string logLevel = "info";
        
        // Combat settings
        float engagementRange = 25.0f;
        float retreatHealthPercent = 0.3f;
        float healHealthPercent = 0.5f;
        std::string combatTactics = "balanced";
        
        // Navigation settings
        float movementSpeed = 1.0f;
        float stuckThreshold = 1.0f;
        bool enablePathfinding = true;
        float explorationRadius = 20.0f;
        
        // Loot settings
        std::string lootFilter = "balanced";
        std::string minimumRarity = "magic";
        int minimumLevel = 1;
        int minimumValue = 100;
        bool enableSeasonalLoot = true;
        
        // Seasonal settings
        bool enableSeasonalContent = true;
        std::string currentSeason = "auto";
        bool prioritizeSeasonalRewards = true;
        
        // Safety settings
        bool enableAntiDetection = true;
        int randomDelayMin = 50;
        int randomDelayMax = 200;
        bool humanizeMovement = true;
        
        // Performance settings
        bool optimizeMemoryUsage = true;
        int maxEntityCount = 1000;
        float updateRadius = 50.0f;
    };

    struct KeyBindings {
        // Movement
        int moveKey = VK_RBUTTON;      // Right mouse button
        int forceMove = VK_SHIFT;      // Force move modifier
        
        // Combat abilities (F1-F12)
        int ability1 = VK_F1;
        int ability2 = VK_F2;
        int ability3 = VK_F3;
        int ability4 = VK_F4;
        int ability5 = VK_F5;
        int ability6 = VK_F6;
        
        // Utility
        int openInventory = 'I';
        int openMap = 'M';
        int usePotion = 'R';
        int teleport = 'T';
        int interact = 'E';
        
        // Emergency
        int emergencyStop = VK_F9;
        int pauseBot = VK_F10;
        int quitGame = VK_F4; // Alt+F4 for emergency quit
    };

private:
    std::string m_configFile;
    BotConfig m_config;
    KeyBindings m_keyBindings;
    nlohmann::json m_jsonConfig;
    
    // Default configurations
    std::unordered_map<std::string, BotConfig> m_presets;

public:
    ConfigManager(const std::string& configFile = "config.json");
    
    // Configuration loading/saving
    bool loadConfig();
    bool saveConfig();
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename);
    
    // Configuration access
    const BotConfig& getConfig() const { return m_config; }
    const KeyBindings& getKeyBindings() const { return m_keyBindings; }
    void setConfig(const BotConfig& config) { m_config = config; }
    void setKeyBindings(const KeyBindings& bindings) { m_keyBindings = bindings; }
    
    // Individual setting methods
    void setTickRate(int rate) { m_config.tickRate = rate; }
    void setFarmMode(const std::string& mode) { m_config.farmMode = mode; }
    void setCombatTactics(const std::string& tactics) { m_config.combatTactics = tactics; }
    void setLootFilter(const std::string& filter) { m_config.lootFilter = filter; }
    void setMinimumRarity(const std::string& rarity) { m_config.minimumRarity = rarity; }
    
    // Preset management
    void loadPreset(const std::string& presetName);
    void saveAsPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
    // Validation
    bool validateConfig() const;
    std::vector<std::string> getConfigErrors() const;
    
    // JSON utilities
    nlohmann::json configToJson() const;
    void jsonToConfig(const nlohmann::json& json);

private:
    void initializeDefaults();
    void createDefaultPresets();
    BotConfig createAggressivePreset() const;
    BotConfig createSafePreset() const;
    BotConfig createBalancedPreset() const;
    BotConfig createSeasonalPreset() const;
    BotConfig createBossPreset() const;
    
    // Validation helpers
    bool isValidFarmMode(const std::string& mode) const;
    bool isValidCombatTactics(const std::string& tactics) const;
    bool isValidLootFilter(const std::string& filter) const;
    bool isValidRarity(const std::string& rarity) const;
    bool isValidLogLevel(const std::string& level) const;
};
