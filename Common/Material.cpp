#include "pch.h"
#include "Helper.h"
#include "Material.h"
#include <assimp/scene.h>


Material::Material()
{

}

Material::~Material()
{
	SAFE_RELEASE(m_pDiffuseRV);
	SAFE_RELEASE(m_pNormalRV);
	SAFE_RELEASE(m_pSpecularRV);
}

void Material::Create(ID3D11Device* device,aiMaterial* pMaterial)
{
	pMaterial->mNumProperties[_AI_MATKEY_TEXTURE_BASE];
}
