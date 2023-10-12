#include "pch.h"
#include "Helper.h"
#include "Material.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>



Material::Material()
{

}

Material::~Material()
{
	SAFE_RELEASE(m_pDiffuseRV);
	SAFE_RELEASE(m_pNormalRV);
	SAFE_RELEASE(m_pSpecularRV);
	SAFE_RELEASE(m_pEmissiveRV);
}

void Material::Create(ID3D11Device* device,aiMaterial* pMaterial)
{
	// Diffuse
	aiString texturePath;
	wstring basePath=L"../Resource/";
	std::filesystem::path path;
	wstring finalPath;

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
}
