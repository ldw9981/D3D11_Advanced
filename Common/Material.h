#pragma once


using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;
using namespace std;

struct aiMaterial;
class Material
{
public:
	Material();
	~Material();
	Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };

	
	void Create(ID3D11Device* device,aiMaterial* pMaterial);
	ID3D11ShaderResourceView* m_pDiffuseRV = nullptr;	// �ؽ�ó ���ҽ� ��.
	ID3D11ShaderResourceView* m_pNormalRV = nullptr;	// �븻�� ���ҽ� ��.
	ID3D11ShaderResourceView* m_pSpecularRV = nullptr;	// ����ŧ���� ���ҽ� ��.
};

