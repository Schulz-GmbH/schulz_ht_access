import com.android.build.gradle.internal.api.BaseVariantOutputImpl

val RELEASE_STORE_FILE: String      by project
val RELEASE_STORE_PASSWORD: String  by project
val RELEASE_KEY_ALIAS: String       by project
val RELEASE_KEY_PASSWORD: String    by project

plugins {
    alias(libs.plugins.android.application)
}

android {
    namespace = "de.schulz.hygiene.hs_access"
    compileSdk = 34

    defaultConfig {
        applicationId = "de.schulz.hygiene.hs_access"
        minSdk = 31
        targetSdk = 34
        versionCode = 1
        versionName = "1.0.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    // --------------------------------------------------
    // Signing-Configs für Release
    // --------------------------------------------------
    signingConfigs {
        create("release") {
            storeFile = file(RELEASE_STORE_FILE)
            storePassword = RELEASE_STORE_PASSWORD
            keyAlias      = RELEASE_KEY_ALIAS
            keyPassword   = RELEASE_KEY_PASSWORD
            enableV1Signing = true
            enableV2Signing = true
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            signingConfig = signingConfigs.getByName("release")
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }

    applicationVariants.all {
        val variant = this
        if (variant.buildType.name == "release") {
            variant.outputs
                .map { it as BaseVariantOutputImpl }
                .forEach { output ->
                    output.outputFileName = "schulz-access-${variant.versionName}.apk"
                }
        }
    }
}

dependencies {
    implementation(libs.appcompat)
    implementation(libs.material)

    testImplementation(libs.junit)
    androidTestImplementation(libs.ext.junit)
    androidTestImplementation(libs.espresso.core)
}
