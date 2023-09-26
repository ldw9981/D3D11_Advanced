#pragma once
#include <wchar.h>
#include <d3d11.h>
#include <exception>
#include <stdio.h>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <vector>

#define LOG_ERROR(...) \
{ \
    wchar_t buffer[256]; \
    swprintf_s(buffer,256, L"[ERROR] %s:%d - ", __FUNCTIONW__, __LINE__); \
    wchar_t message[256]; \
    swprintf_s(message,256, __VA_ARGS__); \
    wcscat_s(buffer, message); \
    wcscat_s(buffer, L"\n"); \
    MessageBoxW(NULL, buffer, L"LOG_ERROR", MB_OK); \
}

#define LOG_WARNING(...) \
{ \
    wchar_t buffer[256]; \
    swprintf_s(buffer,256, L"[WARNING] %s:%d - ", __FUNCTIONW__, __LINE__); \
    wchar_t message[256]; \
    swprintf_s(message,256, __VA_ARGS__); \
    wcscat_s(buffer, message); \
    wcscat_s(buffer, L"\n"); \
    OutputDebugStringW(buffer); \
}

#define LOG_MESSAGE(...) \
{ \
    wchar_t buffer[256]; \
    swprintf_s(buffer,256, L"[MESSAGE] %s:%d - ", __FUNCTIONW__, __LINE__); \
    wchar_t message[256]; \
    swprintf_s(message,256, __VA_ARGS__); \
    wcscat_s(buffer, message); \
    wcscat_s(buffer, L"\n"); \
    OutputDebugStringW(buffer); \
}


template <typename T>
void SAFE_RELEASE(T* p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

template <typename T>
void SAFE_DELETE(T* p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}


LPCWSTR GetComErrorString(HRESULT hr);





// Helper class for COM exceptions
class com_exception : public std::exception
{
public:
	com_exception(HRESULT hr) : result(hr) {}

	const char* what() const noexcept override
	{
		static char s_str[64] = {};
		sprintf_s(s_str, "Failure with HRESULT of %08X",
			static_cast<unsigned int>(result));
		return s_str;
	}

private:
	HRESULT result;
};

// Helper utility converts D3D API failures into exceptions.
inline void HR_T(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw com_exception(hr);
	}
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);





	inline std::vector<uint8_t> ReadData(_In_z_ const wchar_t* name)
	{
		std::ifstream inFile(name, std::ios::in | std::ios::binary | std::ios::ate);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
		if (!inFile)
		{
			wchar_t moduleName[_MAX_PATH] = {};
			if (!GetModuleFileNameW(nullptr, moduleName, _MAX_PATH))
				throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetModuleFileNameW");

			wchar_t drive[_MAX_DRIVE];
			wchar_t path[_MAX_PATH];

			if (_wsplitpath_s(moduleName, drive, _MAX_DRIVE, path, _MAX_PATH, nullptr, 0, nullptr, 0))
				throw std::runtime_error("_wsplitpath_s");

			wchar_t filename[_MAX_PATH];
			if (_wmakepath_s(filename, _MAX_PATH, drive, path, name, nullptr))
				throw std::runtime_error("_wmakepath_s");

			inFile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
		}
#endif

		if (!inFile)
			throw std::runtime_error("ReadData");

		const std::streampos len = inFile.tellg();
		if (!inFile)
			throw std::runtime_error("ReadData");

		std::vector<uint8_t> blob;
		blob.resize(size_t(len));

		inFile.seekg(0, std::ios::beg);
		if (!inFile)
			throw std::runtime_error("ReadData");

		inFile.read(reinterpret_cast<char*>(blob.data()), len);
		if (!inFile)
			throw std::runtime_error("ReadData");

		inFile.close();

		return blob;
	}
