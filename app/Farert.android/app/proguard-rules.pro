# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:60
-keep class org.sutezo.farert.ArchiveRouteActivity { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:11
-keep class org.sutezo.farert.FarertApp { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:35
-keep class org.sutezo.farert.LineListActivity { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:20
-keep class org.sutezo.farert.MainActivity { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:57
-keep class org.sutezo.farert.ResultViewActivity { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:54
-keep class org.sutezo.farert.SettingsActivity { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:40
-keep class org.sutezo.farert.StationListActivity { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:31
-keep class org.sutezo.farert.TerminalSelectActivity { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/build/intermediates/merged_manifests/productionRelease/AndroidManifest.xml:50
-keep class org.sutezo.farert.VersionActivity { <init>(); }
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/fragment_terminal_select.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/fragment_drawer.xml:8
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/folder_list.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/content_version.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/content_main.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_settings.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_result_view.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_archive_route.xml:2
-keep class android.support.constraint.ConstraintLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/content_main.xml:129
-keep class android.support.constraint.Group { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/design-27.1.1.aar/737a0be00e4ce65fe2ce8f558fc2e255/res/layout/design_bottom_navigation_item.xml:26
-keep class android.support.design.internal.BaselineLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/design-27.1.1.aar/737a0be00e4ce65fe2ce8f558fc2e255/res/layout/design_navigation_item.xml:17
-keep class android.support.design.internal.NavigationMenuItemView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/design-27.1.1.aar/737a0be00e4ce65fe2ce8f558fc2e255/res/layout/design_navigation_menu.xml:17
-keep class android.support.design.internal.NavigationMenuView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/design-27.1.1.aar/737a0be00e4ce65fe2ce8f558fc2e255/res/layout/design_layout_snackbar_include.xml:18
-keep class android.support.design.internal.SnackbarContentLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/app_bar_main.xml:9
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_version.xml:9
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_terminal_select.xml:11
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_station_list.xml:10
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_line_list.xml:10
-keep class android.support.design.widget.AppBarLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/content_main.xml:115
-keep class android.support.design.widget.BottomNavigationView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/design-27.1.1.aar/737a0be00e4ce65fe2ce8f558fc2e255/res/layout/design_text_input_password_icon.xml:18
-keep class android.support.design.widget.CheckableImageButton { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/design-27.1.1.aar/737a0be00e4ce65fe2ce8f558fc2e255/res/layout/design_bottom_sheet_dialog.xml:26
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/app_bar_main.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_version.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_terminal_select.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_station_list.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_line_list.xml:2
-keep class android.support.design.widget.CoordinatorLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/design-27.1.1.aar/737a0be00e4ce65fe2ce8f558fc2e255/res/layout/design_layout_snackbar.xml:18
-keep class android.support.design.widget.Snackbar$SnackbarLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_terminal_select.xml:35
-keep class android.support.design.widget.TabItem { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_terminal_select.xml:30
-keep class android.support.design.widget.TabLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_terminal_select.xml:65
-keep class android.support.v4.view.ViewPager { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_main.xml:2
-keep class android.support.v4.widget.DrawerLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_alert_dialog_material.xml:41
-keep class android.support.v4.widget.NestedScrollView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_select_dialog_material.xml:23
-keep class android.support.v7.app.AlertController$RecycleListView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_action_menu_item_layout.xml:17
-keep class android.support.v7.view.menu.ActionMenuItemView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_expanded_menu_layout.xml:17
-keep class android.support.v7.view.menu.ExpandedMenuView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_popup_menu_item_layout.xml:17
# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_list_menu_item_layout.xml:17
-keep class android.support.v7.view.menu.ListMenuItemView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_toolbar.xml:27
-keep class android.support.v7.widget.ActionBarContainer { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_toolbar.xml:43
# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_action_mode_bar.xml:19
-keep class android.support.v7.widget.ActionBarContextView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_toolbar.xml:17
-keep class android.support.v7.widget.ActionBarOverlayLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_action_menu_layout.xml:17
-keep class android.support.v7.widget.ActionMenuView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_activity_chooser_view.xml:19
-keep class android.support.v7.widget.ActivityChooserView$InnerLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_alert_dialog_material.xml:18
-keep class android.support.v7.widget.AlertDialogLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/folder_list.xml:28
-keep class android.support.v7.widget.AppCompatSpinner { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_alert_dialog_button_bar_material.xml:26
-keep class android.support.v7.widget.ButtonBarLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_content_include.xml:19
-keep class android.support.v7.widget.ContentFrameLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_alert_dialog_title_material.xml:45
-keep class android.support.v7.widget.DialogTitle { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_simple_overlay_action_mode.xml:23
-keep class android.support.v7.widget.FitWindowsFrameLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_simple.xml:17
# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_dialog_title_material.xml:22
-keep class android.support.v7.widget.FitWindowsLinearLayout { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/station_list.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/line_list.xml:2
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/fragment_terminal_select.xml:15
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/fragment_drawer.xml:128
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/content_main.xml:24
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_archive_route.xml:10
-keep class android.support.v7.widget.RecyclerView { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_search_view.xml:75
###  -keep class android.support.v7.widget.SearchView$SearchAutoComplete { <init>(android.content.Context, android.util.AttributeSet); }
-keep class android.support.v7.widget.SearchView { *; }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout-v26/abc_screen_toolbar.xml:37
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/app_bar_main.xml:14
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_version.xml:14
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_terminal_select.xml:18
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_station_list.xml:23
# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_line_list.xml:16
# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_toolbar.xml:36
-keep class android.support.v7.widget.Toolbar { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_simple_overlay_action_mode.xml:32
# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_screen_simple.xml:25
-keep class android.support.v7.widget.ViewStubCompat { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_alert_dialog_title_material.xml:56
# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_alert_dialog_material.xml:52
# Referenced at /Users/ntake/.gradle/caches/transforms-1/files-1.1/appcompat-v7-27.1.1.aar/6e2df8631c8c3e4a7a2f6e5f63d44e53/res/layout/abc_alert_dialog_button_bar_material.xml:43
-keep class android.widget.Space { <init>(android.content.Context, android.util.AttributeSet); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/layout/activity_main.xml:18
-keep class org.sutezo.farert.FolderViewFragment { <init>(); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/xml/pref_headers.xml:15
-keep class org.sutezo.farert.SettingsActivity$DataSyncPreferenceFragment { <init>(); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/xml/pref_headers.xml:5
-keep class org.sutezo.farert.SettingsActivity$GeneralPreferenceFragment { <init>(); }

# Referenced at /Users/ntake/exports/devel/farert.repos/app/Farert.android/app/src/main/res/xml/pref_headers.xml:10
-keep class org.sutezo.farert.SettingsActivity$NotificationPreferenceFragment { <init>(); }

