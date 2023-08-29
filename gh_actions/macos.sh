cd ..
brew install sdl2
git clone -b release/29.1 --recursive https://github.com/obsproject/obs-studio.git
mv OBSPlugin/src obs-studio/plugins/ptzoptics-obs
patch --ignore-whitespace obs-studio/plugins/CMakeLists.txt OBSPlugin/obs_plugins_cmakelists.patch
patch --ignore-whitespace obs-studio/plugins/ptzoptics-obs/PTZController/CMakeLists.txt OBSPlugin/macos_cmakelists.patch
cd obs-studio
CI/build-macos.sh
cd ./build_x86_64/plugins/ptzoptics-obs/PTZController/obs-ptzcontroller.plugin/Contents
mkdir ./Frameworks
cp -f /usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib ./Frameworks/libSDL2-2.0.0.dylib
cd ./MacOS
install_name_tool -change  /usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib @loader_path/../Frameworks/libSDL2-2.0.0.dylib ./obs-ptzcontroller
install_name_tool -change  UI/obs-frontend-api/obs-frontend-api.dylib @executable_path/../Frameworks/obs-frontend-api.dylib ./obs-ptzcontroller
install_name_tool -change  libobs/libobs.framework/Versions/A/libobs @executable_path/../Frameworks/libobs.framework/Versions/A/libobs ./obs-ptzcontroller

CODESIGN_CERTIFICATE_PATH=$RUNNER_TEMP/codesign_certificate.p12
SIGN_INSTALLER_CERTIFICATE_PATH=$RUNNER_TEMP/sign_installer_certificate.p12
KEYCHAIN_PATH=$RUNNER_TEMP/app-signing.keychain-db

# import certificate from secrets
echo -n "$CODESIGN_CERTIFICATE_BASE64" | base64 --decode -o $CODESIGN_CERTIFICATE_PATH
CODESIGN_IDENTITY=`openssl pkcs12 -in  $CODESIGN_CERTIFICATE_PATH  -nodes -passin env:CODESIGN_P12_PASSWORD | openssl x509 -noout -subject -nameopt multiline | grep commonName | awk '{ print $3 }'`
echo -n "$SIGN_INSTALLER_CERTIFICATE_BASE64" | base64 --decode -o $SIGN_INSTALLER_CERTIFICATE_PATH
SIGN_INSTALLER_IDENTITY=`openssl pkcs12 -in  $SIGN_INSTALLER_CERTIFICATE_PATH  -nodes -passin env:SIGN_INSTALLER_P12_PASSWORD  | openssl x509 -noout -subject -nameopt multiline | grep commonName | awk '{ print $3 }'`

# create temporary keychain
security create-keychain -p "$TEMP_KEYCHAIN_PASSWORD" $KEYCHAIN_PATH
security set-keychain-settings -lut 21600 $KEYCHAIN_PATH
security unlock-keychain -p "$TEMP_KEYCHAIN_PASSWORD" $KEYCHAIN_PATH

# import certificate to keychain
security import $CODESIGN_CERTIFICATE_PATH -P "$CODESIGN_P12_PASSWORD" -A -t cert -f pkcs12 -k $KEYCHAIN_PATH
security import $SIGN_INSTALLER_CERTIFICATE_PATH -P "$SIGN_INSTALLER_P12_PASSWORD" -A -t cert -f pkcs12 -k $KEYCHAIN_PATH

security list-keychains -d user -s $KEYCHAIN_PATH

cd ../../..
mv obs-ptzcontroller.plugin/Contents/MacOS/obs-ptzcontroller .

codesign --force --timestamp --options=runtime -s "$CODESIGN_IDENTITY"  -v ./obs-ptzcontroller
mv ./obs-ptzcontroller ./obs-ptzcontroller.plugin/Contents/MacOS/

cd obs-ptzcontroller.plugin
pkgbuild --root . \
         --install-location /tmp/obs-ptzcontroller.plugin \
         --scripts ../../../../../../OBSPlugin/installers-src/mac-v2/scripts \
         --identifier PtzOpticsOBSPlugin \
         --version 1.0.0 \
         --timestamp --sign "$SIGN_INSTALLER_IDENTITY" ../../../../../../OBSPlugin/PtzOpticsOBSPlugin_x86_64.pkg

security delete-keychain $KEYCHAIN_PATH
