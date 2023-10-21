#include "pch.h"
#include "Node.h"
#include "Model.h"
#include "Helper.h"

void Node::Create(Model* model,aiNode* node)
{
	m_Name = node->mName.C_Str();	
	LOG_MESSAGEA(m_Name.c_str());

	// d3d�� �� �켱(column-major) assimp�� ����� �� �켱(row-major)  ����̴�.
	m_Local = Math::Matrix(&node->mTransformation.a1).Transpose();
		
	m_MeshIndices.resize(node->mNumMeshes);
	UINT meshIndex=0;
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{	
		meshIndex = node->mMeshes[i];
		m_MeshIndices[i] = meshIndex;

		// Mesh�� Node�� WorldMatrix�� �����Ѵ�.
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
	// �θ� ����� ���� ����� ����Ѵ�.
	if (m_pParent != nullptr)
	{
		m_World = m_Local * m_pParent->m_World;
	}
	else
	{
		m_World = m_Local;
	}

	// �ڽ� ������ Update()�� ȣ���Ѵ�.
	for (auto& child : m_Children)
	{
		child.Update(deltaTime);
	}
}

