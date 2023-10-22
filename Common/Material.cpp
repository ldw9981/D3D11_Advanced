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
	/* 
	//프로퍼티 테스트 코드
	std::vector<std::string> properties;
	for(size_t i=0; i<pMaterial->mNumProperties; i++)
	{
		aiMaterialProperty* pProperty = pMaterial->mProperties[i];
		aiString * propertyName = &pProperty->mKey;
		float* propertyValueFloat = nullptr;
		const char* propertyValueString = nullptr;
		int* propertyValueInt=nullptr;
		if (pProperty->mType == aiPTI_Float) {
			// Handle float type
			propertyValueFloat = (float*)pProperty->mData;
			// Store floatValue in a variable or use it as needed
		}
		else if (pProperty->mType == aiPTI_String) {
			// Handle string type
			propertyValueString = pProperty->mData;
			// Store stringValue in a variable or use it as needed
		}
		else if (pProperty->mType == aiPTI_Integer) {
			// Handle integer type
			propertyValueInt = (int*)pProperty->mData;
			// Store intValue in a variable or use it as needed
		}
		properties.push_back(pProperty->mKey.C_Str());
	}
	*/
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
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pNormalRV));
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pSpecularRV));
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pEmissiveRV));
	}

	if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_OPACITY, 0, &texturePath))
	{
		path = ToWString(string(texturePath.C_Str()));
		finalPath = basePath + path.filename().wstring();
		HR_T(CreateTextureFromFile(device, finalPath.c_str(), &m_pOpacityRV));
	}	
}
