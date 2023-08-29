choco install wget -y
choco install patch -y
wget https://github.com/libsdl-org/SDL/releases/download/release-2.28.2/SDL2-devel-2.28.2-VC.zip
tar -xf SDL2-devel-2.28.2-VC.zip
git clone -b release/29.1 --recursive https://%1@github.com/obsproject/obs-studio.git
move ptzoptics-obs\src obs-studio\plugins\ptzoptics-obs
patch obs-studio\plugins\CMakeLists.txt ptzoptics-obs\obs_plugins_cmakelists.patch || exit /b 1
set SDL2_DIR=%cd%\SDL2-2.28.2
cd obs-studio
powershell.exe "CI/build-windows.ps1"
cd ..
copy /Y obs-studio\build64\rundir\RelWithDebInfo\obs-plugins\64bit\obs-ptzcontroller.dll ptzoptics-obs\installers-src\win\PtzObsPluginInstaller\ || exit /b 1
copy /Y SDL2-2.28.2\lib\x64\SDL2.dll ptzoptics-obs\installers-src\win\PtzObsPluginInstaller\
cd ptzoptics-obs\installers-src\win
"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\amd64\MSBuild.exe" PtzObsPluginInstaller.sln /p:Configuration=Release /p:Platform=x64
