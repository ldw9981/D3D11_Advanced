#include "pch.h"
#include "Helper.h"
#include <comdef.h>
#include <d3dcompiler.h>
#include <Directxtk/DDSTextureLoader.h>
#include <Directxtk/WICTextureLoader.h>

LPCWSTR GetComErrorString(HRESULT hr)
{
	_com_error err(hr);
	LPCWSTR errMsg = err.ErrorMessage();
	return errMsg;
}

HRESULT CompileShaderFromFile(const WCHAR* szFileName,const D3D_SHADER_MACRO* pDefines,LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			MessageBoxA(NULL, (char*)pErrorBlob->GetBufferPointer(), "CompileShaderFromFile", MB_OK);
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT CreateTextureFromFile(ID3D11Device* d3dDevice, const wchar_t* szFileName, ID3D11ShaderResourceView** textureView)
{
	HRESULT hr = S_OK;

	// Load the Texture
	hr = DirectX::CreateDDSTextureFromFile(d3dDevice, szFileName, nullptr, textureView);
	if (FAILED(hr))
	{
		hr = DirectX::CreateWICTextureFromFile(d3dDevice, szFileName, nullptr, textureView);
		if (FAILED(hr))
		{
			MessageBoxW(NULL, GetComErrorString(hr), szFileName, MB_OK);
			return hr;
		}		
	}
	return S_OK;
}


DirectX::SimpleMath::Vector3 CalculateTangent(Vector3 v0, Vector3 v1, Vector3 v2, Vector2 tex0, Vector2 tex1, Vector2 tex2)
{
	Vector3 e0 = v1 - v0;
	Vector3 e1 = v2 - v0;
	Vector2 deltaUV1 = tex1 - tex0;
	Vector2 deltaUV2 = tex2 - tex0;
	
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	Vector3 tangent;
	tangent.x = f * (deltaUV2.y * e0.x - deltaUV1.y * e1.x);
	tangent.y = f * (deltaUV2.y * e0.y - deltaUV1.y * e1.y);
	tangent.z = f * (deltaUV2.y * e0.z - deltaUV1.y * e1.z);

	tangent.Normalize();

	return tangent;
}

std::wstring ToWString(const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());
	return wsTmp;
}
