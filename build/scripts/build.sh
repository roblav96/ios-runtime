#!/usr/bin/env bash

set -e

WORKSPACE=`pwd`

function xcodebuild_pretty {
    XCFORMATTER=true
    if hash xcpretty 2>/dev/null; then
        XCFORMATTER=xcpretty
    fi
    set -o pipefail && xcodebuild "$@" 2>&1 | tee -a "$WORKSPACE/build.log" | $XCFORMATTER
}

CMAKE_FLAGS="-G Xcode -DCMAKE_INSTALL_PREFIX=$WORKSPACE/dist"

mkdir -p "$WORKSPACE/cmake-build"
cd "$WORKSPACE/cmake-build"

echo "Building NativeScript.framework..."
rm -f CMakeCache.txt
rm -f "$WORKSPACE/build.log"
echo -e "\tConfiguring..."
cmake .. $CMAKE_FLAGS -DBUILD_SHARED_LIBS=ON 2>&1 | tee -a "$WORKSPACE/build.log"
echo -e "\tiPhoneOS..."
xcodebuild_pretty -configuration Release -sdk iphoneos -target NativeScript
echo -e "\tiPhoneSimulator..."
xcodebuild_pretty -configuration Release -sdk iphonesimulator -target NativeScript

echo "Packaging NativeScript.framework..."
mkdir -p "$WORKSPACE/dist"
cp -r "$WORKSPACE/cmake-build/src/NativeScript/Release-iphoneos/NativeScript.framework" "$WORKSPACE/dist"
rm "$WORKSPACE/dist/NativeScript.framework/NativeScript"
lipo -create -output "$WORKSPACE/dist/NativeScript.framework/NativeScript" \
    "$WORKSPACE/cmake-build/src/NativeScript/Release-iphonesimulator/NativeScript.framework/NativeScript" \
    "$WORKSPACE/cmake-build/src/NativeScript/Release-iphoneos/NativeScript.framework/NativeScript" \
         >> "$WORKSPACE/build.log" 2>&1

echo "Building objc-metadata-generator..."
xcodebuild_pretty -configuration Release -target MetadataGenerator
echo "Packaging objc-metadata-generator..."
cp -R "$WORKSPACE/cmake-build/metadataGenerator" "$WORKSPACE/dist/"
cp "$WORKSPACE/build/scripts/metadata-generation-build-step" "$WORKSPACE/dist/metadataGenerator/bin/"

echo "Building TestRunner..."
rm -f CMakeCache.txt
cmake .. $CMAKE_FLAGS 2>&1 | tee -a "$WORKSPACE/build.log"
xcodebuild_pretty -configuration Debug -sdk iphoneos -target TestRunner ARCHS="armv7 arm64" ONLY_ACTIVE_ARCH=NO
echo "Packaging TestRunner..."
xcrun -sdk iphoneos PackageApplication -v "$WORKSPACE/cmake-build/tests/TestRunner/Debug-iphoneos/TestRunner.app" \
    -o "$WORKSPACE/cmake-build/tests/TestRunner/Debug-iphoneos/TestRunner.ipa" \
         >> "$WORKSPACE/build.log" 2>&1
