#include "pch.h"
#include "Model.h"
#include "Helper.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool Model::ReadFile(ID3D11Device* device,const char* filePath)
{
	Assimp::Importer importer;
	unsigned int importFlags = aiProcess_Triangulate | aiProcess_GenNormals |
		aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | 
		aiProcess_ConvertToLeftHanded;

	const aiScene* scene = importer.ReadFile(filePath, importFlags);
	if (!scene) {
		LOG_ERRORA("Error loading FBX file: %s", importer.GetErrorString());
		return false;
	}

	aiMetadata* metadata = scene->mMetaData;

	std::string metadataStr;
	float metadataFloat;
	int metadataInt;
	aiString  key;
	if (metadata) {
		// Iterate through the metadata
		for (unsigned int i = 0; i < metadata->mNumProperties; i++) {
			const aiMetadataEntry& entry = metadata->mValues[i];
			key = metadata->mKeys[i];
			
			
			// Check the data type of the metadata entry
			if (entry.mType == aiMetadataType::AI_AISTRING) {
				metadataStr = std::string((char*)entry.mData);
			}
			else if (entry.mType == aiMetadataType::AI_INT32) {
				metadataInt = *reinterpret_cast<const int*>(entry.mData);
			}
			else if (entry.mType == aiMetadataType::AI_FLOAT) {
				metadataFloat  = *reinterpret_cast<const float*>(entry.mData);
			}			
		}
	}
	

	m_Materials.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		m_Materials[i].Create(device, scene->mMaterials[i]);
	}

	m_Meshes.resize(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		m_Meshes[i].Create(device, scene->mMeshes[i]);
	}

	assert(scene->mNumAnimations < 2); // �ִϸ��̼��� ���ų� 1�������Ѵ�. 
	// ����� �ִϸ��̼��� �ϳ��� ��ġ�� ����� FBX export���� NLA��Ʈ��,��� �׼� �ɼ��� ���ϴ�.

	if (scene->mNumAnimations == 0)
		return true;

	m_RootNode.Create(this,scene->mRootNode);

	const aiAnimation* animation = scene->mAnimations[0];
	unsigned int numChannels = animation->mNumChannels;
	assert(animation->mNumChannels > 1); // �ִϸ��̼� �Ǵ� ���(Bone)�� �ϳ� �̻� �־���Ѵ�.
	m_NodeAnimations.resize(animation->mNumChannels);
	for (size_t iChannel = 0; iChannel < numChannels; iChannel++)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[iChannel];
		NodeAnimation animation;
		animation.Create(nodeAnim);
	}
	importer.FreeScene();
	return true;
}

Material* Model::GetMaterial(UINT index)
{
	assert(index < m_Materials.size());
	return &m_Materials[index];
}
void Model::Update(float deltaTime)
{
	m_RootNode.Update(deltaTime);
}