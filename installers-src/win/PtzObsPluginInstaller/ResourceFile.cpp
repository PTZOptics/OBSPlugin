#include "ResourceFile.h"
#include <exception>

ResourceFile::ResourceFile(int id, const TCHAR* type)
{
	m_res = FindResource(NULL, MAKEINTRESOURCE(id), type);
	if (m_res == NULL)
		throw std::exception("Resource is not found");
	m_resSize = SizeofResource(NULL, m_res);
	if (m_resSize == 0)
		throw std::exception("Resource is empty");
	HGLOBAL resHandle = LoadResource(NULL, m_res);
	if (resHandle == NULL)
		throw std::exception("Can't load resource");
	m_resPtr = LockResource(resHandle);
	if (m_resPtr == NULL)
		throw std::exception("Can't lock resource");
}

void* ResourceFile::Data()
{
	return m_resPtr;
}

int ResourceFile::Size()
{
	return m_resSize;
}

ResourceFile::~ResourceFile()
{

}