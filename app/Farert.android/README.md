# Farert Android

Farert is a modern Android application for calculating Japanese railway fares and route planning. Built with Jetpack Compose and Material 3 design, it provides comprehensive fare calculations across Japan's complex railway network.

## Features

- **Route Planning**: Calculate optimal routes between any two stations in Japan
- **Fare Calculation**: Accurate fare computation considering various factors:
  - Distance-based pricing
  - Tax rates (5%, 8%, 10%)
  - Special routing rules (Osaka Loop Line, etc.)
  - JR lines and private railways
- **Multiple Database Support**: Historical railway data from 2014-2022
- **Route Management**: Save and organize calculated routes
- **Modern UI**: Built with Jetpack Compose and Material 3

## Technical Stack

- **Language**: Kotlin
- **UI Framework**: Jetpack Compose
- **Design System**: Material 3
- **Architecture**: MVVM with State Holders
- **Database**: SQLite with embedded railway data
- **Build System**: Gradle with Kotlin DSL
- **Min SDK**: 24 (Android 7.0)
- **Target SDK**: 36 (Android 15)

## Project Structure

```
app/src/main/java/org/sutezo/
├── alps/                    # Route calculation engine
│   ├── CalcRoute.java      # Fare calculation algorithms
│   ├── Route.java          # Core route logic
│   ├── RouteDB.java        # Database access layer
│   └── Station.java        # Railway data models
├── farert/                 # Application layer
│   ├── FarertApp.kt        # Application class
│   ├── MainActivity.kt     # Main activity
│   └── ui/                 # UI components
│       ├── components/     # Reusable UI components
│       ├── compose/        # Screen composables
│       ├── state/          # State management
│       └── theme/          # Material 3 theming
```

## Getting Started

### Prerequisites

- Android Studio Dolphin or later
- JDK 11 or later
- Android SDK with API level 36

### Required Files (Not in Repository)

The following files are not included in the repository and must be provided separately:

#### **app/gradle.properties**
Create this file with your signing configuration:
```properties
# Signing config for release builds
FARERT_STORE_FILE=farert-upload-keystore.jks
FARERT_STORE_PASSWORD=your_store_password
FARERT_KEY_ALIAS=your_key_alias
FARERT_KEY_PASSWORD=your_key_password
```

#### **app/farert-upload-keystore.jks**
Generate or obtain the Android signing keystore file:
```bash
# Generate new keystore (if needed)
keytool -genkey -v -keystore farert-upload-keystore.jks -keyalg RSA -keysize 2048 -validity 10000 -alias farert
```

Place both files in the `app/` directory before building release versions.

### Building the Project

```bash
# Clone the repository
git clone [repository-url]
cd Farert.android

# Build debug APK
./gradlew assembleDebug

# Build release APK
./gradlew assembleRelease

# Install on connected device
./gradlew installDebug
```

### Development Commands

```bash
# Run unit tests
./gradlew test

# Run instrumented tests (requires device/emulator)
./gradlew connectedAndroidTest

# Run lint checks
./gradlew lint

# Clean build
./gradlew clean
```

## Architecture

### Route Calculation Engine

The core calculation logic is implemented in the `org.sutezo.alps` package:

- **RouteDB.java**: Singleton database access with multi-version support
- **CalcRoute.java**: Implements complex fare calculation algorithms
- **Route.java**: Manages route data structures and pathfinding
- **Station.java**: Represents railway stations and connections

### UI Architecture

Built with modern Android architecture patterns:

- **Compose UI**: Declarative UI with Material 3 components
- **State Holders**: Manage screen state and business logic
- **Single Activity**: Navigation handled through Compose Navigation
- **Reactive State**: Uses Kotlin Flow and Compose State

### Database Management

- Multiple embedded SQLite databases in `assets/routeDB/`
- Database selection based on tax rate and data version
- Efficient route calculation with optimized queries
- Support for historical fare data

## Key Features

### Multi-Database Support

The app includes railway databases from different years to account for:
- Tax rate changes (5% → 8% → 10%)
- Route modifications and new line openings
- Fare structure updates

### Advanced Route Calculation

- **Shortest Path**: Dijkstra's algorithm for optimal routing
- **Special Rules**: Handles complex Japanese railway regulations
- **Transfer Optimization**: Minimizes transfers while considering fare impact
- **Multiple Route Options**: Provides alternative routes with different trade-offs

### Modern Android Development

- **Jetpack Compose**: Fully declarative UI
- **Material 3**: Latest Material Design system
- **State Management**: Unidirectional data flow
- **Testing**: Comprehensive unit and integration tests

## Contributing

This project follows modern Android development best practices:

1. Use Jetpack Compose for all new UI
2. Follow Material 3 design guidelines
3. Implement proper state management with State Holders
4. Write tests for new functionality
5. Follow Kotlin coding conventions

## Version History

- **v25.08**: Current version with Compose migration
- **Previous versions**: Legacy XML-based UI (deprecated)

## License

- GPL(v3)

## Credits

Developed by Sutezo for Japanese railway fare calculation and route planning.
