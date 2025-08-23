#include "ConfigManager.h"
#include <fstream>
#include <iostream>

// Define nlohmann::json since we forward declared it
#include <nlohmann/json.hpp>

ConfigManager::ConfigManager(const std::string& configFile) : m_configFile(configFile) {
    initializeDefaults();
    createDefaultPresets();
}

bool ConfigManager::loadConfig() {
    try {
        std::ifstream file(m_configFile);
        if (!file.is_open()) {
            // Create default config if file doesn't exist
            return saveConfig();
        }
        
        file >> m_jsonConfig;
        jsonToConfig(m_jsonConfig);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load config: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::saveConfig() {
    try {
        m_jsonConfig = configToJson();
        
        std::ofstream file(m_configFile);
        if (!file.is_open()) {
            return false;
        }
        
        file << m_jsonConfig.dump(2);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save config: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::loadFromFile(const std::string& filename) {
    std::string oldFile = m_configFile;
    m_configFile = filename;
    bool result = loadConfig();
    if (!result) {
        m_configFile = oldFile;
    }
    return result;
}

bool ConfigManager::saveToFile(const std::string& filename) {
    std::string oldFile = m_configFile;
    m_configFile = filename;
    bool result = saveConfig();
    m_configFile = oldFile;
    return result;
}

void ConfigManager::loadPreset(const std::string& presetName) {
    auto it = m_presets.find(presetName);
    if (it != m_presets.end()) {
        m_config = it->second;
    }
}

void ConfigManager::saveAsPreset(const std::string& presetName) {
    m_presets[presetName] = m_config;
}

std::vector<std::string> ConfigManager::getAvailablePresets() const {
    std::vector<std::string> presets;
    for (const auto& [name, config] : m_presets) {
        presets.push_back(name);
    }
    return presets;
}

bool ConfigManager::validateConfig() const {
    return isValidFarmMode(m_config.farmMode) &&
           isValidCombatTactics(m_config.combatTactics) &&
           isValidLootFilter(m_config.lootFilter) &&
           isValidRarity(m_config.minimumRarity) &&
           isValidLogLevel(m_config.logLevel);
}

std::vector<std::string> ConfigManager::getConfigErrors() const {
    std::vector<std::string> errors;
    
    if (!isValidFarmMode(m_config.farmMode)) {
        errors.push_back("Invalid farm mode: " + m_config.farmMode);
    }
    
    if (!isValidCombatTactics(m_config.combatTactics)) {
        errors.push_back("Invalid combat tactics: " + m_config.combatTactics);
    }
    
    if (!isValidLootFilter(m_config.lootFilter)) {
        errors.push_back("Invalid loot filter: " + m_config.lootFilter);
    }
    
    if (!isValidRarity(m_config.minimumRarity)) {
        errors.push_back("Invalid minimum rarity: " + m_config.minimumRarity);
    }
    
    if (!isValidLogLevel(m_config.logLevel)) {
        errors.push_back("Invalid log level: " + m_config.logLevel);
    }
    
    return errors;
}

nlohmann::json ConfigManager::configToJson() const {
    nlohmann::json j;
    
    j["general"]["tickRate"] = m_config.tickRate;
    j["general"]["farmMode"] = m_config.farmMode;
    j["general"]["enableLogging"] = m_config.enableLogging;
    j["general"]["logLevel"] = m_config.logLevel;
    
    j["combat"]["engagementRange"] = m_config.engagementRange;
    j["combat"]["retreatHealthPercent"] = m_config.retreatHealthPercent;
    j["combat"]["healHealthPercent"] = m_config.healHealthPercent;
    j["combat"]["combatTactics"] = m_config.combatTactics;
    
    j["navigation"]["movementSpeed"] = m_config.movementSpeed;
    j["navigation"]["stuckThreshold"] = m_config.stuckThreshold;
    j["navigation"]["enablePathfinding"] = m_config.enablePathfinding;
    j["navigation"]["explorationRadius"] = m_config.explorationRadius;
    
    j["loot"]["lootFilter"] = m_config.lootFilter;
    j["loot"]["minimumRarity"] = m_config.minimumRarity;
    j["loot"]["minimumLevel"] = m_config.minimumLevel;
    j["loot"]["minimumValue"] = m_config.minimumValue;
    j["loot"]["enableSeasonalLoot"] = m_config.enableSeasonalLoot;
    
    j["seasonal"]["enableSeasonalContent"] = m_config.enableSeasonalContent;
    j["seasonal"]["currentSeason"] = m_config.currentSeason;
    j["seasonal"]["prioritizeSeasonalRewards"] = m_config.prioritizeSeasonalRewards;
    
    j["safety"]["enableAntiDetection"] = m_config.enableAntiDetection;
    j["safety"]["randomDelayMin"] = m_config.randomDelayMin;
    j["safety"]["randomDelayMax"] = m_config.randomDelayMax;
    j["safety"]["humanizeMovement"] = m_config.humanizeMovement;
    
    j["performance"]["optimizeMemoryUsage"] = m_config.optimizeMemoryUsage;
    j["performance"]["maxEntityCount"] = m_config.maxEntityCount;
    j["performance"]["updateRadius"] = m_config.updateRadius;
    
    return j;
}

void ConfigManager::jsonToConfig(const nlohmann::json& json) {
    if (json.contains("general")) {
        const auto& general = json["general"];
        if (general.contains("tickRate")) m_config.tickRate = general["tickRate"];
        if (general.contains("farmMode")) m_config.farmMode = general["farmMode"];
        if (general.contains("enableLogging")) m_config.enableLogging = general["enableLogging"];
        if (general.contains("logLevel")) m_config.logLevel = general["logLevel"];
    }
    
    if (json.contains("combat")) {
        const auto& combat = json["combat"];
        if (combat.contains("engagementRange")) m_config.engagementRange = combat["engagementRange"];
        if (combat.contains("retreatHealthPercent")) m_config.retreatHealthPercent = combat["retreatHealthPercent"];
        if (combat.contains("healHealthPercent")) m_config.healHealthPercent = combat["healHealthPercent"];
        if (combat.contains("combatTactics")) m_config.combatTactics = combat["combatTactics"];
    }
    
    // Continue for other sections...
}

void ConfigManager::initializeDefaults() {
    m_config = BotConfig{}; // Use default values from struct
}

void ConfigManager::createDefaultPresets() {
    m_presets["aggressive"] = createAggressivePreset();
    m_presets["safe"] = createSafePreset();
    m_presets["balanced"] = createBalancedPreset();
    m_presets["seasonal"] = createSeasonalPreset();
    m_presets["boss"] = createBossPreset();
}

ConfigManager::BotConfig ConfigManager::createAggressivePreset() const {
    BotConfig config = m_config;
    config.farmMode = "aggressive";
    config.combatTactics = "aggressive";
    config.engagementRange = 30.0f;
    config.retreatHealthPercent = 0.2f;
    return config;
}

ConfigManager::BotConfig ConfigManager::createSafePreset() const {
    BotConfig config = m_config;
    config.farmMode = "safe";
    config.combatTactics = "defensive";
    config.engagementRange = 20.0f;
    config.retreatHealthPercent = 0.5f;
    return config;
}

ConfigManager::BotConfig ConfigManager::createBalancedPreset() const {
    BotConfig config = m_config;
    config.farmMode = "balanced";
    config.combatTactics = "balanced";
    config.engagementRange = 25.0f;
    config.retreatHealthPercent = 0.3f;
    return config;
}

ConfigManager::BotConfig ConfigManager::createSeasonalPreset() const {
    BotConfig config = m_config;
    config.enableSeasonalContent = true;
    config.prioritizeSeasonalRewards = true;
    config.enableSeasonalLoot = true;
    return config;
}

ConfigManager::BotConfig ConfigManager::createBossPreset() const {
    BotConfig config = m_config;
    config.combatTactics = "boss_only";
    config.engagementRange = 35.0f;
    config.retreatHealthPercent = 0.4f;
    return config;
}

bool ConfigManager::isValidFarmMode(const std::string& mode) const {
    return mode == "aggressive" || mode == "safe" || mode == "balanced";
}

bool ConfigManager::isValidCombatTactics(const std::string& tactics) const {
    return tactics == "aggressive" || tactics == "defensive" || tactics == "balanced" || tactics == "boss_only";
}

bool ConfigManager::isValidLootFilter(const std::string& filter) const {
    return filter == "aggressive" || filter == "safe" || filter == "balanced" || filter == "seasonal";
}

bool ConfigManager::isValidRarity(const std::string& rarity) const {
    return rarity == "normal" || rarity == "magic" || rarity == "rare" || 
           rarity == "legendary" || rarity == "mythic" || rarity == "unique";
}

bool ConfigManager::isValidLogLevel(const std::string& level) const {
    return level == "debug" || level == "info" || level == "warning" || 
           level == "error" || level == "critical";
}
