#include "pch.h"
#include "Node.h"
#include "Model.h"
#include "Helper.h"

void Node::Create(Model* model,aiNode* node)
{
	m_Name = node->mName.C_Str();	
	LOG_MESSAGEA(m_Name.c_str());

	// d3d는 열 우선(column-major) assimp의 행렬은 행 우선(row-major)  행렬이다.
	m_Local = Math::Matrix(&node->mTransformation.a1).Transpose();
		
	m_MeshIndices.resize(node->mNumMeshes);
	UINT meshIndex=0;
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{	
		meshIndex = node->mMeshes[i];
		m_MeshIndices[i] = meshIndex;

		// Mesh와 Node의 WorldMatrix를 연결한다.
		model->m_Meshes[meshIndex].m_pWorldMatrix = &m_World;
	}

	UINT numChild = node->mNumChildren;	
	m_Children.resize(numChild);
	for (UINT i = 0; i < numChild; ++i)
	{
		m_Children[i].m_pParent = this;
		m_Children[i].Create(model,node->mChildren[i]);
	}
}

void Node::Update(float deltaTime)
{
	// 부모 노드의 월드 행렬을 계산한다.
	if (m_pParent != nullptr)
	{
		m_World = m_Local * m_pParent->m_World;
	}
	else
	{
		m_World = m_Local;
	}

	// 자식 노드들의 Update()를 호출한다.
	for (auto& child : m_Children)
	{
		child.Update(deltaTime);
	}
}

