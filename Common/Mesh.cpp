#include "pch.h"
#include "Mesh.h"
#include "Helper.h"

#include <assimp/scene.h>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{

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
	// 버텍스 정보 생성
	unique_ptr<Vertex[]> vertices(new Vertex[mesh->mNumVertices]);

	

	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		memcpy_s(&vertices[i].Position, sizeof(vertices[i].Position),&mesh->mVertices[i],sizeof(mesh->mVertices[i]));
		memcpy_s(&vertices[i].Normal, sizeof(vertices[i].Normal), &mesh->mNormals[i], sizeof(mesh->mNormals[i]));
		memcpy_s(&vertices[i].TexCoord, sizeof(Vector2), &mesh->mTextureCoords[0][i], sizeof(Vector2));
		memcpy_s(&vertices[i].Tangent, sizeof(vertices[i].Tangent), &mesh->mTangents[i], sizeof(mesh->mTangents[i]));
	}
	CreateVertexBuffer(device, vertices.get(), mesh->mNumVertices);

	// 인덱스 정보 생성
	unique_ptr<WORD[]> indices(new WORD[mesh->mNumFaces * 3]);

	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
		indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	}

	CreateIndexBuffer(device, indices.get(), mesh->mNumFaces * 3);

}

