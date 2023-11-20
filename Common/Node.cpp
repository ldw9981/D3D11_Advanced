#include "pch.h"
#include "Node.h"
#include "Model.h"
#include "Helper.h"
#include "Skeleton.h"


Node::~Node()
{
	LOG_MESSAGEA("~Node() %s", m_Name.c_str());	
}

void Node::LoadSkeleton(Model* model,aiNode* node)
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
		model->m_Meshes[meshIndex].m_pNodeWorldTransform = &m_World;
	}

	UINT numChild = node->mNumChildren;	
	m_Children.resize(numChild);
	for (UINT i = 0; i < numChild; ++i)
	{
		m_Children[i].m_pParent = this;
		m_Children[i].LoadSkeleton(model,node->mChildren[i]);
	}
}

void Node::LoadSkeleton(Skeleton* skeleton)
{
	UINT count = skeleton->GetBoneCount();

	Bone* pBone = skeleton->GetBone(0);
	m_Name = pBone->Name;
	m_Children.reserve(pBone->NumChildren);	
		
	for (UINT i = 1; i < count; i++)
	{
		Bone* pBone = skeleton->GetBone(i);
		assert(pBone != nullptr);
		assert(pBone->ParentBoneIndex != -1);

		Node* pParentNode = FindNode(skeleton->GetBoneName(pBone->ParentBoneIndex));
		assert(pParentNode != nullptr);

		auto& node = pParentNode->m_Children.emplace_back();
		node.m_Name = pBone->Name;
		node.m_Local = pBone->RelativeTransform;
		node.m_Children.reserve(pBone->NumChildren);
		node.m_pParent = pParentNode;
	}
}

void Node::UpdateAnimation(float progressTime)
{
	// 노드의 애니메이션이 있다면 애니메이션을 업데이트한다.
	if (m_pNodeAnimation != nullptr)
	{
		Math::Vector3 position,scaling;
		Math::Quaternion rotation;
		m_pNodeAnimation->Evaluate(progressTime, position,rotation,scaling);
		m_Local = Math::Matrix::CreateScale(scaling) * Math::Matrix::CreateFromQuaternion(rotation) * Math::Matrix::CreateTranslation(position);	
	}

	// 부모 노드가 있다면 부모 노드의 WorldMatrix를 곱해서 자신의 WorldMatrix를 만든다.
	if (m_pParent != nullptr)
		m_World = m_Local * m_pParent->m_World;
	else
		m_World = m_Local;
	
	for (auto& child : m_Children)
	{	// 자식 노드들의 Update()를 호출한다.
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

