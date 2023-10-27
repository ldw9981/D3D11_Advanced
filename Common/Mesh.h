#pragma once


// 정점 선언.
struct Vertex
{
	Math::Vector3 Position;		// 정점 위치 정보.	
	Math::Vector2 TexCoord;
	Math::Vector3 Normal;
	Math::Vector3 Tangent;
};

struct BoneWeightVertex
{
	Math::Vector3 Position;		// 정점 위치 정보.	
	Math::Vector2 TexCoord;
	Math::Vector3 Normal;
	Math::Vector3 Tangent;
	int BoneIndex[4] = {};
	float BoneWeight[4] = {};

	void AddBoneData(int boneIndex, float weight)
	{
		// 4개중 하나는 데이터가 비어있어야...
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
	// 계층 구조 노드가 소유한 World의 포인터를 설정
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
	UINT m_VertexBufferStride = 0;						// 버텍스 하나의 크기.
	UINT m_VertexBufferOffset = 0;						// 버텍스 버퍼의 오프셋.
	UINT m_IndexCount = 0;				// 인덱스 개수.
	UINT m_MaterialIndex = 0;			// 메테리얼 인덱스.
	UINT m_BoneCount=0;
};

