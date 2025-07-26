# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Farert is an Android application for calculating Japanese railway fares and route planning. The app uses SQLite databases containing railway route information and calculates fares based on various factors including distance, tax rates, and special routes like the Osaka Loop Line.

## Development Commands

### Build Commands
```bash
# Build the project
./gradlew build

# Build debug APK
./gradlew assembleDebug

# Build release APK  
./gradlew assembleRelease

# Clean build
./gradlew clean
```

### Testing Commands
```bash
# Run unit tests
./gradlew test

# Run instrumented tests (requires connected device/emulator)
./gradlew connectedAndroidTest

# Run specific test class
./gradlew test --tests "org.sutezo.farert.ExampleUnitTest"
```

### Development Tools
```bash
# Check dependencies
./gradlew dependencies

# Lint checks
./gradlew lint

# Install on connected device
./gradlew installDebug
```

## Architecture

### Core Components

**Application Layer (`org.sutezo.farert`)**
- `FarertApp.kt` - Application class managing global state and database initialization
- `MainActivity.kt` - Main activity with route planning interface and RecyclerView adapter
- `DatabaseOpenHelper.kt` - SQLite database management with multi-version support

**Route Calculation Engine (`org.sutezo.alps`)**
- `Route.java` - Core route calculation logic and data structures
- `RouteDB.java` - Database access layer with singleton pattern
- `CalcRoute.java` - Fare calculation algorithms
- `Station.java`, `KM.java`, `Fare.java` - Data models for railway entities

**UI Activities**
- `TerminalSelectActivity.kt` - Station selection interface
- `LineListActivity.kt` - Railway line selection
- `StationListActivity.kt` - Station list with fragments
- `ResultViewActivity.kt` - Fare calculation results display
- `ArchiveRouteActivity.kt` - Route saving/loading functionality
- `SettingsActivity.kt` - User preferences and database selection

### Key Architecture Patterns

1. **Database Versioning**: Multiple railway databases (2014-2022) with different tax rates (5%, 8%, 10%)
2. **Route State Management**: Global route state managed through `FarertApp` singleton
3. **Fragment-based UI**: Uses support library fragments for complex UI components
4. **Recycler View Pattern**: Custom adapter for route display with different view types

### Database Structure
- Multiple embedded SQLite databases in assets
- Database selection based on tax rate and data version
- Route calculation considers JR lines, private railways, and special routing rules

### Build Configuration
- **Target SDK**: 36 (Android 15)
- **Min SDK**: 24 (Android 7.0)
- **Build Tools**: Android Gradle Plugin 8.11.1
- **Kotlin**: 2.0.21 with Compose support
- **Dependencies**: AndroidX libraries, Material Design, Compose BOM

### Testing
- Unit tests in `src/test/java`
- Instrumented tests in `src/androidTest/java`
- Uses JUnit 4 and Espresso for UI testing