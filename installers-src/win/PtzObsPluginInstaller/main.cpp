#include <windows.h>
#include <string>
#include "resource.h"
#include "ResourceFile.h"
#include <fstream>

std::wstring GetObsStudioInstallLocation()
{
	HKEY obsStudioKey;
	if (RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OBS Studio", &obsStudioKey) != ERROR_SUCCESS)
		throw std::exception();

	TCHAR value[2560];
	DWORD valueSize = sizeof(value);
	if (RegQueryValueEx(obsStudioKey, L"UninstallString", NULL, NULL, (LPBYTE)value, &valueSize) != ERROR_SUCCESS)
		throw std::exception();
	std::wstring uninstallStr(value);
	size_t lastSlashPos = uninstallStr.find_last_of('\\');
	if (lastSlashPos == std::wstring::npos)
		throw std::exception();
	return uninstallStr.substr(0, lastSlashPos);


	if (RegCloseKey(obsStudioKey) != ERROR_SUCCESS)
		throw std::exception();

}

struct ResFile
{
	int id;
	const TCHAR* filename;
};

void CopyFiles(std::wstring folder)
{
	ResFile files[] = {
		{IDR_PLUGIN_DLL, L"obs-ptzcontroller.dll" },
		{IDR_SDL_DLL, L"sdl2.dll" },
	};
	for (ResFile file : files)
	{
		ResourceFile pluginDllRes(file.id, L"BIN");
		void* d = pluginDllRes.Data();
		int s = pluginDllRes.Size();

		std::wstring path = folder + L"\\obs-plugins\\64bit\\" + file.filename;

		std::ofstream filestream(path, std::ios::out | std::ios::binary);				
		filestream.write((char*)d, s);

		std::ios::iostate  state = filestream.rdstate();
		if (state & filestream.failbit)
		{
			filestream.close();
			throw std::exception();
		}
		
		filestream.close();
	}
}

bool Install()
{
	try
	{		
		std::wstring folder = GetObsStudioInstallLocation();
		CopyFiles(folder);
	}
	catch (...)
	{
		return false;
	}

    return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{	
    bool res = Install();
    UINT type = res ? MB_ICONINFORMATION : MB_ICONERROR;
    MessageBox(NULL, res ? L"Installed" : L"Install failed", L"PTZ OBS Plugin", MB_OK | type);
    
    return 0;
}