#pragma once

struct aiMaterial;



class Material
{
public:
	Material();
	~Material();

	void Create(ID3D11Device* device,aiMaterial* pMaterial);
	ID3D11ShaderResourceView* m_pDiffuseRV = nullptr;	// �ؽ�ó ���ҽ� ��.
	ID3D11ShaderResourceView* m_pNormalRV = nullptr;	// �븻�� ���ҽ� ��.
	ID3D11ShaderResourceView* m_pSpecularRV = nullptr;	// ����ŧ���� ���ҽ� ��.
	ID3D11ShaderResourceView* m_pEmissiveRV = nullptr;	// �̹̽ú�� ���ҽ� ��.	
	ID3D11ShaderResourceView* m_pOpacityRV = nullptr;	// ����� ���ҽ� ��.	
};

