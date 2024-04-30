cd ..
choco install wget -y
choco install patch -y

echo "TESTPOINT 1"

wget https://github.com/libsdl-org/SDL/releases/download/release-2.28.2/SDL2-devel-2.28.2-VC.zip
tar -xf SDL2-devel-2.28.2-VC.zip

echo "TESTPOINT 2"
rem  git clone -b release/30.1 --recursive https://github.com/obsproject/obs-studio.git
git clone -b 30.1.0 --recursive https://github.com/obsproject/obs-studio.git
move OBSPlugin\src obs-studio\plugins\ptzoptics-obs
patch --ignore-whitespace obs-studio\plugins\CMakeLists.txt OBSPlugin\obs_plugins_cmakelists.patch || exit /b 1

echo "TESTPOINT 3"
echo "==== FILE AFTER PATCH BEGIN"
type obs-studio\plugins\CMakeLists.txt
echo "==== FILE AFTER PATCH ENDS"
dir %cd%

set SDL2_DIR=%cd%\SDL2-2.28.2
cd obs-studio
cmake --list-presets
cmake --preset windows-x64
cmake --build --preset windows-x64

echo "TESTPOINT 4"

cd ..

dir
echo "TESTPOINT 4.5"
dir obs-studio\build_x64\rundir\RelWithDebInfo\obs-plugins\64bit
echo "TESTPOINT 5"
dir OBSPlugin\installers-src\win\PtzObsPluginInstaller\

copy /Y obs-studio\build_x64\rundir\RelWithDebInfo\obs-plugins\64bit\obs-ptzcontroller.dll OBSPlugin\installers-src\win\PtzObsPluginInstaller\ || exit /b 1
echo "TESTPOINT 6"
copy /Y SDL2-2.28.2\lib\x64\SDL2.dll OBSPlugin\installers-src\win\PtzObsPluginInstaller\
echo "TESTPOINT 7"
cd OBSPlugin\installers-src\win
"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\amd64\MSBuild.exe" PtzObsPluginInstaller.sln /p:Configuration=Release /p:Platform=x64
echo "TESTPOINT 8"
