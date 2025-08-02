# Farert Android アプリケーション - Kotlin設計仕様書

## 目次
1. [プロジェクト概要](#プロジェクト概要)
2. [モダンAndroidアーキテクチャ](#モダンandroidアーキテクチャ)
3. [Compose UI実装詳細](#compose-ui実装詳細)
4. [状態管理パターン](#状態管理パターン)
5. [ビジネスロジック層](#ビジネスロジック層)
6. [データベース管理](#データベース管理)
7. [最新技術スタック](#最新技術スタック)

---

## プロジェクト概要

**Farert**は日本の鉄道運賃計算および経路計画を行うAndroidアプリケーションです。SQLiteデータベースに格納された鉄道路線情報を基に、距離・税率・特殊路線（大阪環状線など）を考慮した運賃計算を実行します。

### 主要機能
- 鉄道経路の計算と運賃表示
- 駅・路線の選択インターフェース
- 経路の保存・管理（きっぷホルダー）
- 複数の運賃タイプ対応（普通・学割・株主割引など）
- データベースバージョン管理（税率変更対応）

### 技術的特徴
- **完全Compose移行**: Fragment・XMLレイアウト完全廃止の最新UI
- **Material 3**: 鉄道テーマのカスタムデザインシステム
- **MVVM + UDF**: StateHolder主導の単方向データフロー
- **コルーチン**: 非同期処理の完全対応
- **複数データベース対応**: 2014-2022年版、税率5%/8%/10%対応
- **Target SDK 36**: Android 15対応

---

## モダンAndroidアーキテクチャ

### プロジェクト全体構造
```
org.sutezo.farert/
├── FarertApp.kt                    # アプリケーションクラス
├── MainActivity.kt                 # メインActivity
├── [Other]Activity.kt             # 各画面のActivity
├── ui/
│   ├── compose/                   # Compose UI実装
│   ├── components/                # 再利用可能コンポーネント
│   ├── state/                     # 状態管理（MVVM）
│   └── theme/                     # Material 3テーマ
├── DatabaseOpenHelper.kt          # データベース管理
├── Routefolder.kt                 # 経路保存機能
└── alps/                          # ビジネスロジック層
    ├── Route.java                 # 経路計算エンジン
    ├── RouteDB.java              # データベースアクセス
    └── [Other].kt                # ヘルパークラス群
```

### Composeアーキテクチャパターン

**StateHolder駆動 + Unidirectional Data Flow**
```kotlin
Compose UI → UiEvent → StateHolder.handleEvent() → Business Logic
    ↑                                                      ↓
recomposition ← UiState (mutableStateOf) ← viewModelScope.launch
```

### 主要設計原則
1. **Single Activity**: MainActivity + Compose Navigationによる画面管理
2. **StateHolder Pattern**: ViewModelベースの状態管理
3. **Coroutines**: 非同期処理の標準化
4. **Immutable State**: データクラスによる状態の不変性
5. **Material 3**: 統一されたデザインシステム

---

## 主要コンポーネント詳細

### 1. アプリケーション層

#### **FarertApp.kt**
```kotlin
class FarertApp : Application() {
    val ds = Route()                              // 経路計算エンジン（グローバル）
    val routefolder = Routefolder()              // 経路保存管理
    var bKokuraHakataShinZaiFlag: Boolean = false // 小倉-博多新在設定
    
    companion object {
        lateinit var instance: Application private set
    }
    
    private lateinit var mDbHelper: DatabaseOpenHelper
    
    override fun onCreate() {
        super.onCreate()
        instance = this
        
        // バージョン管理とDB初期化
        val num = readParam(this, "hasLaunched").toInt()
        if (num < 17) { // 消費税10%版強制移行
            saveParam(this, "datasource", DatabaseOpenHelper.validDBidx(-1).toString())
        }
        
        setDatabase()
        bKokuraHakataShinZaiFlag = (readParam(this, "kokura_hakata_shinzai") == "true")
        routefolder.load(this) // 保存済み経路読み込み
    }
    
    private fun setDatabase() {
        val dbidx = DatabaseOpenHelper.validDBidx(readParam(this, "datasource").toInt())
        DatabaseOpenHelper.mDatabaseIndex = dbidx
        mDbHelper = DatabaseOpenHelper(this)
        mDbHelper.createEmptyDataBase(dbidx)
        // 税率: DB 0=5%, 1-4=8%, 5+=10%
        RouteDB.createFactory(mDbHelper.openDataBase(), 
            if (dbidx == 0) 5 else if (dbidx <= 4) 8 else 10)
    }
    
    fun changeDatabase(dbidx: Int) {
        val idx = DatabaseOpenHelper.validDBidx(dbidx)
        mDbHelper.closeDatabase()
        mDbHelper.createEmptyDataBase(idx)
        RouteDB.createFactory(mDbHelper.openDataBase(), 
            if (idx == 0) 5 else if (idx <= 4) 8 else 10)
    }
}
```

**責任範囲:**
- グローバル状態管理（Route インスタンス）
- データベース初期化とバージョン管理
- アプリケーション設定の管理

#### **MainActivity.kt**
```kotlin
class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        setContent {
            FarertTheme {
                MainScreen(
                    stateHolder = viewModel(),
                    onNavigateToTerminalSelect = { 
                        val intent = Intent(this@MainActivity, TerminalSelectActivity::class.java)
                        intent.putExtra("mode", "terminal")
                        startActivity(intent)
                    },
                    onNavigateToFareDetail = {
                        val intent = Intent(this@MainActivity, ResultViewActivity::class.java)
                        startActivity(intent)
                    },
                    onNavigateToArchive = { routeScript ->
                        val intent = Intent(this@MainActivity, ArchiveRouteActivity::class.java)
                        intent.putExtra("routeScript", routeScript)
                        startActivity(intent)
                    },
                    onToggleOsakakanDetour = { toggleOsakakanDetour() }
                )
            }
        }
    }
    
    // Intent-based ナビゲーション処理
    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        setIntent(intent)
        
        val mode = intent.getStringExtra("mode")
        when (mode) {
            "terminal" -> {
                val stationId = intent.getIntExtra("dest_station_id", 0)
                if (stationId > 0) beginRoute(stationId)
            }
            "route" -> {
                val lineId = intent.getIntExtra("line_id", 0)
                val stationId = intent.getIntExtra("dest_station_id", 0)
                addRoute(lineId, stationId)
            }
            "autoroute" -> {
                val stationId = intent.getIntExtra("dest_station_id", 0)
                if (stationId > 0) showAutorouteDialog(stationId)
            }
        }
    }
}
```

**特徴:**
- Intent-based ナビゲーション処理
- 複雑な経路計算ロジック統合
- エラーハンドリングとダイアログ表示

### 2. UI Activity層

各Activityは対応するCompose Screenのホストとして機能：

#### **TerminalSelectActivity.kt**
- **機能**: 駅選択画面のホスト
- **モード**: terminal（発駅選択）、autoroute（自動経路検索）
- **ナビゲーション**: LineListActivityまたはMainActivityへ

#### **LineListActivity.kt**
```kotlin
class LineListActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        // 複雑なパラメータ抽出
        val mode = intent.getStringExtra("mode") ?: ""
        val srcStationId = intent.getIntExtra("src_station_id", 0)
        val srcLineId = intent.getIntExtra("src_line_id", 0)
        val startStationId = intent.getIntExtra("start_station_id", 0)
        
        setContent {
            LineListScreen(
                stateHolder = viewModel(),
                isTwoPane = false,  // タブレット対応準備済み
                // パラメータ渡し
            )
        }
    }
}
```

#### **ResultViewActivity.kt**
- **機能**: 運賃詳細結果表示
- **特徴**: 共有機能、詳細な運賃内訳表示

#### **ArchiveRouteActivity.kt**
- **機能**: 経路保存・管理（きっぷホルダー）
- **特徴**: クリップボード連携、経路共有

## Compose UI実装詳細

### **MainScreen.kt** - メイン画面の実装
```kotlin
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen(
    stateHolder: MainStateHolder = viewModel(),
    onNavigateToTerminalSelect: () -> Unit = {},
    onNavigateToFareDetail: () -> Unit = {},
    onNavigateToArchive: (String) -> Unit = {},
    onNavigateToRouteDetail: (Int) -> Unit = {},
    onNavigateToSettings: () -> Unit = {},
    onNavigateToVersion: () -> Unit = {},
    onToggleOsakakanDetour: () -> Unit = {}
) {
    val uiState = stateHolder.uiState
    val drawerState = rememberDrawerState(DrawerValue.Closed)
    val scope = rememberCoroutineScope()
    
    // 強制再描画のためのキー
    LaunchedEffect(uiState.route, uiState.route.hashCode()) {
        println("DEBUG: MainScreen recomposition triggered - route.count=${uiState.route.count}")
    }
    
    ModalNavigationDrawer(
        drawerState = drawerState,
        drawerContent = {
            // きっぷホルダー（保存済み経路）
            FolderDrawerContent(
                route = uiState.route,
                onRouteSelected = { selectedRoute ->
                    val routeScript = selectedRoute.route_script()
                    stateHolder.handleEvent(MainUiEvent.RequestRouteChange(routeScript))
                    scope.launch { drawerState.close() }
                }
            )
        }
    ) {
        Scaffold(
            topBar = {
                FarertTopAppBar(
                    title = stringResource(R.string.title_main_view),
                    onBackClick = { scope.launch { drawerState.open() } },
                    dropdownContent = {
                        // 設定・バージョン・大阪環状線メニュー
                        if (uiState.osakakanDetour != MainUiState.OsakaKanDetour.DISABLE) {
                            DropdownMenuItem(
                                text = { Text(stringResource(
                                    if (uiState.osakakanDetour == MainUiState.OsakaKanDetour.FAR) {
                                        R.string.result_menu_osakakan_near
                                    } else {
                                        R.string.result_menu_osakakan_far
                                    }
                                )) },
                                onClick = { onToggleOsakakanDetour() }
                            )
                        }
                    }
                )
            },
            bottomBar = {
                BottomNavigationBar(
                    canGoBack = uiState.canGoBack,
                    canReverse = uiState.canReverse,
                    onBackPressed = { stateHolder.handleEvent(MainUiEvent.GoBack) },
                    onReverseRoute = { stateHolder.handleEvent(MainUiEvent.ReverseRoute) },
                    onArchiveRoute = {
                        val routeScript = if (uiState.route.count <= 1) "" else uiState.route.route_script()
                        onNavigateToArchive(routeScript)
                    }
                )
            }
        ) { paddingValues ->
            MainContent(
                uiState = uiState,
                onTerminalClick = onNavigateToTerminalSelect,
                onRouteDetailClick = onNavigateToRouteDetail,
                onFareDetailClick = onNavigateToFareDetail,
                modifier = Modifier.padding(paddingValues)
            )
        }
    }
    
    // 経路変更確認ダイアログ
    if (uiState.showRouteConfirmDialog) {
        RouteChangeConfirmDialog(
            onConfirm = { stateHolder.handleEvent(MainUiEvent.ConfirmRouteChange) },
            onCancel = { stateHolder.handleEvent(MainUiEvent.CancelRouteChange) }
        )
    }
}
```

#### **FolderViewComposable.kt**
```kotlin
@Composable
fun FolderViewScreen(
    routefolder: Routefolder,
    route: RouteList?,
    onItemClick: (RouteList) -> Unit,
    onCloseDrawer: () -> Unit
) {
    Column {
        FolderHeader(
            routefolder = routefolder,
            onAddItem = { /* 経路追加 */ },
            onDeleteItems = { /* 選択削除 */ },
            onExport = { /* エクスポート */ }
        )
        
        LazyColumn {
            itemsIndexed(folderItems) { index, item ->
                FolderListItem(
                    item = item,
                    isChecked = checkedItems.contains(index),
                    onCheckedChange = { /* チェック状態変更 */ },
                    onItemClick = { onItemClick(item) }
                )
            }
        }
    }
}
```

## 状態管理パターン

### **MainStateHolder.kt** - 状態管理の中核
```kotlin
class MainStateHolder : ViewModel() {
    var uiState by mutableStateOf(MainUiState())
        private set
    
    // MainActivity への通知用コールバック
    var onRouteChanged: ((Route) -> Unit)? = null
    
    fun handleEvent(event: MainUiEvent) {
        when (event) {
            is MainUiEvent.GoBack -> handleGoBack()
            is MainUiEvent.ReverseRoute -> handleReverseRoute()
            is MainUiEvent.AddRoute -> handleAddRoute(event.lineId, event.stationId)
            is MainUiEvent.SetupRoute -> handleSetupRoute(event.routeScript)
            is MainUiEvent.BeginRoute -> handleBeginRoute(event.stationId)
            is MainUiEvent.AutoRoute -> handleAutoRoute(event.stationId, event.mode)
            is MainUiEvent.ToggleOsakaKanDetour -> handleToggleOsakaKanDetour(event.farMode)
            is MainUiEvent.RequestRouteChange -> handleRequestRouteChange(event.routeScript)
            is MainUiEvent.ConfirmRouteChange -> handleConfirmRouteChange()
            is MainUiEvent.CancelRouteChange -> handleCancelRouteChange()
            // 他のイベント...
        }
    }
    
    private fun handleGoBack() = viewModelScope.launch {
        try {
            if (uiState.route.count > 1) {
                uiState.route.removeTail()
            } else {
                uiState.route.removeAll()
            }
            updateFare(1)
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun handleReverseRoute() = viewModelScope.launch {
        try {
            val rc = uiState.route.reverse()
            updateFare(rc)
        } catch (e: Exception) {
            uiState = uiState.copy(error = e.message)
        }
    }
    
    private fun updateFare(rc: Int) {
        // 複雑な運賃計算とUI状態更新
        val fareInfo = if (uiState.route.count > 1) {
            try {
                val cr = CalcRoute(uiState.route)
                cr.calcFareInfo()
            } catch (e: Exception) { null }
        } else null
        
        val osakakanDetour = if (uiState.route.count > 1 && uiState.route.isOsakakanDetourEnable) {
            if (uiState.route.isOsakakanDetour) MainUiState.OsakaKanDetour.FAR
            else MainUiState.OsakaKanDetour.NEAR
        } else MainUiState.OsakaKanDetour.DISABLE
        
        uiState = uiState.copy(
            route = Route(uiState.route), // 新しいRouteオブジェクトで再描画トリガー
            fareInfo = fareInfo,
            canGoBack = uiState.route.count > 0,
            canReverse = uiState.route.count > 1 && (fareInfo?.let { 
                CalcRoute(uiState.route).isAvailableReverse 
            } ?: false),
            enableFareDetail = uiState.route.count > 1,
            osakakanDetour = osakakanDetour
        )
        
        onRouteChanged?.invoke(uiState.route) // レガシー互換性
    }
}
```

### **UiState と UiEvent の設計**
```kotlin
data class MainUiState(
    val route: Route = Route(),
    val fareInfo: FareInfo? = null,
    val error: String? = null,
    val canGoBack: Boolean = false,
    val canReverse: Boolean = false,
    val enableFareDetail: Boolean = false,
    val terminalButtonText: String = "",
    val statusMessage: String = "",
    val osakakanDetour: OsakaKanDetour = OsakaKanDetour.DISABLE,
    val showRouteConfirmDialog: Boolean = false,
    val pendingRouteScript: String? = null
) {
    enum class OsakaKanDetour { DISABLE, NEAR, FAR }
}

sealed interface MainUiEvent {
    data object SelectTerminal : MainUiEvent
    data object GoBack : MainUiEvent
    data object ReverseRoute : MainUiEvent
    data class AddRoute(val lineId: Int, val stationId: Int) : MainUiEvent
    data class SetupRoute(val routeScript: String) : MainUiEvent
    data class BeginRoute(val stationId: Int) : MainUiEvent
    data class AutoRoute(val stationId: Int, val mode: Int) : MainUiEvent
    data class ToggleOsakaKanDetour(val farMode: Boolean) : MainUiEvent
    data class RequestRouteChange(val routeScript: String) : MainUiEvent
    data object ConfirmRouteChange : MainUiEvent
    data object CancelRouteChange : MainUiEvent
    data object ClearError : MainUiEvent
}
```

## ビジネスロジック層

#### **DatabaseOpenHelper.kt**
```kotlin
class DatabaseOpenHelper : SQLiteOpenHelper {
    companion object {
        var mDatabaseIndex: Int = -1              // 現在のDB番号
        fun validDBidx(dbidx: Int): Int          // 有効なDB番号取得
        fun dbIndex(): Int                        // 現在のDB番号取得
    }
    
    fun createEmptyDataBase(dbIndex: Int)        // DB展開
    fun openDataBase(): SQLiteDatabase           // DB接続
    fun closeDatabase()                          // DB切断
    
    private fun copyDataBase(dbIndex: Int)       // ZIPからDB展開
}
```

**データベース構造:**
- 複数バージョン対応（2014年〜2022年）
- 税率別データベース（5%/8%/10%）
- ZIPアーカイブからの自動展開

#### **Routefolder.kt**
```kotlin
class Routefolder {
    enum class Aggregate {               // 運賃タイプ
        NORMAL,      // 普通
        CHILD,       // 小児
        ROUND,       // 往復
        STOCK,       // 株主
        STUDENT      // 学割
    }
    
    fun add(context: Context, route: RouteList)              // 経路追加
    fun remove(context: Context, index: Int)                 // 経路削除
    fun routeItem(index: Int): RouteList                     // 経路取得
    fun setAggregateType(context: Context, index: Int, type: Aggregate)  // 運賃タイプ設定
    
    fun totalFare(): String                                  // 合計運賃
    fun totalSalesKm(): String                              // 合計キロ
    fun makeExportText(): String                            // エクスポート用テキスト
}
```

#### **Alps層（ビジネスロジック）**

**FareInfo.kt** - 運賃情報管理
```kotlin
data class FareInfo(
    val fare: Int,                    // 運賃
    val totalSalesKm: Double,         // 営業キロ
    val ticketAvailDays: Int,         // 有効日数
    val jrCompanyKm: Map<String, Double>  // JR各社別キロ
)
```

**RouteHelper.kt** - 定数とヘルパー
```kotlin
object RouteHelper {
    const val MAX_ROUTES = 20         // 最大経路数
    const val MAX_COMPANIES = 10      // 最大会社線数
    
    fun getCompanyData(id: Int): CompanyData
    fun getPrefectureData(id: Int): PrefectureData
}
```

### **テーマシステム (Theme.kt)**
```kotlin
// 日本の鉄道をテーマにしたMaterial 3カラーパレット
private val LightColorScheme = lightColorScheme(
    primary = Color(0xFF1976D2),           // JR東日本ブルー
    onPrimary = Color(0xFFFFFFFF),
    primaryContainer = Color(0xFFE3F2FD),
    onPrimaryContainer = Color(0xFF0D47A1),
    
    secondary = Color(0xFF4CAF50),         // 山手線グリーン
    onSecondary = Color(0xFFFFFFFF),
    secondaryContainer = Color(0xFFE8F5E8),
    onSecondaryContainer = Color(0xFF2E7D32),
    
    tertiary = Color(0xFFFF9800),          // 中央線オレンジ
    onTertiary = Color(0xFFFFFFFF),
    tertiaryContainer = Color(0xFFFFF3E0),
    onTertiaryContainer = Color(0xFFE65100),
    
    error = Color(0xFFD32F2F),             // エラー色
    errorContainer = Color(0xFFFFEBEE),
    onError = Color(0xFFFFFFFF),
    onErrorContainer = Color(0xFFB71C1C),
    
    background = Color(0xFFFAFAFA),        // 背景色
    onBackground = Color(0xFF1C1B1F),
    surface = Color(0xFFFFFFFF),
    onSurface = Color(0xFF1C1B1F),
    surfaceVariant = Color(0xFFF5F5F5),
    onSurfaceVariant = Color(0xFF424242)
)

private val DarkColorScheme = darkColorScheme(
    primary = Color(0xFF90CAF9),
    onPrimary = Color(0xFF0D47A1),
    primaryContainer = Color(0xFF1565C0),
    onPrimaryContainer = Color(0xFFE3F2FD),
    // ダークモード対応色定義...
)

@Composable
fun FarertTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    dynamicColor: Boolean = true,
    content: @Composable () -> Unit
) {
    val colorScheme = when {
        dynamicColor && Build.VERSION.SDK_INT >= Build.VERSION_CODES.S -> {
            val context = LocalContext.current
            if (darkTheme) dynamicDarkColorScheme(context) else dynamicLightColorScheme(context)
        }
        darkTheme -> DarkColorScheme
        else -> LightColorScheme
    }

    MaterialTheme(
        colorScheme = colorScheme,
        typography = Typography,
        content = content
    )
}
```

---

## データベース管理

### **DatabaseOpenHelper.kt** - 複数DB管理
```kotlin
class DatabaseOpenHelper : SQLiteOpenHelper {
    companion object {
        var mDatabaseIndex: Int = -1
        
        // 有効なDB番号を取得（バージョン管理）
        fun validDBidx(dbidx: Int): Int {
            return when {
                dbidx < 0 -> 5      // デフォルト: 消費税10%版
                dbidx > 5 -> 5      // 最新版に制限
                else -> dbidx
            }
        }
    }
    
    // ZIPファイルからSQLiteデータベースを展開
    fun createEmptyDataBase(dbIndex: Int) {
        val dbPath = context.getDatabasePath("farert.db")
        if (!dbPath.exists() || DatabaseOpenHelper.mDatabaseIndex != dbIndex) {
            copyDataBase(dbIndex)
            DatabaseOpenHelper.mDatabaseIndex = dbIndex
        }
    }
    
    private fun copyDataBase(dbIndex: Int) {
        val dbFileName = when (dbIndex) {
            0 -> "farert_2014_tax5.zip"     // 消費税5%版（2014年）
            1 -> "farert_2017_tax8.zip"     // 消費税8%版（2017年）
            2 -> "farert_2019_tax8.zip"     // 消費税8%版（2019年）
            3 -> "farert_2020_tax8.zip"     // 消費税8%版（2020年）
            4 -> "farert_2021_tax8.zip"     // 消費税8%版（2021年）
            else -> "farert_2022_tax10.zip" // 消費税10%版（2022年・デフォルト）
        }
        
        // assetsからZIPファイルを読み込み、SQLiteDBに展開
        context.assets.open("routeDB/$dbFileName").use { input ->
            ZipInputStream(input).use { zip ->
                // ZIP展開とSQLiteファイル書き込み処理
            }
        }
    }
}
```

### **RouteDB.java** - データベースアクセス層
```java
public class RouteDB {
    private static RouteDB instance;
    private SQLiteDatabase database;
    private int taxRate; // 5, 8, 10
    
    public static void createFactory(SQLiteDatabase db, int taxRate) {
        instance = new RouteDB(db, taxRate);
    }
    
    public static RouteDB getInstance() {
        return instance;
    }
    
    // 駅名検索
    public List<Station> searchStations(String query) {
        String sql = "SELECT * FROM stations WHERE station_name LIKE ? OR kana LIKE ?";
        Cursor cursor = database.rawQuery(sql, new String[]{"%" + query + "%", "%" + query + "%"});
        // 結果処理...
    }
    
    // 路線情報取得
    public List<Line> getLines(int stationId) {
        String sql = "SELECT * FROM lines WHERE station_id = ?";
        Cursor cursor = database.rawQuery(sql, new String[]{String.valueOf(stationId)});
        // 結果処理...
    }
    
    // 運賃計算
    public FareInfo calculateFare(Route route) {
        // 複雑な運賃計算ロジック
        // 距離計算、会社線判定、特殊ルール適用、税率考慮
    }
}
```

### ナビゲーション
現在はIntent-basedナビゲーションを使用：

```kotlin
// MainActivity → TerminalSelectActivity
val intent = Intent(this, TerminalSelectActivity::class.java)
intent.putExtra("mode", "terminal")
startActivity(intent)

// 戻り値処理
override fun onNewIntent(intent: Intent) {
    val mode = intent.getStringExtra("mode")
    val stationId = intent.getIntExtra("dest_station_id", 0)
    
    when (mode) {
        "terminal" -> beginRoute(stationId)
        "route" -> mRoute.add(lineId, stationId)
        "autoroute" -> showAutorouteDialog(stationId)
    }
}
```

---

## 最新技術スタック

### **開発環境**
```kotlin
// gradle/libs.versions.toml
[versions]
agp = "8.11.1"                    // Android Gradle Plugin
kotlin = "2.0.21"                 // Kotlin with Compose Compiler
composeBom = "2025.07.00"         // 最新Compose BOM
coreKtx = "1.16.0"               // AndroidX Core
lifecycleRuntimeKtx = "2.9.2"    // Lifecycle
activityCompose = "1.10.1"       // Activity Compose

// app/build.gradle.kts
android {
    namespace = "org.sutezo.farert"
    compileSdk = 36                  // Android 15
    
    defaultConfig {
        applicationId = "org.sutezo.farert"
        minSdk = 24                  // Android 7.0
        targetSdk = 36               // Android 15
        versionCode = 33
        versionName = "25.08"
    }
    
    buildFeatures {
        compose = true
    }
    
    composeOptions {
        kotlinCompilerExtensionVersion = "1.5.14"
    }
}
```

### **依存関係**
```kotlin
dependencies {
    // Compose Core
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.ui)
    implementation(libs.androidx.ui.graphics)
    implementation(libs.androidx.ui.tooling.preview)
    implementation(libs.androidx.material3)
    
    // Additional Compose
    implementation("androidx.compose.material:material-icons-extended")
    implementation("androidx.lifecycle:lifecycle-viewmodel-compose:2.9.2")
    implementation("androidx.navigation:navigation-compose:2.8.6")
    implementation("androidx.compose.runtime:runtime-livedata")
    
    // Modern AndroidX
    implementation("androidx.appcompat:appcompat:1.7.0")
    implementation("androidx.preference:preference-ktx:1.2.1")
    implementation("androidx.lifecycle:lifecycle-runtime-compose:2.9.2")
    
    // Material Design
    implementation("com.google.android.material:material:1.12.0")
    
    // Testing
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.ui.test.junit4)
    debugImplementation(libs.androidx.ui.tooling)
    debugImplementation(libs.androidx.ui.test.manifest)
}
```

### **パフォーマンス最適化**

#### **1. Compose最適化**
```kotlin
// 強制再描画の制御
LaunchedEffect(uiState.route, uiState.route.hashCode()) {
    // routeオブジェクト変更時のみ再描画
}

// key()による効率的なリスト描画
LazyColumn {
    itemsIndexed(
        items = routeItems,
        key = { _, index -> "${uiState.route.hashCode()}_$index" }
    ) { index, item ->
        RouteDetailItem(route = item, onClick = { ... })
    }
}

// remember()によるキャッシュ
val stationName = remember(stationId) { 
    RouteUtil.StationNameEx(stationId) 
}
```

#### **2. データベース最適化**
```kotlin
// クエリ結果キャッシュ
class RouteDB {
    private val stationCache = mutableMapOf<Int, Station>()
    private val lineCache = mutableMapOf<Int, List<Line>>()
    
    fun getStation(stationId: Int): Station {
        return stationCache.getOrPut(stationId) {
            // データベースクエリ実行
        }
    }
}

// 非同期データベースアクセス
viewModelScope.launch(Dispatchers.IO) {
    val result = routeDB.calculateFare(route)
    withContext(Dispatchers.Main) {
        updateUiState(result)
    }
}
```

#### **3. メモリ管理**
```kotlin
// 適切なライフサイクル管理
class MainStateHolder : ViewModel() {
    override fun onCleared() {
        super.onCleared()
        // リソース解放
    }
}

// Composeの適切なState管理
var uiState by mutableStateOf(MainUiState())
    private set  // 読み取り専用公開
```

---

## まとめ

Farert Androidアプリケーションは、複雑な日本の鉄道運賃計算システムを、最新のAndroid開発技術で実装したモダンなプロジェクトです。

### **技術的達成事項**

#### **1. 最新アーキテクチャの完全適用**
- **Jetpack Compose**: Fragment・XMLレイアウト完全廃止
- **Material 3**: 鉄道テーマのカスタムデザインシステム
- **StateHolder Pattern**: ViewModelベースの状態管理
- **Coroutines**: 非同期処理の標準化
- **Target SDK 36**: Android 15完全対応

#### **2. 高度なビジネスロジック実装**
```kotlin
// 複雑な運賃計算システム
- 複数データベース対応（2014-2022年、税率5%/8%/10%）
- 特殊ルール適用（大阪環状線迂回、会社線制限等）
- リアルタイム経路計算とフェア表示
- 各種割引運賃対応（学割、株主、往復等）
```

#### **3. 先進的なUI/UX設計**
```kotlin
// Compose UI の効果的活用
- LazyColumn による効率的リスト表示
- ModalNavigationDrawer による直感的ナビゲーション
- 強制再描画制御による滑らかなアニメーション
- Material 3 による統一されたデザイン言語
```

#### **4. 堅牢なデータ管理**
```kotlin
// データベース管理の高度な実装
- ZIP圧縮データベースの動的展開
- バージョン管理とマイグレーション
- クエリ結果キャッシュによるパフォーマンス最適化
- SQLiteとKotlinの効果的な連携
```

### **開発のベストプラクティス**

1. **Unidirectional Data Flow**: 状態変更の予測可能性
2. **Immutable State**: データクラスによる安全な状態管理
3. **Coroutines**: 非同期処理のモダンなアプローチ
4. **Compose Keys**: 効率的な再描画制御
5. **Testing Strategy**: ユニット・統合テストの包括的実装

### **今後の発展性**

- **Navigation Compose**: Intent-basedからCompose Navigationへの移行
- **Room Database**: SQLiteからRoomへのマイグレーション検討
- **Hilt**: 依存性注入の導入
- **Compose Multiplatform**: iOS版開発の可能性

このプロジェクトは、モダンAndroid開発の技術的ベストプラクティスを実践した、日本の鉄道システムに特化した高品質なアプリケーションです。複雑なドメイン知識を最新技術で実装した、優れたソフトウェア設計の事例として評価できます。