#pragma once
#include <windows.h>
#include "../Common/GameApp.h"
#include <d3d11.h>
#include <directxtk/SimpleMath.h>


using namespace DirectX::SimpleMath;
using namespace DirectX;


// ���� ����.
struct Vertex
{
	Vector3 Position;		// ���� ��ġ ����.	
	Vector2 TexCoord;
	Vector3 Normal;
	Vector3 Tangent;
};

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
	Vector4 Ambient=	{ 0.1f,0.1f,0.1f,0.1f};
	Vector4 Diffuse =	{ 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular =	{ 1.0f,1.0f,1.0f,1.0f };
	Vector3 EyePosition;
	bool UseNormalMap=true;
};

static_assert((sizeof(CB_DirectionLight) % 16) == 0,
	"Constant Buffer size must be 16-byte aligned");

struct CB_Marterial
{
	Vector4 Ambient = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Diffuse = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 Specular = { 1.0f,1.0f,1.0f,1.0f };
	float  SpecularPower=80;
	Vector3 dummy;
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
	ID3D11ShaderResourceView* m_pTextureRV = nullptr;	// �ؽ�ó ���ҽ� ��.
	ID3D11ShaderResourceView* m_pNormalRV = nullptr;	// �븻�� ���ҽ� ��.
	ID3D11SamplerState* m_pSamplerLinear = nullptr;		// ���÷� ����.

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
	Vector3 m_CameraPos = Vector3(0.0f, 0.0f, -200.0f);

	CB_Transform m_Transform;
	CB_Marterial m_Material;
	CB_DirectionLight m_Light;
	float m_MeshScale=50.0f;

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

