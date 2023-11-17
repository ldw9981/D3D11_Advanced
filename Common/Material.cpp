#include "pch.h"
#include "Helper.h"
#include "Material.h"

#include <assimp/material.h>


Material::Material()
{

}

Material::~Material()
{
	SAFE_RELEASE(m_pDiffuseRV);
	SAFE_RELEASE(m_pNormalRV);
	SAFE_RELEASE(m_pSpecularRV);
	SAFE_RELEASE(m_pEmissiveRV);
	SAFE_RELEASE(m_pOpacityRV);
}

void Material::Create(ID3D11Device* device,aiMaterial* pMaterial)
{
	// Diffuse
	aiString texturePath;
	wstring basePath=L"../Resource/";
	std::filesystem::path path;
	wstring finalPath;
	string name = pMaterial->GetName().C_Str();
	
	//프로퍼티 테스트 코드
	std::vector<std::pair<std::string,int>> propertiesInt;
	std::vector<std::pair<std::string,float>> propertiesFloat;
	std::vector<std::pair<std::string,Vector2>> propertiesFloat2;
	std::vector<std::pair<std::string,Vector3>> propertiesFloat3;
	std::vector<std::pair<std::string,Vector4>> propertiesFloat4;
	std::vector<std::pair<std::string,std::string>> propertiesString;
	for(size_t i=0; i<pMaterial->mNumProperties; i++)
	{
		aiMaterialProperty* pProperty = pMaterial->mProperties[i];
	
		if (pProperty->mType == aiPTI_Float) {
			if (pProperty->mDataLength == 4)
				propertiesFloat.push_back({ pProperty->mKey.C_Str(),*(float*)pProperty->mData });
			else if (pProperty->mDataLength == 8)
				propertiesFloat2.push_back({ pProperty->mKey.C_Str(),*(Vector2*)pProperty->mData });
			else if (pProperty->mDataLength == 12)
				propertiesFloat3.push_back({ pProperty->mKey.C_Str(),*(Vector3*)pProperty->mData });
			else if (pProperty->mDataLength == 16)
				propertiesFloat4.push_back({ pProperty->mKey.C_Str(),*(Vector4*)pProperty->mData });
		}
		else if (pProperty->mType == aiPTI_String) {
			propertiesString.push_back({pProperty->mKey.C_Str(),pProperty->mData});	
		}
		else if (pProperty->mType == aiPTI_Integer) {
			propertiesInt.push_back({pProperty->mKey.C_Str(),*(int*)pProperty->mData});
		}		
	}
	
	aiColor3D color(1.f, 1.f, 1.f);
	if(AI_SUCCESS == pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
	{
		m_Color ={ color.r, color.g, color.b , 1};
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath)) 
	{
		path = ToWString(string(texturePath.C_Str()));		
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile( device, finalPath.c_str(),&m_pDiffuseRV));
		m_FilePathDiffuse = finalPath;
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pNormalRV));
		m_FilePathNormal = finalPath;
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pSpecularRV));
		m_FilePathSpecular = finalPath;
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pEmissiveRV));
		m_FilePathEmissive = finalPath;
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_OPACITY, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pOpacityRV));
		m_FilePathOpacity = finalPath;
	}	
}

void Material::ApplyDeviceContext(ID3D11DeviceContext* deviceContext , CB_Marterial* cpuCbMaterial,ID3D11Buffer* gpuCbMarterial, ID3D11BlendState* alphaBlendState )
{
	deviceContext->PSSetShaderResources(0, 1, &m_pDiffuseRV);
	deviceContext->PSSetShaderResources(1, 1, &m_pNormalRV);
	deviceContext->PSSetShaderResources(2, 1, &m_pSpecularRV);
	deviceContext->PSSetShaderResources(3, 1, &m_pEmissiveRV);
	deviceContext->PSSetShaderResources(4, 1, &m_pOpacityRV);

	cpuCbMaterial->Diffuse = m_Color;
	cpuCbMaterial->UseDiffuseMap = m_pDiffuseRV != nullptr ? true : false;
	cpuCbMaterial->UseNormalMap = m_pNormalRV != nullptr ? true : false;
	cpuCbMaterial->UseSpecularMap = m_pSpecularRV != nullptr ? true : false;
	cpuCbMaterial->UseEmissiveMap = m_pEmissiveRV != nullptr ? true : false;
	cpuCbMaterial->UseOpacityMap = m_pOpacityRV != nullptr ? true : false;

	if (cpuCbMaterial->UseOpacityMap && alphaBlendState != nullptr)
		deviceContext->OMSetBlendState(alphaBlendState, nullptr, 0xffffffff); // 알파블렌드 상태설정 , 다른옵션은 기본값 
	else
		deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);	// 설정해제 , 다른옵션은 기본값

	deviceContext->UpdateSubresource(gpuCbMarterial, 0, nullptr, cpuCbMaterial, 0, 0);
}
