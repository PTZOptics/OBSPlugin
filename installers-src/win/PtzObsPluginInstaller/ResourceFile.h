#pragma once
#include <Windows.h>

class ResourceFile
{
public:
	ResourceFile(int id, const TCHAR* type);
	void* Data();
	int Size();
	~ResourceFile();
private:
	HRSRC m_res;
	LPVOID m_resPtr;
	DWORD m_resSize;
};

