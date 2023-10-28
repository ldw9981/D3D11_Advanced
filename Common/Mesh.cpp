#include "pch.h"
#include "Mesh.h"
#include "Helper.h"
#include "Node.h"

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

	// 버텍스 버퍼 정보
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

	// 버텍스 버퍼 정보
	m_VertexCount = vertexCount;
	m_VertexBufferStride = sizeof(BoneWeightVertex);
	m_VertexBufferOffset = 0;
}

void Mesh::CreateIndexBuffer(ID3D11Device* device, WORD* indices, UINT indexCount)
{
	// 인덱스 개수 저장.
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

void Mesh::Create(ID3D11Device* device, aiMesh* mesh)
{
	m_MaterialIndex = mesh->mMaterialIndex;

	// 버텍스 정보 생성
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
		
		UINT meshBoneCount = mesh->mNumBones;
		// 본정보 컨테이너 크기 조절
		m_Bones.resize(meshBoneCount);
		
		// 메쉬와 연결된 본들을 처리
		UINT boneIndexCounter = 0;
		std::map<std::string, int> BoneMapping;
		for (UINT i = 0; i < mesh->mNumBones; ++i)
		{
		
			aiBone* bone = mesh->mBones[i];			
			std::string boneName = bone->mName.C_Str();
			UINT boneIndex = 0;
			
			if (BoneMapping.find(boneName) == BoneMapping.end())
			{
				// Map bone name to bone index
				boneIndex = boneIndexCounter;
				boneIndexCounter++;
				m_Bones[boneIndex].NodeName = boneName;
				m_Bones[boneIndex].OffsetMatrix = Math::Matrix(&bone->mOffsetMatrix.a1).Transpose();
				BoneMapping[boneName] = boneIndex;
			}
			else
			{
				boneIndex = BoneMapping[boneName];
			}
					
			// 본과 연결된 버텍스들을 처리
			for (UINT j = 0; j < bone->mNumWeights; ++j)
			{
				UINT vertexID = bone->mWeights[j].mVertexId;
				float weight = bone->mWeights[j].mWeight;
				m_BoneWeightVertices[vertexID].AddBoneData(boneIndex, weight);
			}
		}		
		CreateBoneWeightVertexBuffer(device, &m_BoneWeightVertices[0], (UINT)m_BoneWeightVertices.size());
	}
	
	// 인덱스 정보 생성
	m_Indices.resize(mesh->mNumFaces * 3);
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		m_Indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		m_Indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		m_Indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}
	CreateIndexBuffer(device, &m_Indices[0], (UINT)m_Indices.size());
}

void Mesh::UpdateBoneNodePtr(Node* pRootNode)
{
	assert(pRootNode != nullptr);
	
	for (auto& bone : m_Bones)
	{
		bone.NodeWorldMatrixPtr = &pRootNode->FindNode(bone.NodeName)->m_World;
	}
}

void Mesh::UpdateMatrixPallete(Math::Matrix* MatrixPalletePtr)
{
	assert(m_Bones.size()<256);
	for (UINT i = 0; i < m_Bones.size(); ++i)
	{
		Math::Matrix& BoneNodeWorldMatrix = *m_Bones[i].NodeWorldMatrixPtr;
		MatrixPalletePtr[i] = (m_Bones[i].OffsetMatrix * BoneNodeWorldMatrix).Transpose();
	}
}