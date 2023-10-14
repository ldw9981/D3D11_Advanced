#pragma once
#include "../Common/GameApp.h"
#include "../Common/Mesh.h"
#include "../Common/Material.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

struct CB_Transform
{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
}; 

struct CB_DirectionLight
{
	Vector3 Direction = { 0.5f, 0.0f, 0.8f };
	float pad0;
	Vector4 Ambient = { 0.1f,0.1f,0.1f,0.1f };
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 EyePosition;
	float pad1;
};



struct CB_Marterial
{
	Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };	// 16byte
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };	// 16byte
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };  // 16byte
	Vector4 Emissive = { 1.0f,1.0f,1.0f,1.0f };  // 16byte
	float  SpecularPower = 80;					// 4
	bool UseDiffuseMap = true;					
	bool pad1[3];								// 8
	bool UseNormalMap = true;
	bool pad2[3];								// 12
	bool UseSpecularMap = true;
	bool pad3[3];								// 16byte				
	bool UseEmissiveMap = true;			
	bool pad4[3];								// 4
	bool UseOpacityMap = true;
	bool pad5[3];								// 8
	Vector2 pad6;								// 16byte
};



class TutorialApp :
    public GameApp
{
public:
	TutorialApp(HINSTANCE hInstance);
	~TutorialApp();

	// ������ ������������ �����ϴ� �ʼ� ��ü�� �������̽�
	ID3D11Device* m_pDevice = nullptr;						// ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;	// ����/���ٽ� ��

	// ������ ���������ο� �����ϴ� ���ҽ� ��ü�� �������̽�
	ID3D11Buffer* m_pVertexBuffer = nullptr;			// ���� ����.
	ID3D11Buffer* m_pIndexBuffer = nullptr;				// �ε��� ����.
	ID3D11VertexShader* m_pVertexShader = nullptr;		// ���� ���̴�.
	ID3D11PixelShader* m_pPixelShader = nullptr;		// �ȼ� ���̴�.
	ID3D11PixelShader* m_pPixelShaderSolid = nullptr;	// �ȼ� ���̴�.
	ID3D11InputLayout* m_pInputLayout = nullptr;		// �Է� ���̾ƿ�.
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// ���÷� ����.
	ID3D11BlendState* m_AlphaBlendState = nullptr;		// ���� ���� ���� (������ó�������� �������)
	ID3D11Buffer* m_pCBMaterial = nullptr;				// ��� ����: ��ȯ���
	ID3D11Buffer* m_pCBTransform = nullptr;				// ��� ����: ��ȯ���
	ID3D11Buffer* m_pCBDirectionLight = nullptr;		// ��� ����: ���Ɽ

	// ������ ���������ο� �����ϴ� ����
	UINT m_VertexBufferStride = 0;						// ���ؽ� �ϳ��� ũ��.
	UINT m_VertexBufferOffset = 0;						// ���ؽ� ������ ������.
	int m_nIndices = 0;				// �ε��� ����.
	Matrix	m_World;				// ������ǥ�� �������� ��ȯ�� ���� ���.
	Matrix  m_View;					// ����ǥ�� �������� ��ȯ�� ���� ���.
	Matrix  m_Projection;			// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.

	Vector3 m_ClearColor ={ 0.0f, 0.0f, 0.0f};
	Vector2 m_Rotation = Vector2(0.0f, 0.0f);	
	Vector3 m_CameraPos = Vector3(0.0f, 200.0f, -1000.0f);

	CB_Transform m_Transform;
	CB_Marterial m_CBMaterial;
	CB_DirectionLight m_Light;
	float m_MeshScale=1.0f;
	
	
	vector<Mesh> m_Meshes;	
	vector<Material> m_Materials;

	virtual bool Initialize(UINT Width, UINT Height);
	virtual void Update();
	virtual void Render();

	bool InitD3D();			
	void UninitD3D();

	bool InitScene();		// ���̴�,���ؽ�,�ε���
	void UninitScene();		 

	bool InitImGUI();
	void UninitImGUI();

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

