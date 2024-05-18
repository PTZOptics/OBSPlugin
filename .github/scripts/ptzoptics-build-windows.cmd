cd ..
choco install wget -y
wget https://github.com/libsdl-org/SDL/releases/download/release-2.28.2/SDL2-devel-2.28.2-VC.zip
tar -xf SDL2-devel-2.28.2-VC.zip
set SDL2_DIR=%cd%\SDL2-2.28.2
cd obs-studio
powershell.exe "CI/build-windows.ps1"