#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <regex>

/**
 * @brief Manages item filtering and loot prioritization
 */
class LootFilter {
public:
    enum class ItemRarity {
        NORMAL = 0,
        MAGIC = 1,
        RARE = 2,
        LEGENDARY = 3,
        MYTHIC = 4,
        UNIQUE = 5
    };

    enum class ItemType {
        UNKNOWN,
        WEAPON,
        ARMOR,
        ACCESSORY,
        CONSUMABLE,
        CURRENCY,
        GEM,
        MATERIAL,
        QUEST_ITEM,
        SEASONAL_ITEM
    };

    struct ItemInfo {
        std::string name;
        ItemType type;
        ItemRarity rarity;
        int level;
        int value;                     // Estimated value
        bool isIdentified;
        std::vector<std::string> affixes; // Item modifiers
        
        // Position in world
        float x, y, z;
        
        // Filter results
        bool shouldLoot = false;
        int priority = 0;              // Higher = more important
        std::string filterReason;      // Why it was included/excluded
    };

    struct FilterRule {
        std::string name;
        std::function<bool(const ItemInfo&)> condition;
        int priority;
        bool enabled;
        std::string description;
    };

private:
    std::vector<FilterRule> m_rules;
    std::unordered_map<std::string, bool> m_itemBlacklist;
    std::unordered_map<std::string, int> m_itemPriorities;
    
    // Configuration
    bool m_enableCurrencyFilter = true;
    bool m_enableRarityFilter = true;
    bool m_enableValueFilter = true;
    bool m_enableSeasonalFilter = true;
    
    ItemRarity m_minRarity = ItemRarity::MAGIC;
    int m_minLevel = 1;
    int m_minValue = 100;
    
    // Statistics
    uint64_t m_itemsFiltered = 0;
    uint64_t m_itemsLooted = 0;
    std::unordered_map<ItemRarity, uint64_t> m_lootedByRarity;

public:
    LootFilter();
    
    // Core filtering
    bool shouldLootItem(const ItemInfo& item);
    std::vector<ItemInfo> filterItems(const std::vector<ItemInfo>& items);
    std::vector<ItemInfo> prioritizeItems(const std::vector<ItemInfo>& items);
    
    // Rule management
    void addRule(const FilterRule& rule);
    void removeRule(const std::string& ruleName);
    void enableRule(const std::string& ruleName, bool enabled);
    std::vector<FilterRule> getRules() const { return m_rules; }
    
    // Configuration methods
    void setMinimumRarity(ItemRarity rarity) { m_minRarity = rarity; }
    void setMinimumLevel(int level) { m_minLevel = level; }
    void setMinimumValue(int value) { m_minValue = value; }
    
    void enableCurrencyFilter(bool enable) { m_enableCurrencyFilter = enable; }
    void enableRarityFilter(bool enable) { m_enableRarityFilter = enable; }
    void enableValueFilter(bool enable) { m_enableValueFilter = enable; }
    void enableSeasonalFilter(bool enable) { m_enableSeasonalFilter = enable; }
    
    // Blacklist management
    void addToBlacklist(const std::string& itemName);
    void removeFromBlacklist(const std::string& itemName);
    bool isBlacklisted(const std::string& itemName) const;
    
    // Priority management
    void setItemPriority(const std::string& itemName, int priority);
    int getItemPriority(const std::string& itemName) const;
    
    // Preset filters
    void loadAggressiveFilter();    // Loot everything valuable
    void loadSafeFilter();          // Only high-value items
    void loadBalancedFilter();      // Default balanced approach
    void loadSeasonalFilter();      // Focus on seasonal items
    void loadBossFilter();          // Optimized for boss drops
    
    // Statistics
    uint64_t getItemsFiltered() const { return m_itemsFiltered; }
    uint64_t getItemsLooted() const { return m_itemsLooted; }
    std::unordered_map<ItemRarity, uint64_t> getLootStatsByRarity() const { return m_lootedByRarity; }
    
    // Utility methods
    static ItemRarity parseRarity(const std::string& rarityStr);
    static ItemType parseItemType(const std::string& typeStr);
    static std::string rarityToString(ItemRarity rarity);
    static std::string itemTypeToString(ItemType type);
    
    // Item analysis
    int calculateItemValue(const ItemInfo& item);
    bool hasValuableAffixes(const ItemInfo& item);
    bool isSeasonalItem(const ItemInfo& item);
    bool isCurrencyItem(const ItemInfo& item);

private:
    void initializeDefaultRules();
    void initializeItemPriorities();
    bool evaluateRule(const FilterRule& rule, const ItemInfo& item);
    int calculateBasePriority(const ItemInfo& item);
    void updateStatistics(const ItemInfo& item, bool looted);
    
    // Predefined rule functions
    bool isHighValueCurrency(const ItemInfo& item);
    bool isUpgradeCandidate(const ItemInfo& item);
    bool hasGoodAffixes(const ItemInfo& item);
    bool isRareBase(const ItemInfo& item);
    bool isSeasonalReward(const ItemInfo& item);
};
