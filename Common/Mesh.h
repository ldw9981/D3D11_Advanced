#pragma once


// ���� ����.
struct Vertex
{
	Math::Vector3 Position;		// ���� ��ġ ����.	
	Math::Vector2 TexCoord;
	Math::Vector3 Normal;
	Math::Vector3 Tangent;
};

struct BoneWeightVertex
{
	Math::Vector3 Position;		// ���� ��ġ ����.	
	Math::Vector2 TexCoord;
	Math::Vector3 Normal;
	Math::Vector3 Tangent;
	int BoneIndex[4] = {};
	float BoneWeight[4] = {};

	void AddBoneData(int boneIndex, float weight)
	{
		// 4���� �ϳ��� �����Ͱ� ����־��...
		assert(BoneWeight[0] == 0.0f || BoneWeight[1] == 0.0f || BoneWeight[2] == 0.0f || BoneWeight[3] == 0.0f);
		for (int i = 0; i < 4; i++)
		{
			if (BoneWeight[i] == 0.0f)
			{
				BoneIndex[i] = boneIndex;
				BoneWeight[i] = weight;
				return;
			}
		}
	}
};

struct BoneInfo
{
	std::string NodeName;
	Math::Matrix OffsetMatrix;
	Math::Matrix* NodeWorldPtr;
};

struct aiMesh;
class Mesh
{
public:
	Mesh();
	~Mesh();
public:	
	void CreateVertexBuffer(ID3D11Device* device, Vertex* vertices, UINT vertexCount);
	void CreateBoneWeightVertexBuffer(ID3D11Device* device, BoneWeightVertex* vertices, UINT vertexCount);

	void CreateIndexBuffer(ID3D11Device* device,WORD* indies, UINT indexCount);
	void Create(ID3D11Device* device, aiMesh* mesh);
	// ���� ���� ��尡 ������ World�� �����͸� ����
	void SetNodeWorldPtr(Math::Matrix* world) { m_pNodeWorld = world; }

	std::vector<Vertex> m_Vertices;
	std::vector<BoneWeightVertex> m_BoneWeightVertices;
	std::vector<WORD> m_Indices;

	std::vector<BoneInfo> m_BoneInfo;
	std::map<std::string,int> m_BoneMapping;

	Math::Matrix* m_pNodeWorld = nullptr;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	UINT m_VertexCount=0;
	UINT m_VertexBufferStride = 0;						// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;						// ���ؽ� ������ ������.
	UINT m_IndexCount = 0;				// �ε��� ����.
	UINT m_MaterialIndex = 0;			// ���׸��� �ε���.
	UINT m_BoneCount=0;
};

