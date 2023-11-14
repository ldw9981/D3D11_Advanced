#include "pch.h"
#include "Mesh.h"
#include "Helper.h"
#include "Node.h"
#include "Skeleton.h"

using namespace DirectX;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

void Mesh::CreateVertexBuffer(ID3D11Device* device, Vertex* vertices, UINT vertexCount)
{
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(Vertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;
	HR_T(device->CreateBuffer(&bd, &vbData, &m_pVertexBuffer));

	// ���ؽ� ���� ����
	m_VertexCount = vertexCount;
	m_VertexBufferStride = sizeof(Vertex);
	m_VertexBufferOffset = 0;
}

void Mesh::CreateBoneWeightVertexBuffer(ID3D11Device* device, BoneWeightVertex* vertices, UINT vertexCount)
{
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(BoneWeightVertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;
	HR_T(device->CreateBuffer(&bd, &vbData, &m_pVertexBuffer));

	// ���ؽ� ���� ����
	m_VertexCount = vertexCount;
	m_VertexBufferStride = sizeof(BoneWeightVertex);
	m_VertexBufferOffset = 0;
}

void Mesh::CreateIndexBuffer(ID3D11Device* device, WORD* indices, UINT indexCount)
{
	// �ε��� ���� ����.
	m_IndexCount = indexCount;

	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(WORD) * indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices;
	HR_T(device->CreateBuffer(&bd, &ibData, &m_pIndexBuffer));
}

void Mesh::Create(ID3D11Device* device, aiMesh* mesh, Skeleton* skeleton)
{
	m_MaterialIndex = mesh->mMaterialIndex;

	// ���ؽ� ���� ����
	if (!mesh->HasBones())
	{
		//Static Mesh
		m_Vertices.resize(mesh->mNumVertices);
		for (UINT i = 0; i < mesh->mNumVertices; ++i)
		{
			m_Vertices[i].Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			m_Vertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			m_Vertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			m_Vertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		}
		CreateVertexBuffer(device, &m_Vertices[0], (UINT)m_Vertices.size());
	}
	else
	{
		//Skeletal Mesh
		m_BoneWeightVertices.resize(mesh->mNumVertices);
		for (UINT i = 0; i < mesh->mNumVertices; ++i)
		{
			m_BoneWeightVertices[i].Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			m_BoneWeightVertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			m_BoneWeightVertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			m_BoneWeightVertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

		}
		
		UINT meshBoneCount = mesh->mNumBones;	// �޽��� ����� ������
		m_BoneReferences.resize(meshBoneCount); // �� ���� ���� �����̳� ũ�� ����		
	
		for (UINT i = 0; i < meshBoneCount; ++i)
		{		
			aiBone* bone = mesh->mBones[i];			
			
			UINT boneIndex = skeleton->GetBoneIndex(bone->mName.C_Str());			
			assert(boneIndex != -1);
			Bone* pBone = skeleton->GetBone(boneIndex);
			assert(pBone != nullptr);
			
			m_BoneReferences[i].NodeName = bone->mName.C_Str();
			
			pBone->OffsetMatrix = Math::Matrix(&bone->mOffsetMatrix.a1).Transpose();

			m_BoneReferences[i].BoneIndex = boneIndex;

			// ���� ����� ���ؽ����� ó��
			for (UINT j = 0; j < bone->mNumWeights; ++j)
			{
				UINT vertexID = bone->mWeights[j].mVertexId;
				float weight = bone->mWeights[j].mWeight;
				m_BoneWeightVertices[vertexID].AddBoneData(boneIndex, weight);
			}
		}		
		CreateBoneWeightVertexBuffer(device, &m_BoneWeightVertices[0], (UINT)m_BoneWeightVertices.size());
	}
	
	// �ε��� ���� ����
	m_Indices.resize(mesh->mNumFaces * 3);
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		m_Indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		m_Indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		m_Indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}
	CreateIndexBuffer(device, &m_Indices[0], (UINT)m_Indices.size());
}

void Mesh::UpdateBoneNodePtr(Node* pRootNode,Skeleton* skeleton)
{
	assert(pRootNode != nullptr);
	
	for (auto& bone : m_BoneReferences)
	{
		Node* pNode = pRootNode->FindNode(bone.NodeName);
		assert(pNode!=nullptr);
		bone.NodeWorldMatrixPtr = &pNode->m_World;
	}
}

void Mesh::UpdateMatrixPallete(Math::Matrix* MatrixPalletePtr, Skeleton* skeleton)
{
	assert(m_BoneReferences.size()< 128);
	for (UINT i = 0; i < m_BoneReferences.size(); ++i)
	{
		Math::Matrix& BoneNodeWorldMatrix = *m_BoneReferences[i].NodeWorldMatrixPtr;
		// HLSL ������ۿ� ������Ʈ�Ҷ� �ٷ� �����Ҽ��ֵ��� ��ġ�ؼ� �����Ѵ�.

		int BoneIndex = m_BoneReferences[i].BoneIndex;
		Bone* pBone = skeleton->GetBone(BoneIndex); 

		MatrixPalletePtr[BoneIndex] = (pBone->OffsetMatrix * BoneNodeWorldMatrix).Transpose();
	}
}