
AAB = app/build/outputs/bundle/release/app-release.aab
APK = app/build/outputs/apk/release/app-release.apk 

.PHONY: fin

all: $(AAB) $(APK) fin

$(APK) : 
	./gradlew assembleRelease

$(AAB) :
	./gradlew bundleRelease

fin :
	ls -l $(APK) $(AAB)
