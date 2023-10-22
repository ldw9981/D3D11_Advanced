#include "pch.h"
#include "Model.h"
#include "Helper.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
#include <assimp/cimport.h>


bool Model::ReadFile(ID3D11Device* device,const char* filePath)
{
	LOG_MESSAGEA("Loading file: %s", filePath);
	Assimp::Importer importer;
	
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS,0);	// $assimp_fbx$ ��� ��������
	
	
	unsigned int importFlags = aiProcess_Triangulate | aiProcess_GenNormals |
		aiProcess_GenUVCoords | aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded;

	const aiScene* scene = importer.ReadFile(filePath, importFlags);
	if (!scene) {
		LOG_ERRORA("Error loading file: %s", importer.GetErrorString());
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
	// ä�μ��� aiAnimation �ȿ��� �ִϸ��̼� ������  ǥ���ϴ� aiNode�� �����̴�.
	assert(animation->mNumChannels > 1); // �ִϸ��̼��� �ִٸ� aiNode �� �ϳ� �̻� �־���Ѵ�.

	m_ClipAnimations.resize(1);
	m_ClipAnimations[0].NodeAnimations.resize(animation->mNumChannels);
	m_ClipAnimations[0].Duration = (float)(animation->mDuration / animation->mTicksPerSecond);
	for (size_t iChannel = 0; iChannel < animation->mNumChannels; iChannel++)
	{
		aiNodeAnim* pAiNodeAnim = animation->mChannels[iChannel];
		NodeAnimation& refNodeAnim = m_ClipAnimations[0].NodeAnimations[iChannel];
		refNodeAnim.Create(pAiNodeAnim, animation->mTicksPerSecond);
	}
	importer.FreeScene();
	UpdateNodeAnimationReference(0);

	LOG_MESSAGEA("Complete file: %s", filePath);
	return true;
}

Material* Model::GetMaterial(UINT index)
{
	assert(index < m_Materials.size());
	return &m_Materials[index];
}
void Model::Update(float deltaTime)
{
	if (!m_ClipAnimations.empty())
	{
		m_AnimationProressTime += deltaTime;
		m_AnimationProressTime = fmod(m_AnimationProressTime, m_ClipAnimations[0].Duration);
		m_RootNode.UpdateAnimation(m_AnimationProressTime);
		return;
	}
}

void Model::UpdateNodeAnimationReference(UINT clipIndex)
{
	assert(clipIndex < m_ClipAnimations.size());
	ClipAnimation& clipAnimation = m_ClipAnimations[clipIndex];
	for (size_t i = 0; i < clipAnimation.NodeAnimations.size(); i++)
	{
		NodeAnimation& nodeAnimation = clipAnimation.NodeAnimations[i];
		Node* node = m_RootNode.FindNode(nodeAnimation.NodeName);
		node->m_pNodeAnimation = &clipAnimation.NodeAnimations[i];
	}
}

void Model::SetWorldTransform(const Math::Matrix& transform)
{
	m_RootNode.m_Local = transform;
}