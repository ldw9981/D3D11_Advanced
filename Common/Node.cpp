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

void Node::UpdateAnimation(float progressTime)
{
	// ����� �ִϸ��̼��� �ִٸ� �ִϸ��̼��� ������Ʈ�Ѵ�.
	if (m_pNodeAnimation != nullptr)
	{
		Math::Vector3 position;
		Math::Quaternion rotation;
		Math::Vector3 scaling;
		m_pNodeAnimation->Evaluate(progressTime, position,rotation,scaling);

		m_Local = Math::Matrix::CreateScale(scaling) * Math::Matrix::CreateFromQuaternion(rotation) * Math::Matrix::CreateTranslation(position);	
	}

	// �θ� ��尡 �ִٸ� �θ� ����� WorldMatrix�� ���ؼ� �ڽ��� WorldMatrix�� �����.
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
		child.UpdateAnimation(progressTime);
	}
}

Node* Node::FindNode(const std::string& name)
{
	if (m_Name == name)
		return this;

	for (auto& child : m_Children)
	{
		Node* found = child.FindNode(name);
		if (found != nullptr)
			return found;
	}

	return nullptr;
}

