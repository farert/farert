plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
    alias(libs.plugins.kotlin.compose)
}

android {
    signingConfigs {
        create("release") {
            storeFile = file("farert-upload-keystore.jks")
            storePassword = project.property("RELEASE_STORE_PASSWORD") as String
            keyPassword = project.property("RELEASE_KEY_PASSWORD") as String
            keyAlias = project.property("RELEASE_KEY_ALIAS") as String
        }
    }
    namespace = "org.sutezo.farert"
    compileSdk = 36

    defaultConfig {
        applicationId = "org.sutezo.farert"
        minSdk = 24
        targetSdk = 36
        versionCode = 45
        versionName = "26.04.01"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        debug {
            isDebuggable = true
        }
        release {
            isMinifyEnabled = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            signingConfig = signingConfigs.getByName("release")
            isShrinkResources = true

        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    kotlinOptions {
        jvmTarget = "11"
    }
    buildFeatures {
        compose = true
        buildConfig = true
    }

    lint {
        baseline = file("lint-baseline.xml")
    }
    packaging {
        resources {
            excludes += "/META-INF/{AL2.0,LGPL2.1}"
        }
        jniLibs {
            useLegacyPackaging = false
        }
    }
    buildToolsVersion = "35.0.0"
    androidComponents {
        onVariants { variant ->
            variant.packaging.dex.useLegacyPackaging = false
        }
    }
    // Disable all baseline profile related tasks
    afterEvaluate {
        tasks.matching { it.name.contains("BaselineProfile", ignoreCase = true) }.configureEach {
            enabled = false
        }
        tasks.matching { it.name.contains("ArtProfile", ignoreCase = true) }.configureEach {
            enabled = false
        }
        tasks.matching { it.name.contains("StartupProfile", ignoreCase = true) }.configureEach {
            enabled = false
        }
    }

}

tasks.withType<JavaCompile>().configureEach {
    if (name.contains("UnitTest", ignoreCase = true)) {
        // Exclude local CLI test helpers from Android Studio unit test compilation.
        exclude(
            "**/android/**",
            "**/org/sutezo/farert/**",
            "**/org/sutezo/alps/JavaTestMain.java"
        )
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.lifecycle.runtime.ktx)
    implementation(libs.androidx.activity.compose)
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.ui)
    implementation(libs.androidx.ui.graphics)
    implementation(libs.androidx.ui.tooling.preview)
    implementation(libs.androidx.material3)
    
    // Additional Compose dependencies
    implementation(libs.androidx.material.icons.extended)
    implementation(libs.androidx.lifecycle.viewmodel.compose)
    implementation(libs.androidx.navigation.compose)
    implementation(libs.androidx.runtime.livedata)
    
    // Modern AndroidX libraries
    implementation(libs.androidx.appcompat)
    implementation(libs.androidx.preference.ktx)
    implementation(libs.androidx.lifecycle.viewmodel.ktx)
    implementation(libs.androidx.lifecycle.runtime.compose)
    
    // Material Design
    implementation(libs.material)
    
    // Testing
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.ui.test.junit4)
    debugImplementation(libs.androidx.ui.tooling)
    debugImplementation(libs.androidx.ui.test.manifest)
}
