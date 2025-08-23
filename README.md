# Torchlight Infinity Farm Bot

A fully functional farming bot for Torchlight Infinity, created using Path of Exile bot architecture as reference.

## Core Features

### 🎯 Map Farming and Clearing
- **Automatic map traversal** with intelligent navigation
- **Monster killing** with advanced targeting system
- **Support for randomly generated maps** through dynamic mapping
- **Adaptive tactics** based on mob types

### 💎 Loot Collection
- **Item filtering** by rarity, type, and value
- **Customizable filters** for different playstyles
- **Automatic ignoring** of unwanted items
- **Prioritization** of valuable items

### 🌟 Seasonal Mechanics
- **Automatic completion** of seasonal objectives
- **Integration with unique mechanics** of current season
- **Adaptive behavior** for seasonal content

### 👹 Boss Fighting
- **Boss finding and killing** with specialized tactics
- **Dodging dangerous attacks** through pattern analysis
- **Optimized strategies** for different boss types

### ⚙️ Flexibility and Customization
- **Behavior configuration** (aggressive/safe/balanced farming)
- **Modular architecture** for easy extension
- **Configuration files** for personalization

## Technical Features

### 🔍 Working with "Raw" Game
- **Advanced memory scanning** for unstable code
- **Dynamic structure detection** of data
- **Resilience to game changes**
- **Automatic recovery** on failures

### 🛡️ Security
- **Movement humanization** and actions
- **Random delays** between actions
- **Anti-detection mechanisms**
- **Minimal system load**

## Project Architecture

```
ProcessMemoryReader/
├── TorchlightBot.h/cpp          # Main bot class
├── GameState.h                  # Game world state
├── EntityManager.h              # Entity management
├── NavigationSystem.h           # Navigation and pathfinding
├── CombatSystem.h              # Combat system
├── LootFilter.h                # Loot filtering
├── InputManager.h              # Input management
├── Logger.h                    # Logging system
├── ConfigManager.h             # Configuration management
├── Memory.h/cpp                # Process memory operations
├── Process.h/cpp               # Process management
├── config.json                 # Configuration file
└── README.md                   # This documentation
```

## Installation and Setup

### Requirements
- Windows 10/11 (x64)
- Visual Studio 2019 or newer
- Torchlight Infinity (running game)

### Building the Project
1. Open `ProcessMemoryReader.sln` in Visual Studio
2. Install NuGet package `nlohmann.json`
3. Select `Release|x64` configuration
4. Build the project (Build → Build Solution)

### First Run
1. Launch Torchlight Infinity
2. Run the bot as administrator
3. Bot will automatically find the game process
4. Configure parameters through menu or config.json

## Usage

### Main Menu
```
=== Torchlight Infinity Farm Bot ===
1. Start Bot                    # Start bot
2. Stop Bot                     # Stop bot
3. Pause/Resume Bot             # Pause/resume
4. Set Farm Mode                # Configure farm mode
5. Show Statistics              # Show statistics
6. Show Bot Status              # Bot status
7. Exit                         # Exit
```

### Farm Modes
- **Aggressive**: Fast clearing, attack all mobs
- **Safe**: Careful approach, retreat on low HP
- **Balanced**: Balanced mode (default)

### Hotkeys
- `F9` - Emergency stop
- `F10` - Pause bot
- `Alt+F4` - Emergency game exit

## Configuration

### config.json
Main configuration file contains:

```json
{
  "general": {
    "tickRate": 50,              // Update rate (ms)
    "farmMode": "balanced",      // Farm mode
    "enableLogging": true,       // Enable logging
    "logLevel": "info"          // Log level
  },
  "combat": {
    "engagementRange": 25.0,     // Attack range
    "retreatHealthPercent": 0.3, // HP % for retreat
    "healHealthPercent": 0.5,    // HP % for healing
    "combatTactics": "balanced"  // Combat tactics
  },
  // ... other sections
}
```

### Loot Filters
Configuration through LootFilter class:
- Minimum item rarity
- Filter by item type
- Value estimation
- Item blacklist

### Presets
Available configuration presets:
- `aggressive` - Maximum farming speed
- `safe` - Safe farming
- `balanced` - Balanced approach
- `seasonal` - Focus on seasonal content
- `boss` - Boss optimization

## Logging System

### Log Levels
- `DEBUG` - Detailed debug information
- `INFO` - General operation information
- `WARNING` - Warnings
- `ERROR` - Errors
- `CRITICAL` - Critical errors

### Log Files
Logs are saved in `logs/` folder:
- `bot_YYYY-MM-DD.log` - Main logs
- Automatic log rotation
- Old file cleanup

## Statistics

The bot collects the following statistics:
- Runtime
- Monsters killed
- Bosses defeated
- Items collected
- Maps cleared
- Efficiency (kills/hour, loot/hour)

## Security and Anti-Detection

### Humanization
- Random delays between actions
- Natural mouse movement trajectories
- Variation in ability timing

### Monitoring
- System performance tracking
- CPU/memory load control
- Automatic pause on high activity

## Troubleshooting

### Common Issues

**Bot doesn't find game process**
- Ensure the game is running
- Run bot as administrator
- Check process name in code

**Bot doesn't move/attack**
- Check key bindings in config.json
- Ensure game is in focus
- Verify input access permissions

**Memory reading errors**
- Restart the bot
- Update memory addresses
- Check game version compatibility

**Poor performance**
- Reduce tickRate in config
- Limit maxEntityCount
- Optimize updateRadius

## Development and Extension

### Adding New Features
1. Create new class in appropriate module
2. Add integration to TorchlightBot
3. Update configuration if needed
4. Add logging

### Module Structure
- **GameState** - Game state and player data
- **EntityManager** - World entity management
- **NavigationSystem** - Navigation and pathfinding  
- **CombatSystem** - Combat logic and tactics
- **LootFilter** - Item filtering and evaluation
- **InputManager** - User input simulation

## Support

### Community
- GitHub Issues for bug reports
- Forum for discussions
- Wiki for documentation

### Contributing
1. Fork the repository
2. Create feature branch
3. Make changes
4. Create Pull Request

## License

This project is distributed under the MIT License. See LICENSE file for details.

## Disclaimer

This bot is created for educational purposes only. Using bots may violate game rules. Use at your own risk.

## Version History

### v1.0.0 (Current)
- Basic farming functionality
- Navigation and combat systems
- Loot filtering
- Seasonal content support
- Logging system
- Configuration files

### Planned Features
- Trading system
- Multi-account support
- Web management interface
- Machine learning optimization
- External API integration
