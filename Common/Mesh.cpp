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

void Mesh::Create(ID3D11Device* device, aiMesh* mesh, Skeleton* skeleton)
{
	m_MaterialIndex = mesh->mMaterialIndex;
	m_Name = mesh->mName.C_Str();

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

		UINT meshBoneCount = mesh->mNumBones;	// 메쉬와 연결된 본개수
		m_BoneReferences.resize(meshBoneCount); // 본 연결 정보 컨테이너 크기 조절		

		for (UINT i = 0; i < meshBoneCount; ++i)
		{
			aiBone* bone = mesh->mBones[i];

			UINT boneIndex = skeleton->GetBoneIndexByBoneName(bone->mName.C_Str());
			assert(boneIndex != -1);
			Bone* pBone = skeleton->GetBone(boneIndex);
			assert(pBone != nullptr);

			m_BoneReferences[i].NodeName = bone->mName.C_Str();

			pBone->OffsetMatrix = Math::Matrix(&bone->mOffsetMatrix.a1).Transpose();

			m_BoneReferences[i].BoneIndex = boneIndex;

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

void Mesh::UpdateNodeInstancePtr(Node* pRootNode, Skeleton* skeleton)
{
	assert(pRootNode != nullptr);
	if (m_BoneReferences.empty())
	{
		Bone* pBone = skeleton->GetBone(skeleton->GetBoneIndexByMeshName(m_Name));
		assert(pBone != nullptr);

		Node* pNode = pRootNode->FindNode(pBone->Name);
		assert(pNode != nullptr);
		m_pNodeWorldTransform = &pNode->m_World;
	}
	else
	{
		for (auto& bone : m_BoneReferences)
		{
			Node* pNode = pRootNode->FindNode(bone.NodeName);
			assert(pNode != nullptr);
			bone.NodeWorldMatrixPtr = &pNode->m_World;
		}
	}

}

void Mesh::UpdateMatrixPallete(CB_MatrixPalette* pMatrixPallete, Skeleton* skeleton)
{
	assert(m_BoneReferences.size() < 128);
	for (UINT i = 0; i < m_BoneReferences.size(); ++i)
	{
		Math::Matrix& BoneNodeWorldMatrix = *m_BoneReferences[i].NodeWorldMatrixPtr;
		// HLSL 상수버퍼에 업데이트할때 바로 복사할수있도록 전치해서 저장한다.

		int BoneIndex = m_BoneReferences[i].BoneIndex;
		Bone* pBone = skeleton->GetBone(BoneIndex);

		pMatrixPallete->Array[BoneIndex] = (pBone->OffsetMatrix * BoneNodeWorldMatrix).Transpose();
	}
}

void Mesh::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}



StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}

template<class T>
void StaticMesh::CreateVertexBuffer(ID3D11Device* device, T* vertices, UINT vertexCount)
{
	D3D11_BUFFER_DESC bd = {};
	bd.ByteWidth = sizeof(T) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices;
	HR_T(device->CreateBuffer(&bd, &vbData, &m_pVertexBuffer));

	// 버텍스 버퍼 정보
	m_VertexCount = vertexCount;
	m_VertexBufferStride = sizeof(T);
	m_VertexBufferOffset = 0;
}

void StaticMesh::CreateIndexBuffer(ID3D11Device* device, WORD* indices, UINT indexCount)
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

void StaticMesh::Create(ID3D11Device* device, aiMesh* mesh, Skeleton* skeleton)
{
	m_MaterialIndex = mesh->mMaterialIndex;
	m_Name = mesh->mName.C_Str();


	CreateVertexBuffer<Vertex>(device, &m_Vertices[0], (UINT)m_Vertices.size());

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

void StaticMesh::UpdateNodeInstancePtr(Node* pRootNode, Skeleton* skeleton)
{
	assert(pRootNode != nullptr);

	Bone* pBone = skeleton->GetBone(skeleton->GetBoneIndexByMeshName(m_Name));
	assert(pBone != nullptr);

	Node* pNode = pRootNode->FindNode(pBone->Name);
	assert(pNode != nullptr);
	m_pNodeWorldTransform = &pNode->m_World;
}


void StaticMesh::Render(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	deviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexBufferStride, &m_VertexBufferOffset);
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}




SkinnedMesh::SkinnedMesh()
{

}
SkinnedMesh::~SkinnedMesh()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
}


void SkinnedMesh::Create(ID3D11Device* device, aiMesh* mesh, Skeleton* skeleton)
{
	m_MaterialIndex = mesh->mMaterialIndex;
	m_Name = mesh->mName.C_Str();

	// 버텍스 정보 생성
	//Skeletal Mesh
	m_BoneWeightVertices.resize(mesh->mNumVertices);
	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		m_BoneWeightVertices[i].Position = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		m_BoneWeightVertices[i].Normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		m_BoneWeightVertices[i].TexCoord = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		m_BoneWeightVertices[i].Tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
	}
	UINT meshBoneCount = mesh->mNumBones;	// 메쉬와 연결된 본개수
	m_BoneReferences.resize(meshBoneCount); // 본 연결 정보 컨테이너 크기 조절		
	for (UINT i = 0; i < meshBoneCount; ++i)
	{
		aiBone* bone = mesh->mBones[i];
		UINT boneIndex = skeleton->GetBoneIndexByBoneName(bone->mName.C_Str());
		assert(boneIndex != -1);
		Bone* pBone = skeleton->GetBone(boneIndex);
		assert(pBone != nullptr);
		m_BoneReferences[i].NodeName = bone->mName.C_Str();
		pBone->OffsetMatrix = Math::Matrix(&bone->mOffsetMatrix.a1).Transpose();
		m_BoneReferences[i].BoneIndex = boneIndex;
		// 본과 연결된 버텍스들을 처리
		for (UINT j = 0; j < bone->mNumWeights; ++j)
		{
			UINT vertexID = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			m_BoneWeightVertices[vertexID].AddBoneData(boneIndex, weight);
		}
	}
	CreateVertexBuffer<BoneWeightVertex>(device, &m_BoneWeightVertices[0], (UINT)m_BoneWeightVertices.size());

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

void SkinnedMesh::UpdateNodeInstancePtr(Node* pRootNode, Skeleton* skeleton)
{
	assert(pRootNode != nullptr);
	for (auto& bone : m_BoneReferences)
	{
		Node* pNode = pRootNode->FindNode(bone.NodeName);
		assert(pNode != nullptr);
		bone.NodeWorldMatrixPtr = &pNode->m_World;
	}
}

void SkinnedMesh::UpdateMatrixPallete(CB_MatrixPalette* pMatrixPallete, Skeleton* skeleton)
{
	assert(m_BoneReferences.size() < 128);
	for (UINT i = 0; i < m_BoneReferences.size(); ++i)
	{
		Math::Matrix& BoneNodeWorldMatrix = *m_BoneReferences[i].NodeWorldMatrixPtr;
		// HLSL 상수버퍼에 업데이트할때 바로 복사할수있도록 전치해서 저장한다.

		int BoneIndex = m_BoneReferences[i].BoneIndex;
		Bone* pBone = skeleton->GetBone(BoneIndex);

		pMatrixPallete->Array[BoneIndex] = (pBone->OffsetMatrix * BoneNodeWorldMatrix).Transpose();
	}
}
