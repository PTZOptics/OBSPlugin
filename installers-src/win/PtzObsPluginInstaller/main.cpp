#include <windows.h>
#include <string>
#include "resource.h"
#include "ResourceFile.h"
#include <fstream>
#include <Shlobj.h>

std::wstring GetObsStudioInstallLocation()
{
	HKEY obsStudioKey;
	if (RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\OBS Studio", &obsStudioKey) != ERROR_SUCCESS)
		throw std::exception("OBS Studio registry key is not found");

	TCHAR value[2560];
	DWORD valueSize = sizeof(value);
	if (RegQueryValueEx(obsStudioKey, L"UninstallString", NULL, NULL, (LPBYTE)value, &valueSize) != ERROR_SUCCESS)
		throw std::exception("UninstallString is not found");
	if (RegCloseKey(obsStudioKey) != ERROR_SUCCESS)
		throw std::exception("Can't close registry key");

	std::wstring uninstallStr(value);
	if (uninstallStr[0] == '"')
		uninstallStr = uninstallStr.substr(1);
	size_t lastSlashPos = uninstallStr.find_last_of('\\');
	if (lastSlashPos == std::wstring::npos)
		throw std::exception("UninstallString doesn't have '\\' symbol");
	return uninstallStr.substr(0, lastSlashPos);	
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

			int sz = WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, NULL, 0, NULL, NULL);
			char* path_c = new char[sz];
			WideCharToMultiByte(CP_ACP, 0, path.c_str(), -1, path_c, sz, NULL, NULL);
			std::string path_c_s(path_c);
			delete[] path_c;
			throw std::exception(("Error writing '" + path_c_s + "'").c_str());
		}
		
		filestream.close();
	}
}

void Log(std::string str)
{
	char path[MAX_PATH + 1];
	if (SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path) != S_OK) return;
	std::string folder(path);
	folder += "\\PtzOpticsObsInstaller";
	CreateDirectoryA(folder.c_str(), NULL);
	std::ofstream log_file((folder + "\\log.txt").c_str());

	log_file << str;
	log_file.close();
}

bool Install()
{	
	try
	{		
		std::wstring folder = GetObsStudioInstallLocation();
		CopyFiles(folder);
	}
	catch (std::exception ex)
	{
		Log(ex.what());
		return false;
	}

	Log("Successfully installed");
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