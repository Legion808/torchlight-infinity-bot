#include "TorchlightBot.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void printMenu() {
    std::cout << "\n=== Torchlight Infinity Farm Bot ===\n";
    std::cout << "1. Start Bot\n";
    std::cout << "2. Stop Bot\n";
    std::cout << "3. Pause/Resume Bot\n";
    std::cout << "4. Set Farm Mode\n";
    std::cout << "5. Show Statistics\n";
    std::cout << "6. Show Bot Status\n";
    std::cout << "7. Exit\n";
    std::cout << "Choice: ";
}

void printFarmModeMenu() {
    std::cout << "\n=== Farm Mode Selection ===\n";
    std::cout << "1. Aggressive (Fight everything, fast clearing)\n";
    std::cout << "2. Safe (Careful engagement, retreat when low health)\n";
    std::cout << "3. Balanced (Default balanced approach)\n";
    std::cout << "Choice: ";
}

std::string botStateToString(TorchlightBot::BotState state) {
    switch (state) {
        case TorchlightBot::BotState::IDLE: return "IDLE";
        case TorchlightBot::BotState::FARMING: return "FARMING";
        case TorchlightBot::BotState::COMBAT: return "COMBAT";
        case TorchlightBot::BotState::LOOTING: return "LOOTING";
        case TorchlightBot::BotState::NAVIGATING: return "NAVIGATING";
        case TorchlightBot::BotState::BOSS_FIGHT: return "BOSS_FIGHT";
        case TorchlightBot::BotState::SEASONAL_ACTIVITY: return "SEASONAL_ACTIVITY";
        case TorchlightBot::BotState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string farmModeToString(TorchlightBot::FarmMode mode) {
    switch (mode) {
        case TorchlightBot::FarmMode::AGGRESSIVE: return "AGGRESSIVE";
        case TorchlightBot::FarmMode::SAFE: return "SAFE";
        case TorchlightBot::FarmMode::BALANCED: return "BALANCED";
        default: return "UNKNOWN";
    }
}

int main() {
    std::cout << "Initializing Torchlight Infinity Farm Bot...\n";
    
    TorchlightBot bot;
    
    if (!bot.initialize()) {
        std::cout << "Failed to initialize bot. Please make sure Torchlight Infinity is running.\n";
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }
    
    std::cout << "Bot initialized successfully!\n";
    
    bool running = true;
    bool botPaused = false;
    
    while (running) {
        printMenu();
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                std::cout << "Starting bot...\n";
                bot.start();
                std::cout << "Bot started! Use option 2 to stop.\n";
                break;
            }
            
            case 2: {
                std::cout << "Stopping bot...\n";
                bot.stop();
                std::cout << "Bot stopped.\n";
                break;
            }
            
            case 3: {
                if (bot.getCurrentState() == TorchlightBot::BotState::IDLE) {
                    std::cout << "Resuming bot...\n";
                    bot.resume();
                    botPaused = false;
                } else {
                    std::cout << "Pausing bot...\n";
                    bot.pause();
                    botPaused = true;
                }
                break;
            }
            
            case 4: {
                printFarmModeMenu();
                int modeChoice;
                std::cin >> modeChoice;
                
                switch (modeChoice) {
                    case 1:
                        bot.setFarmMode(TorchlightBot::FarmMode::AGGRESSIVE);
                        std::cout << "Farm mode set to AGGRESSIVE\n";
                        break;
                    case 2:
                        bot.setFarmMode(TorchlightBot::FarmMode::SAFE);
                        std::cout << "Farm mode set to SAFE\n";
                        break;
                    case 3:
                        bot.setFarmMode(TorchlightBot::FarmMode::BALANCED);
                        std::cout << "Farm mode set to BALANCED\n";
                        break;
                    default:
                        std::cout << "Invalid choice.\n";
                        break;
                }
                break;
            }
            
            case 5: {
                auto stats = bot.getStatistics();
                std::cout << "\n=== Bot Statistics ===\n";
                std::cout << "Runtime: " << stats.runtime.count() << " seconds\n";
                std::cout << "Maps Cleared: " << stats.mapsCleared << "\n";
                std::cout << "Monsters Killed: " << stats.monstersKilled << "\n";
                std::cout << "Bosses Killed: " << stats.bossesKilled << "\n";
                std::cout << "Items Looted: " << stats.itemsLooted << "\n";
                
                if (stats.runtime.count() > 0) {
                    std::cout << "Monsters/Hour: " << (stats.monstersKilled * 3600) / stats.runtime.count() << "\n";
                    std::cout << "Items/Hour: " << (stats.itemsLooted * 3600) / stats.runtime.count() << "\n";
                }
                break;
            }
            
            case 6: {
                std::cout << "\n=== Bot Status ===\n";
                std::cout << "Current State: " << botStateToString(bot.getCurrentState()) << "\n";
                std::cout << "Farm Mode: " << farmModeToString(bot.getFarmMode()) << "\n";
                std::cout << "Status: " << (botPaused ? "PAUSED" : "RUNNING") << "\n";
                break;
            }
            
            case 7: {
                std::cout << "Shutting down bot...\n";
                bot.stop();
                running = false;
                break;
            }
            
            default: {
                std::cout << "Invalid choice. Please try again.\n";
                break;
            }
        }
        
        // Small delay to prevent console spam
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Bot shutdown complete. Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();
    
    return 0;
}
