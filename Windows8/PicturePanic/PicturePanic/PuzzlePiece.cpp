#include "pch.h"

#include "PuzzlePiece.h"
#include "Utility/BasicLoader.h"
#include "Utility/Utility.h"
#include "Utility/Easing.h"
#include "Math/Geometry.h"
#include "Resources/ResourceFactory.h"
#include "PicturePanic.h"
#include "Globals/GameGlobals.h"

using namespace GameFramework::Utility;
using namespace GameFramework::Math;
using namespace GameFramework::Resources;
using namespace GameFramework::Globals;

// init one time data
Texture2DResource^ PuzzlePiece::_texturePuzzlePiece				   = nullptr;
Texture2DResource^ PuzzlePiece::_textureCustomColorBack			   = nullptr;
Texture2DResource^ PuzzlePiece::_textureCustomColorSides		   = nullptr;
ID3D11RasterizerState* PuzzlePiece::_pRasterizerStateWireframe	   = nullptr;
ID3D11RasterizerState* PuzzlePiece::_pRasterizerStateBackfaceCull  = nullptr;
ID3D11Buffer* PuzzlePiece::_pConstantBufferPuzzlePiece			   = nullptr;
ID3D11Buffer* PuzzlePiece::_pMatrixBuffer						   = nullptr;
ID3D11Buffer* PuzzlePiece::_pLightBuffer						   = nullptr;
ID3D11Buffer* PuzzlePiece::_pCameraBuffer						   = nullptr;
ID3D11Buffer* PuzzlePiece::_pVertexBufferPieceBack				   = nullptr;  
ID3D11Buffer* PuzzlePiece::_pVertexBufferPieceSides				   = nullptr;  
ID3D11Buffer* PuzzlePiece::_pIndexBufferPieceFrontAndBack		   = nullptr;
ID3D11Buffer* PuzzlePiece::_pIndexBufferPieceSides				   = nullptr;
BoundingBox* PuzzlePiece::_pBoundingBox						       = nullptr;
ID3D11PixelShader* PuzzlePiece::_pPixelShaderBoundingBox		   = nullptr;
SpecularMapVertexType* PuzzlePiece::_frontVertexData			   = nullptr;
SpecularMapVertexType* PuzzlePiece::_customBackVertexData		   = nullptr;
ID3D11Buffer* PuzzlePiece::_pVertexBufferBoundingBox			   = nullptr;


      
ID3D11DepthStencilView*	PuzzlePiece::_pDepthStencilView				  = nullptr;
ID3D11DepthStencilState* PuzzlePiece::_pDepthStencilState			  = nullptr;
ID3D11Buffer* PuzzlePiece::_pConstantBufferGlowSettings				  = nullptr;
ID3D11Buffer* PuzzlePiece::_pConstantBufferGlowMatrixAndTime		  = nullptr; 
ID3D11PixelShader* PuzzlePiece::_pPixelShaderGlow_Original			  = nullptr;
ID3D11PixelShader* PuzzlePiece::_pPixelShaderGlow_Glow				  = nullptr;
ID3D11PixelShader* PuzzlePiece::_pPixelShaderGlow_BlurHorizontal	  = nullptr;
ID3D11PixelShader* PuzzlePiece::_pPixelShaderGlow_BlurVertical		  = nullptr;
ID3D11VertexShader* PuzzlePiece::_pVertexShaderGlow_Quad			  = nullptr;
ID3D11VertexShader* PuzzlePiece::_pVertexShaderGlow_QuadHorizontal	  = nullptr;
ID3D11VertexShader* PuzzlePiece::_pVertexShaderGlow_QuadVertical	  = nullptr;
ID3D11InputLayout* PuzzlePiece::_pInputLayoutGlow					  = nullptr;
ID3D11InputLayout* PuzzlePiece::_pInputLayoutBlurHorizontal			  = nullptr;
ID3D11InputLayout* PuzzlePiece::_pInputLayoutBlurVertical			  = nullptr;

BumpyGlossyMetalEffect^	PuzzlePiece::_effect						  = ref new BumpyGlossyMetalEffect();
PostCoronaEffect^ PuzzlePiece::_effectSelected						  = nullptr;

Texture2DResource^ PuzzlePiece::_textureColorBack					  = nullptr;
Texture2DResource^ PuzzlePiece::_textureNormalMapBack				  = nullptr;
Texture2DResource^ PuzzlePiece::_textureGlossBack					  = nullptr;
Texture2DResource^ PuzzlePiece::_textureEnvironmentMap				  = nullptr;

const float	PuzzlePiece::PIECE_WIDTH_NORMALIZED		= 1.0f;
const float	PuzzlePiece::PIECE_HEIGHT_NORMALIZED	= 1.0f;
const float	PuzzlePiece::PIECE_DEPTH_NORMALIZED		= 0.05f;
//const float	PuzzlePiece::PIECE_DEPTH_NORMALIZED		= 1.0f;

const float PuzzlePiece::REVEAL_SPIN_TIME = 1.0f;
const float PuzzlePiece::SPIN_TIME = 0.25f;
const float PuzzlePiece::SCALE_TIME = 0.25f;
const float PuzzlePiece::SELECTED_SCALEFACTOR = 0.95f;

PuzzlePiece::PuzzlePiece(float xScale, float yScale, float zScale) : _isAnimating(false), _state(STATE::NORMAL), _isSelected(false), _selectedFirstTime(false), 
																	 _scaleTargetDelta(0), _scaleToTime(-1), _scaleCurrent(1.0f), _pVertexBufferPieceCustomBack(nullptr)
																	 
{
	_scale.x = xScale;
	_scale.y = yScale;
	_scale.z = zScale;

	// initialize matricies
	_matScale		= XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	_matTranslation = XMMatrixIdentity();
	_matRotation	= XMMatrixRotationX(XMConvertToRadians(180.0f));

	UpdateWorldMatrix();

	Reset();
}

void PuzzlePiece::InitializePiece(ID3D11Device1* pDevice)
{
	memcpy(_verticesFront, PuzzlePiece::_frontVertexData, sizeof(SpecularMapVertexType) * 4);

	CreateVertexBuffer(pDevice, 4, _verticesFront, &_pVertexBufferPieceFront);
}


void PuzzlePiece::InitializeCustomBackPiece(_In_ ID3D11Device1* pDevice)
{
	memcpy(_verticesCustomBack, PuzzlePiece::_customBackVertexData, sizeof(SpecularMapVertexType) * 4);

	CreateVertexBuffer(pDevice, 4, _verticesCustomBack, &_pVertexBufferPieceCustomBack);
}


void PuzzlePiece::Reset()
{
	/*if (_vecFlipData.size())
	{
		FlipValues* pFlipValues = _vecFlipData[0];
		
		while(pFlipValues)
		{
			delete pFlipValues;
			pFlipValues = nullptr;

			_vecFlipData.pop_back();

			if (_vecFlipData.size())
			{
				pFlipValues = _vecFlipData[0];
			}
		}
	}*/

	//_lastRotation = 0.0f;

	if (_isSelected)
	{
		UnSelect();
	}

	_state = NORMAL;
	_isAnimating = false;	
}

void PuzzlePiece::Initialize(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, D3D_FEATURE_LEVEL featureLevel)
{
	CreateBuffers(pDevice);

	CreateTextures();

	CreateRasterizerStates(pDevice);

	_effect->Initialize(pDevice);
	_effect->EnvironmentMap = _textureEnvironmentMap;

	CreateResources(static_cast<int>(GameGlobals::WindowWidth), static_cast<int>(GameGlobals::WindowHeight));
}


void PuzzlePiece::CreateResources(_In_ int width, _In_ int height)
{
	ID3D11Device1* pDevice = GameGlobals::D3DDevice;

	// Setup the render target texture description.
	int minDimension = static_cast<int>(PicturePanic::Instance->RenderWindowSize);

	_effectSelected = ref new PostCoronaEffect(minDimension, minDimension);

	_effectSelected->Initialize(pDevice);

	// Create a descriptor for the depth/stencil buffer.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, static_cast<UINT>(minDimension), static_cast<UINT>(minDimension), 1, 1, D3D11_BIND_DEPTH_STENCIL);

	// Allocate a 2-D surface as the depth/stencil buffer.
	ComPtr<ID3D11Texture2D> depthStencil;
	DXCall(pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil));
		
	// Create a DepthStencil view on this surface to use on bind.
	DXCall(pDevice->CreateDepthStencilView(depthStencil.Get(), &CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D), &_pDepthStencilView));
}


void PuzzlePiece::OnWindowSizeChange(ID3D11Device1* pDevice, ID3D11DeviceContext1* pContext, float windowWidth, float windowHeight)
{
	bool needTextures = true;

	if (_effectSelected != nullptr)
	{
		if (_effectSelected->RenderTargetHeight == static_cast<int>(PicturePanic::Instance->RenderWindowSize))
		{
			needTextures = false;
		}
		else
		{
			_effectSelected->Release();
			_effectSelected = nullptr;

			_pDepthStencilView->Release();
		}
	}

	if (needTextures)
	{
		CreateResources(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
	}
}

void PuzzlePiece::CreateBuffers(ID3D11Device1* pDevice)
{
	// create the vertex and index buffers for drawing the puzzle piece
    BasicShapes^ shapes = ref new BasicShapes(pDevice);

	_pBoundingBox = new BoundingBox();

	shapes->CreateSegmentedBox(float3(PIECE_WIDTH_NORMALIZED, PIECE_HEIGHT_NORMALIZED, PIECE_DEPTH_NORMALIZED),
							   /*&_pVertexBufferPieceFront,*/
							   /*&_pVertexBufferPieceBack,*/
							   &_pIndexBufferPieceFrontAndBack,
							   &_pVertexBufferPieceSides,
							   &_pIndexBufferPieceSides,
							   _pBoundingBox);
	CreateFrontFaceData();
	CreateBackFace(pDevice);
	CreateSideFaces(pDevice);
		              
	

	// create vertex and index buffers for drawing bounding box
	XMFLOAT3 corners[8];

	_pBoundingBox->GetCorners(corners);

	BasicVertex boxVertices[8];

	for (int i = 0; i < 8; ++i)
	{
		boxVertices[i].pos = float3(corners[i].x, corners[i].y, corners[i].z);

		boxVertices[i].norm = float3(0, 1, 0);
		boxVertices[i].tex = float2(0, 0);
	}

	D3D11_BUFFER_DESC boxVertexBufferDesc;

    boxVertexBufferDesc.ByteWidth			 = sizeof(BasicVertex) * 8;
    boxVertexBufferDesc.Usage				 = D3D11_USAGE_DEFAULT;
    boxVertexBufferDesc.BindFlags			 = D3D11_BIND_VERTEX_BUFFER;
    boxVertexBufferDesc.CPUAccessFlags		 = 0;
    boxVertexBufferDesc.MiscFlags			 = 0;
    boxVertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA boxVertexBufferData;

    boxVertexBufferData.pSysMem		  = boxVertices;
    boxVertexBufferData.SysMemPitch	  = 0;
    boxVertexBufferData.SysMemSlicePitch = 0;

	DXCall(pDevice->CreateBuffer(&boxVertexBufferDesc, &boxVertexBufferData, &_pVertexBufferBoundingBox));
}


void PuzzlePiece::CreateFrontFaceData()
{
	// create vertices for specular map front face
	
	float3 r = float3(PIECE_WIDTH_NORMALIZED, PIECE_HEIGHT_NORMALIZED, PIECE_DEPTH_NORMALIZED);
	
	BasicVertex frontBasicVertices[] =
    {
		{ float3(-r.x,  r.y,  -r.z), float3( 0.0f,  0.0f,  -1.0f), float2(0.0f, 0.0f) }, // -Z (front face)
        { float3( r.x,  r.y,  -r.z), float3( 0.0f,  0.0f,  -1.0f), float2(1.0f, 0.0f) },
        { float3( r.x, -r.y,  -r.z), float3( 0.0f,  0.0f,  -1.0f), float2(1.0f, 1.0f) },
        { float3(-r.x, -r.y,  -r.z), float3( 0.0f,  0.0f,  -1.0f), float2(0.0f, 1.0f) }        
    };

	float3 tangent;
	float3 binormal;
	// Calculate the tangent and binormal of that face.
	CalculateTangentBinormal(frontBasicVertices[0], frontBasicVertices[1], frontBasicVertices[2], tangent, binormal);

	float3 normal;
	// Calculate the new normal using the tangent and binormal.
	CalculateNormal(tangent, binormal, normal);


	//XMFLOAT3 position;
	//XMFLOAT2 texture;
	//XMFLOAT3 normal;
	//XMFLOAT3 tangent;
	//XMFLOAT3 binormal;

		
	SpecularMapVertexType frontVertices[] = 
	{
		{ XMFLOAT3(-r.x,  r.y, -r.z), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(tangent.x, tangent.y, tangent.z), XMFLOAT3(binormal.x, binormal.y, binormal.z), XMFLOAT2(0.0f, 0.0f)  },
		{ XMFLOAT3( r.x,  r.y, -r.z), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(tangent.x, tangent.y, tangent.z), XMFLOAT3(binormal.x, binormal.y, binormal.z), XMFLOAT2(1.0f, 0.0f)  },
		{ XMFLOAT3( r.x, -r.y, -r.z), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(tangent.x, tangent.y, tangent.z), XMFLOAT3(binormal.x, binormal.y, binormal.z), XMFLOAT2(1.0f, 1.0f)  },
		{ XMFLOAT3(-r.x, -r.y, -r.z), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(tangent.x, tangent.y, tangent.z), XMFLOAT3(binormal.x, binormal.y, binormal.z), XMFLOAT2(0.0f, 1.0f)  }
	};

	_frontVertexData = new SpecularMapVertexType[4];

	memcpy(_frontVertexData, frontVertices, sizeof(SpecularMapVertexType) * ARRAYSIZE(frontVertices));
}


void PuzzlePiece::CreateBackFace(ID3D11Device1* pDevice)
{
	// create vertices for specular map back face
	
	float3 r = float3(PIECE_WIDTH_NORMALIZED, PIECE_HEIGHT_NORMALIZED, PIECE_DEPTH_NORMALIZED);
	
	BasicVertex backBasicVertices[] =
    {
		{ float3( r.x,  r.y, r.z), float3( 0.0f,  0.0f, 1.0f), float2(0.0f, 0.0f) }, // +Z (back face)
        { float3(-r.x,  r.y, r.z), float3( 0.0f,  0.0f, 1.0f), float2(1.0f, 0.0f) },
        { float3(-r.x, -r.y, r.z), float3( 0.0f,  0.0f, 1.0f), float2(1.0f, 1.0f) },
        { float3( r.x, -r.y, r.z), float3( 0.0f,  0.0f, 1.0f), float2(0.0f, 1.0f) }
    };

	float3 tangent;
	float3 binormal;
	// Calculate the tangent and binormal of that face.
	CalculateTangentBinormal(backBasicVertices[0], backBasicVertices[1], backBasicVertices[2], tangent, binormal);

	float3 normal;
	// Calculate the new normal using the tangent and binormal.
	CalculateNormal(tangent, binormal, normal);


	//XMFLOAT3 position;
	//XMFLOAT2 texture;
	//XMFLOAT3 normal;
	//XMFLOAT3 tangent;
	//XMFLOAT3 binormal;
		
	SpecularMapVertexType backVertices[] =
	{
		{ XMFLOAT3( r.x,  r.y, r.z), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(tangent.x, tangent.y, tangent.z), XMFLOAT3(binormal.x, binormal.y, binormal.z), XMFLOAT2(0.0f, 0.0f)  },
		{ XMFLOAT3(-r.x,  r.y, r.z), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(tangent.x, tangent.y, tangent.z), XMFLOAT3(binormal.x, binormal.y, binormal.z), XMFLOAT2(1.0f, 0.0f)  },
		{ XMFLOAT3(-r.x, -r.y, r.z), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(tangent.x, tangent.y, tangent.z), XMFLOAT3(binormal.x, binormal.y, binormal.z), XMFLOAT2(1.0f, 1.0f)  },
		{ XMFLOAT3( r.x, -r.y, r.z), XMFLOAT3(normal.x, normal.y, normal.z), XMFLOAT3(tangent.x, tangent.y, tangent.z), XMFLOAT3(binormal.x, binormal.y, binormal.z), XMFLOAT2(0.0f, 1.0f)  }
	};

	CreateVertexBuffer(pDevice, ARRAYSIZE(backVertices), backVertices, &_pVertexBufferPieceBack);

	_customBackVertexData = new SpecularMapVertexType[4];

	memcpy(_customBackVertexData, backVertices, sizeof(SpecularMapVertexType) * ARRAYSIZE(backVertices));
}


void PuzzlePiece::CreateSideFaces(ID3D11Device1* pDevice)
{
	// create vertices for specular map side faces
	
	float3 r = float3(PIECE_WIDTH_NORMALIZED, PIECE_HEIGHT_NORMALIZED, PIECE_DEPTH_NORMALIZED);
	
	BasicVertex sideBasicVertices[] =
	{
		{ float3(-r.x,  r.y,  r.z), float3( 0.0f,  1.0f,  0.0f), float2(0.0f, 0.0f) }, // +Y (top face)
        { float3( r.x,  r.y,  r.z), float3( 0.0f,  1.0f,  0.0f), float2(1.0f, 0.0f) },
        { float3( r.x,  r.y, -r.z), float3( 0.0f,  1.0f,  0.0f), float2(1.0f, 1.0f) },
        { float3(-r.x,  r.y, -r.z), float3( 0.0f,  1.0f,  0.0f), float2(0.0f, 1.0f) },

        { float3( r.x, -r.y,  r.z), float3( 0.0f, -1.0f,  0.0f), float2(0.0f, 0.0f) }, // -Y (bottom face)
        { float3(-r.x, -r.y,  r.z), float3( 0.0f, -1.0f,  0.0f), float2(1.0f, 0.0f) },
        { float3(-r.x, -r.y, -r.z), float3( 0.0f, -1.0f,  0.0f), float2(1.0f, 1.0f) },
        { float3( r.x, -r.y, -r.z), float3( 0.0f, -1.0f,  0.0f), float2(0.0f, 1.0f) },

        { float3( r.x,  r.y, -r.z), float3( 1.0f,  0.0f,  0.0f), float2(0.0f, 0.0f) }, // +X (right face)
        { float3( r.x,  r.y,  r.z), float3( 1.0f,  0.0f,  0.0f), float2(1.0f, 0.0f) },
        { float3( r.x, -r.y,  r.z), float3( 1.0f,  0.0f,  0.0f), float2(1.0f, 1.0f) },
        { float3( r.x, -r.y, -r.z), float3( 1.0f,  0.0f,  0.0f), float2(0.0f, 1.0f) },

        { float3(-r.x,  r.y,  r.z), float3(-1.0f,  0.0f,  0.0f), float2(0.0f, 0.0f) }, // -X (left face)
        { float3(-r.x, -r.y,  r.z), float3(-1.0f,  0.0f,  0.0f), float2(1.0f, 0.0f) },
        { float3(-r.x, -r.y, -r.z), float3(-1.0f,  0.0f,  0.0f), float2(1.0f, 1.0f) },
        { float3(-r.x,  r.y, -r.z), float3(-1.0f,  0.0f,  0.0f), float2(0.0f, 1.0f) }
	};


	SpecularMapVertexType sideVertices[16];


	int currentVertex = 0;
	float3 tangent;
	float3 binormal;
	float3 normal;
	
	for (int i = 0; i < 4; ++i)
	{
		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(sideBasicVertices[currentVertex], sideBasicVertices[currentVertex + 1], sideBasicVertices[currentVertex + 2], tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		SetupSideFace(&sideVertices[currentVertex], &sideBasicVertices[currentVertex], normal, tangent, binormal);

		// next face (4 vertices per face)
		currentVertex += 4;
	}

	CreateVertexBuffer(pDevice, ARRAYSIZE(sideVertices), sideVertices, &_pVertexBufferPieceSides);
	
	unsigned short sidesIndices[] = 
    {
        0,1,2,
        2,3,0,

        4,5,6,
        6,7,4,

        8,9,10,
        10,11,8,

        12,13,14,
        14,15,12
	};

	CreateIndexBuffer(pDevice, ARRAYSIZE(sidesIndices), sidesIndices, &_pIndexBufferPieceSides);
}

void PuzzlePiece::SetupSideFace(SpecularMapVertexType* pFace, BasicVertex *pBasicFace, float3 normal, float3 tangent, float3 binormal)
{
	pFace[0].position = XMFLOAT3(pBasicFace[0].pos.x, pBasicFace[0].pos.y, pBasicFace[0].pos.z);
	pFace[1].position = XMFLOAT3(pBasicFace[1].pos.x, pBasicFace[1].pos.y, pBasicFace[1].pos.z);
	pFace[2].position = XMFLOAT3(pBasicFace[2].pos.x, pBasicFace[2].pos.y, pBasicFace[2].pos.z);
	pFace[3].position = XMFLOAT3(pBasicFace[3].pos.x, pBasicFace[3].pos.y, pBasicFace[3].pos.z);	
	
	pFace[0].normal = XMFLOAT3(normal.x, normal.y, normal.z);
	pFace[1].normal = XMFLOAT3(normal.x, normal.y, normal.z);
	pFace[2].normal = XMFLOAT3(normal.x, normal.y, normal.z);
	pFace[3].normal = XMFLOAT3(normal.x, normal.y, normal.z);

	pFace[0].binormal = XMFLOAT3(binormal.x, binormal.y, binormal.z);
	pFace[1].binormal = XMFLOAT3(binormal.x, binormal.y, binormal.z);
	pFace[2].binormal = XMFLOAT3(binormal.x, binormal.y, binormal.z);
	pFace[3].binormal = XMFLOAT3(binormal.x, binormal.y, binormal.z);

	pFace[0].tangent = XMFLOAT3(tangent.x, tangent.y, tangent.z);
	pFace[1].tangent = XMFLOAT3(tangent.x, tangent.y, tangent.z);
	pFace[2].tangent = XMFLOAT3(tangent.x, tangent.y, tangent.z);
	pFace[3].tangent = XMFLOAT3(tangent.x, tangent.y, tangent.z);

	pFace[0].texture = XMFLOAT2(pBasicFace[0].tex.u, pBasicFace[0].tex.v);
	pFace[1].texture = XMFLOAT2(pBasicFace[1].tex.u, pBasicFace[1].tex.v);
	pFace[2].texture = XMFLOAT2(pBasicFace[2].tex.u, pBasicFace[2].tex.v);
	pFace[3].texture = XMFLOAT2(pBasicFace[3].tex.u, pBasicFace[3].tex.v);
}

void PuzzlePiece::CreateVertexBuffer(_In_ ID3D11Device1* pDevice, _In_ unsigned int numVertices, _In_ SpecularMapVertexType *pVertexData, _Out_ ID3D11Buffer **ppVertexBuffer)
{
	*ppVertexBuffer = nullptr;

    ComPtr<ID3D11Buffer> vertexBufferInternal;

    D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.ByteWidth			 = sizeof(SpecularMapVertexType) * numVertices;
	vertexBufferDesc.Usage				 = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags			 = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags		 = 0;
    vertexBufferDesc.MiscFlags			 = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

    vertexBufferData.pSysMem		  = pVertexData;
    vertexBufferData.SysMemPitch	  = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    DXCall(pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBufferInternal));


    *ppVertexBuffer = vertexBufferInternal.Detach();
}

void PuzzlePiece::CreateIndexBuffer(_In_ ID3D11Device1* pDevice, _In_ unsigned int numIndices, _In_ unsigned short *pIndexData, _Out_ ID3D11Buffer **ppIndexBuffer)
{
	*ppIndexBuffer = nullptr;

    ComPtr<ID3D11Buffer> indexBufferInternal;

    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.ByteWidth			= sizeof(unsigned short) * numIndices;
    indexBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags		= 0;
    indexBufferDesc.MiscFlags			= 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(indexBufferData));

    indexBufferData.pSysMem			 = pIndexData;
    indexBufferData.SysMemPitch		 = 0;
    indexBufferData.SysMemSlicePitch = 0;

    DXCall(pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBufferInternal));

    *ppIndexBuffer = indexBufferInternal.Detach();
}


void PuzzlePiece::CreateConstantBuffers(ID3D11Device1* pDevice)
{
	// create the constant buffer for updating model and camera data
    CD3D11_BUFFER_DESC constantBufferDesc(ConstantBufferSize(sizeof(ShaderConstantBuffer)), D3D11_BIND_CONSTANT_BUFFER);

    DXCall(pDevice->CreateBuffer(&constantBufferDesc, nullptr, &_pConstantBufferPuzzlePiece));

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage				 = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth			 = ConstantBufferSize(sizeof(MatrixBufferType));
	matrixBufferDesc.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags		 = 0;
	matrixBufferDesc.MiscFlags			 = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	DXCall(pDevice->CreateBuffer(&matrixBufferDesc, NULL, &_pMatrixBuffer));

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	lightBufferDesc.ByteWidth			= ConstantBufferSize(sizeof(LightBufferType));
	lightBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags		= 0;
	lightBufferDesc.MiscFlags			= 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	DXCall(pDevice->CreateBuffer(&lightBufferDesc, NULL, &_pLightBuffer));
	
	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage				 = D3D11_USAGE_DEFAULT;
	cameraBufferDesc.ByteWidth			 = ConstantBufferSize(sizeof(CameraBufferType));
	cameraBufferDesc.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags		 = 0;
	cameraBufferDesc.MiscFlags			 = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	DXCall(pDevice->CreateBuffer(&cameraBufferDesc, NULL, &_pCameraBuffer));



	// create constant buffers for glow effect
	
	D3D11_BUFFER_DESC glowSettingsDesc;
	glowSettingsDesc.Usage				 = D3D11_USAGE_DEFAULT;
	glowSettingsDesc.ByteWidth			 = ConstantBufferSize(sizeof(GlowSettingsType));
	glowSettingsDesc.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER;
	glowSettingsDesc.CPUAccessFlags		 = 0;
	glowSettingsDesc.MiscFlags			 = 0;
	glowSettingsDesc.StructureByteStride = 0;

	DXCall(pDevice->CreateBuffer(&glowSettingsDesc, NULL, &_pConstantBufferGlowSettings));

	D3D11_BUFFER_DESC glowMatrixAndTimeDesc;
	glowMatrixAndTimeDesc.Usage				  = D3D11_USAGE_DEFAULT;
	glowMatrixAndTimeDesc.ByteWidth			  = ConstantBufferSize(sizeof(GlowMatrixAndTimeType));
	glowMatrixAndTimeDesc.BindFlags			  = D3D11_BIND_CONSTANT_BUFFER;
	glowMatrixAndTimeDesc.CPUAccessFlags	  = 0;
	glowMatrixAndTimeDesc.MiscFlags			  = 0;
	glowMatrixAndTimeDesc.StructureByteStride = 0;

	DXCall(pDevice->CreateBuffer(&glowMatrixAndTimeDesc, NULL, &_pConstantBufferGlowMatrixAndTime));
}


void PuzzlePiece::CreateTextures()
{		
	// Textures
	_textureColorBack      = ResourceFactory::CreateTexture2DResource(L"Assets/InGame/colormap_512x512.dds");
	_textureNormalMapBack  = ResourceFactory::CreateTexture2DResource(L"Assets/InGame/normalmap_512x512.dds");
	_textureGlossBack      = ResourceFactory::CreateTexture2DResource(L"Assets/InGame/glossmap_512x512.dds");
	_textureEnvironmentMap = ResourceFactory::CreateTexture2DResource(L"Assets/InGame/environmentmap_512x512.dds");
}

void PuzzlePiece::CreateRasterizerStates(ID3D11Device1* pDevice)
{
	D3D11_RASTERIZER_DESC descBackfaceCull;

    descBackfaceCull.AntialiasedLineEnable = FALSE;
    descBackfaceCull.CullMode			   = D3D11_CULL_BACK;
    descBackfaceCull.DepthBias			   = 0;
    descBackfaceCull.DepthBiasClamp		   = 0.0f;
    descBackfaceCull.DepthClipEnable	   = TRUE;
    descBackfaceCull.FillMode			   = D3D11_FILL_SOLID;
    descBackfaceCull.FrontCounterClockwise = false;
    descBackfaceCull.MultisampleEnable     = true;
    descBackfaceCull.ScissorEnable		   = false;
    descBackfaceCull.SlopeScaledDepthBias  = 0;
	
	DXCall(pDevice->CreateRasterizerState(&descBackfaceCull, &_pRasterizerStateBackfaceCull));

	// for rendering bounding box

	D3D11_RASTERIZER_DESC descWireframe;

    descWireframe.AntialiasedLineEnable = FALSE;
    descWireframe.CullMode			    = D3D11_CULL_NONE;
    descWireframe.DepthBias			    = 0;
    descWireframe.DepthBiasClamp		= 0.0f;
    descWireframe.DepthClipEnable	    = TRUE;
    descWireframe.FillMode			    = D3D11_FILL_WIREFRAME;
    descWireframe.FrontCounterClockwise = false;
    descWireframe.MultisampleEnable	    = true;
    descWireframe.ScissorEnable		    = false;
    descWireframe.SlopeScaledDepthBias  = 0;

	DXCall(pDevice->CreateRasterizerState(&descWireframe, &_pRasterizerStateWireframe));
}

void PuzzlePiece::LoadNewPuzzle(Platform::String^ modePath, int puzzleID)
{
	if (_texturePuzzlePiece)
	{
		_texturePuzzlePiece->Release();
		_texturePuzzlePiece = nullptr;
	}

	_texturePuzzlePiece = ResourceFactory::CreateTexture2DResource(L"Assets\\Puzzles" + modePath + puzzleID.ToString() + L".jpg");
}


void PuzzlePiece::SetLocation(float x, float y, float z)
{
	_vLocation.x = x;
	_vLocation.y = y;
	_vLocation.z = z;

	_matTranslation = XMMatrixTranslation(x, y, z);

	UpdateWorldMatrix();
}

void PuzzlePiece::SetCorrectUVs(ID3D11DeviceContext1 *pContext, float uMin, float vMin, float uMax, float vMax)
{
	_correctUMin = uMin;
	_correctUMax = uMax;

	_correctVMin = vMin;
	_correctVMax = vMax;

	_currentUMin = _correctUMin;
	_currentUMax = _correctUMax;
	_currentVMin = _correctVMin;
	_currentVMax = _correctVMax;

	UpdateUVs(pContext);
}

void PuzzlePiece::SetUVs(ID3D11DeviceContext1 *pContext, float uMin, float vMin, float uMax, float vMax)
{
	_currentUMin = uMin;
	_currentUMax = uMax;
	_currentVMin = vMin;
	_currentVMax = vMax;

	UpdateUVs(pContext);
}

void PuzzlePiece::ResetUVs(ID3D11DeviceContext1* pContext)
{
	_currentUMin = _correctUMin;
	_currentUMax = _correctUMax;
	_currentVMin = _correctVMin;
	_currentVMax = _correctVMax;

	UpdateUVs(pContext);
}

void PuzzlePiece::FlipAndSwapWithPiece(ID3D11DeviceContext1* pContext, PuzzlePiece^ swapPiece)
{
	FlipAndSwap(pContext, 0.0f, swapPiece);
	swapPiece->HideDuringGame();
}
	
void PuzzlePiece::UpdateUVs(ID3D11DeviceContext1* pContext)
{
	_verticesFront[0].texture.x = _currentUMin;
	_verticesFront[0].texture.y = _currentVMin;
	_verticesFront[1].texture.x = _currentUMax;
	_verticesFront[1].texture.y = _currentVMin;
	_verticesFront[2].texture.x = _currentUMax;
	_verticesFront[2].texture.y = _currentVMax;
	_verticesFront[3].texture.x = _currentUMin;
	_verticesFront[3].texture.y = _currentVMax;

	pContext->UpdateSubresource(_pVertexBufferPieceFront, 0, NULL, &_verticesFront, 0, 0);

	if (_pVertexBufferPieceCustomBack != nullptr)
	{
		_verticesCustomBack[0].texture.x = _currentUMax;
		_verticesCustomBack[0].texture.y = _currentVMax;
		_verticesCustomBack[1].texture.x = _currentUMin;
		_verticesCustomBack[1].texture.y = _currentVMax;
		_verticesCustomBack[2].texture.x = _currentUMin;
		_verticesCustomBack[2].texture.y = _currentVMin;
		_verticesCustomBack[3].texture.x = _currentUMax;
		_verticesCustomBack[3].texture.y = _currentVMin;

		pContext->UpdateSubresource(_pVertexBufferPieceCustomBack, 0, NULL, &_verticesCustomBack, 0, 0);
	}

	// set the UVs for the front face
	/*D3D11_MAPPED_SUBRESOURCE mappedResource;

	DXCall(pContext->Map(_pVertexBufferPieceFront, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	BasicVertex* pVertices = reinterpret_cast<BasicVertex*>(mappedResource.pData);

	pVertices[0].tex.u = _currentUMin;
	pVertices[0].tex.v = _currentVMin;
	pVertices[1].tex.u = _currentUMax;
	pVertices[1].tex.v = _currentVMin;
	pVertices[2].tex.u = _currentUMax;
	pVertices[2].tex.v = _currentVMax;
	pVertices[3].tex.u = _currentUMin;
	pVertices[3].tex.v = _currentVMax;

	//mappedSubResource.pData = _vertexData;

	pContext->Unmap(_pVertexBufferPieceFront, 0);
	*/
	
}
	

void PuzzlePiece::UpdateWorldMatrix()
{
	_matScale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);

	XMMATRIX mat = XMMatrixScaling(_scaleCurrent, _scaleCurrent, _scaleCurrent);

	//XMMATRIX matOffset = XMMatrixTranslation(-_vLocation.x, -_vLocation.y, -_vLocation.z);
	_matWorld = mat * _matRotation * _matTranslation * _matScale;
}

bool PuzzlePiece::HitTest( _In_ FXMVECTOR vOrigin, _In_ FXMVECTOR vDirection)
{
	if (_isAnimating)
	{
		return false;
	}

	// if correct don't hit test - piece is locked in place
	if (IsCorrect())
	{
		return false;
	}

	BoundingBox transformedBB;

	PuzzlePiece::_pBoundingBox->Transform(transformedBB, _matWorld);

	float intersectionDistance;

	return transformedBB.Intersects(vOrigin, vDirection, intersectionDistance);
}

void PuzzlePiece::Update(float timeTotal, float timeDelta, PerspectiveCamera *pCamera)
{
	bool stateComplete = false;

	if (_vecFlipData.size())
	{
		FlipValues* pFlipValues = _vecFlipData[0];

		if (pFlipValues->delay > 0)
		{
			pFlipValues->delay -= timeDelta;

			return;
		}

		float normalizedSpinTime = pFlipValues->currentSpinTime / pFlipValues->totalSpinTime;

		pFlipValues->currentSpinTime += timeDelta;

		float adjusted;

		_state = pFlipValues->state;

		switch (_state)
		{
			case STATE::SHOW:
			case STATE::HIDE:
				adjusted = BackEaseOut(normalizedSpinTime,  0.7f);
					
				if (pFlipValues->currentSpinTime >= pFlipValues->totalSpinTime)
				{
					stateComplete = true;

					_matRotation = XMMatrixRotationX(pFlipValues->lastRotation + (pFlipValues->targetRotationInRadians));
				}
				else
				{
					_matRotation = XMMatrixRotationX(pFlipValues->lastRotation + (pFlipValues->targetRotationInRadians * adjusted));
				}
			
				break;
	
		case STATE::FLIPANDSWAP:
				adjusted = QuadraticEaseOut(normalizedSpinTime);

				_matRotation = XMMatrixRotationX(pFlipValues->lastRotation + (pFlipValues->targetRotationInRadians * adjusted));

				if (pFlipValues->currentSpinTime >= pFlipValues->totalSpinTime)
				{
					SwapWith(pFlipValues->pContext, pFlipValues->swapTargetPiece);

					pFlipValues->swapTargetPiece->ShowDuringGame(0.2f);

					ShowDuringGame(0.0f);			

					stateComplete = true;
				}
		
				break;

		case STATE::HIDE_DURING_GAME:
		case STATE::SHOW_DURING_GAME:
				adjusted = QuadraticEaseOut(normalizedSpinTime);
				
				if (pFlipValues->currentSpinTime >= pFlipValues->totalSpinTime)
				{
					stateComplete = true;

					_matRotation = XMMatrixRotationX(pFlipValues->lastRotation + (pFlipValues->targetRotationInRadians));
				}
				else
				{
					_matRotation = XMMatrixRotationX(pFlipValues->lastRotation + (pFlipValues->targetRotationInRadians * adjusted));
				}
			
				break;
		}
	}

	if (stateComplete)
	{
		_vecFlipData.erase(_vecFlipData.begin());

		if (_vecFlipData.size() == 0)
		{
			_state = STATE::NORMAL;

			_isAnimating = false;
		}
	}

	if (_scaleToTime >= 0.0f)
	{
		_scaleToTime += timeDelta;

		float normalizedTime = _scaleToTime / SCALE_TIME;

		float factor = BackEaseOut(normalizedTime, 0.7f);

		_scaleCurrent = _scaleStart + (factor * _scaleTargetDelta);

		if (_scaleToTime >= SCALE_TIME)
		{
			_scaleToTime = -1.0f;

			_scaleCurrent = _scaleStart + _scaleTargetDelta;
		}
	}

	UpdateWorldMatrix();


	// update the effect matricies
	_effect->ViewMatrix		   = pCamera->ViewMatrix();
	_effect->ProjectionMatrix  = pCamera->ProjectionMatrix();

	_effect->Update(timeTotal, timeDelta);

	if (_isSelected)
	{
		_effectSelected->ViewMatrix		   = pCamera->ViewMatrix();
		_effectSelected->ProjectionMatrix  = pCamera->ProjectionMatrix();
		
		_effectSelected->Update(timeTotal, timeDelta);
	}

	UpdateShaderConstants(pCamera, timeTotal);
}



void PuzzlePiece::ScaleTo(float scaleTarget)
{
	_scaleTargetDelta = scaleTarget - _scaleCurrent;

	_scaleStart = _scaleCurrent;

	if (_scaleTargetDelta != 0.0f)
	{
		_scaleToTime = 0;
	}
}


void PuzzlePiece::UpdateShaderConstants(PerspectiveCamera* pCamera, float timeTotal)
{
	// update the constant buffer data for this piece
	// set the world matrix for this piece
	XMMATRIX matWorldTransposed		 = XMMatrixTranspose(_matWorld);
	XMMATRIX matViewTransposed		 = XMMatrixTranspose(pCamera->ViewMatrix());
	XMMATRIX matProjectionTransposed = XMMatrixTranspose(pCamera->ProjectionMatrix());

	_constantBufferData.world	   = matWorldTransposed;
	_constantBufferData.view	   = matViewTransposed;
	_constantBufferData.projection = matProjectionTransposed;

	_constantBufferData.light.direction = XMFLOAT3(0.5f, -0.5f, 0.0f);
	_constantBufferData.light.color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	_constantBufferData.material.ambient = 0.1f;
	_constantBufferData.material.diffuse = 0.5f;
	_constantBufferData.material.specular = 0.5f;
	_constantBufferData.material.shininess = 30;

	_constantBufferData.ambientLight = XMFLOAT4(1, 1, 1, 1);
	
	XMFLOAT3 eyePos = pCamera->Position();
	XMVECTOR vCamera = XMLoadFloat3(&eyePos);

	float x = XMVectorGetX(vCamera);
	float y = XMVectorGetY(vCamera);
	float z = XMVectorGetZ(vCamera);

	XMFLOAT3 cameraPos = XMFLOAT3(x, y, z);

	_constantBufferData.eyePosition = cameraPos;

	// specular map constants
	_matrixBufferData.world		 = matWorldTransposed;
	_matrixBufferData.view		 = matViewTransposed;
	_matrixBufferData.projection = matProjectionTransposed;

	_lightBufferData.diffuseColor   = float4(1.0f, 1.0f, 1.0f, 1.0f);
	_lightBufferData.lightDirection = float3(0.0f, 0.0f, 1.0f);
	_lightBufferData.specularColor  = float4(0.0f, 1.0f, 1.0f, 1.0f);
	_lightBufferData.specularPower  = 32.0f;

	_cameraBufferData.cameraPosition = cameraPos;
}

void PuzzlePiece::SwapWith(ID3D11DeviceContext1* pContext, PuzzlePiece^ swapPiece)
{
	float2 minSwapUV = swapPiece->GetMinUV();
	float2 maxSwapUV = swapPiece->GetMaxUV();

	float2 currentMinUV = GetMinUV();
	float2 currentMaxUV = GetMaxUV();

	_currentUMin = minSwapUV.u;
	_currentVMin = minSwapUV.v;
	_currentUMax = maxSwapUV.u;
	_currentVMax = maxSwapUV.v;
	
	UpdateUVs(pContext);

	swapPiece->SetUVs(pContext, currentMinUV.u, currentMinUV.v, currentMaxUV.u, currentMaxUV.v);
}


void PuzzlePiece::Show(_In_ float delay, _In_ float spinTime)
{
	FlipValues* pValues = new FlipValues();
	
	pValues->state = SHOW;
	
	pValues->delay					  = delay;
	pValues->totalSpinTime			  = spinTime;
	pValues->currentSpinTime		  = 0.0f;
	pValues->swapTargetPiece		  = nullptr;
	pValues->pContext				  = nullptr;

	pValues->targetRotationInRadians = XMConvertToRadians(180.0f);

	pValues->lastRotation = XMConvertToRadians(180.0f);

	_vecFlipData.push_back(pValues);
}


void PuzzlePiece::Hide(_In_ float delay, _In_ float spinTime)
{
	FlipValues* pValues = new FlipValues();

	pValues->state = HIDE;

	pValues->delay					  = delay;
	pValues->totalSpinTime			  = spinTime;
	pValues->currentSpinTime		  = 0.0f;
	pValues->swapTargetPiece		  = nullptr;
	pValues->pContext				  = nullptr;
	
	pValues->targetRotationInRadians = XMConvertToRadians(180.0f);

	pValues->lastRotation = 0;

	_vecFlipData.push_back(pValues);
}

void PuzzlePiece::HideDuringGame()
{
	FlipValues* pValues = new FlipValues();

	pValues->state = HIDE_DURING_GAME;

	pValues->delay					  = 0.0f;
	pValues->totalSpinTime			  = SPIN_TIME;
	pValues->currentSpinTime		  = 0.0f;
	pValues->swapTargetPiece		  = nullptr;
	pValues->pContext				  = nullptr;
	
	pValues->targetRotationInRadians = XMConvertToRadians(180.0f);

	pValues->lastRotation = 0;

	_vecFlipData.push_back(pValues);
}

void PuzzlePiece::ShowDuringGame(float delay)
{
	FlipValues* pValues = new FlipValues();

	pValues->state = SHOW_DURING_GAME;

	pValues->delay					  = delay;
	pValues->totalSpinTime			  = SPIN_TIME;
	pValues->currentSpinTime		  = 0.0f;
	pValues->swapTargetPiece		  = nullptr;
	pValues->pContext				  = nullptr;
	
	pValues->targetRotationInRadians = XMConvertToRadians(180.0f);

	pValues->lastRotation = XMConvertToRadians(180.0f);

	_vecFlipData.push_back(pValues);
}

void PuzzlePiece::FlipAndSwap(ID3D11DeviceContext1* pContext, float delay, PuzzlePiece^ swapPiece)
{
	FlipValues* pValues = new FlipValues();

	pValues->state = FLIPANDSWAP;

	pValues->delay					  = delay;
	pValues->totalSpinTime			  = SPIN_TIME;
	pValues->currentSpinTime		  = 0.0f;
	pValues->swapTargetPiece		  = swapPiece;
	pValues->pContext				  = pContext;
	
	if (_lastRotation == 0.0f)
	{
		pValues->targetRotationInRadians = XMConvertToRadians(180.0f);
		pValues->lastRotation = XMConvertToRadians(180.0f);
	}
	else
	{
		pValues->targetRotationInRadians = XMConvertToRadians(180.0f);
		pValues->lastRotation = 0.0f;
	}

	_vecFlipData.push_back(pValues);
}


bool PuzzlePiece::IsCorrect()
{
	if (_isAnimating)
	{
		return false;
	}

	if (_currentUMin == _correctUMin &&
		_currentUMax == _correctUMax &&
		_currentVMin == _correctVMin &&
		_currentVMax == _correctVMax)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void PuzzlePiece::Select()
{
	SetLocation(_vLocation.x, _vLocation.y, _vLocation.z - 1);

	_isSelected = true;

	_selectedFirstTime = true;

	//_state = STATE::SELECTED;

	//_currentSeconds = 0;
	//_isAnimating = true;

	ScaleTo(SELECTED_SCALEFACTOR);
	//_scale.z = 10.0f;
}

void PuzzlePiece::UnSelect()
{
	SetLocation(_vLocation.x, _vLocation.y, _vLocation.z + 1);

	_isSelected = false;

	//_currentSeconds = 0;
	//_isAnimating = true;

	ScaleTo(1.0f);
}

void PuzzlePiece::RotateOnce()
{
	_isAnimating = true;

	_degreesLeftToRotate = 360.0f;
}

void PuzzlePiece::Render(ID3D11DeviceContext1* pContext, float timeTotal)
{
	pContext->RSSetState(PuzzlePiece::_pRasterizerStateBackfaceCull);

	if (_isSelected)
	{
		return;
	}
	
	_effect->WorldMatrix = _matWorld;

	RenderPieceFront(pContext);
		
	_effect->Reflectiveness = 1.0f;

	_effect->ColorMap  = _textureColorBack;
	_effect->NormalMap = _textureNormalMapBack;
	_effect->GlossMap  = _textureGlossBack;
		
	RenderPieceSides(pContext);
	RenderPieceBack(pContext);
}

void PuzzlePiece::RenderSelected(ID3D11DeviceContext1* pContext)
{
	if (!_isSelected)
	{
		return;
	}

	if (_selectedFirstTime)
	{
		_effectSelected->InitialClear(pContext);
		
		_selectedFirstTime = false;
	}
		
	_effectSelected->WorldMatrix = _matWorld;

	_effectSelected->Begin(pContext);

	_effectSelected->BeginObjectPass();

	_effectSelected->VertexBuffer = _pVertexBufferPieceFront;
	_effectSelected->IndexBuffer  = _pIndexBufferPieceFrontAndBack;
	_effectSelected->VertexStride = sizeof(SpecularMapVertexType);
	_effectSelected->IndexCount   = 6;

	_effectSelected->RenderObject();

	_effectSelected->Render();

	_effectSelected->End();

	//_effectSelected->RenderDebug(pContext);

	
	_effect->WorldMatrix = _matWorld;

	RenderPieceFront(pContext);
		
	_effect->Reflectiveness = 1.0f;

	_effect->ColorMap  = _textureColorBack;
	_effect->NormalMap = _textureNormalMapBack;
	_effect->GlossMap  = _textureGlossBack;
		
	RenderPieceSides(pContext);
	RenderPieceBack(pContext);
}


void PuzzlePiece::RenderCorrect(_In_ ID3D11DeviceContext1* pContext, PostEffect^ postEffect)
{
	if (!IsCorrect())
	{
		return;
	}

	postEffect->WorldMatrix = _matWorld;

	postEffect->VertexBuffer = _pVertexBufferPieceFront;
	postEffect->IndexBuffer  = _pIndexBufferPieceFrontAndBack;
	postEffect->VertexStride = sizeof(SpecularMapVertexType);
	postEffect->IndexCount   = 6;

	postEffect->RenderObject();
}


void PuzzlePiece::RenderAsTransition(_In_ ID3D11DeviceContext1* pContext)
{
	pContext->RSSetState(PuzzlePiece::_pRasterizerStateBackfaceCull);

	_effect->WorldMatrix = _matWorld;
	_effect->FlatTextured = true;
	
	if (_texturePuzzlePiece != nullptr)
	{
		_effect->Reflectiveness = 0.0f;
		
		_effect->ColorMap  = _texturePuzzlePiece;
		_effect->GlossMap  = _textureGlossBack;

		_effect->VertexBuffer = _pVertexBufferPieceFront;
		_effect->IndexBuffer  = _pIndexBufferPieceFrontAndBack;
		_effect->VertexStride = sizeof(SpecularMapVertexType);
		_effect->IndexCount   = 6;

		_effect->Begin(pContext);

		_effect->Render();

		_effect->End();
	}
	
	_effect->ColorMap  = _textureCustomColorSides;
	_effect->NormalMap = _textureNormalMapBack;
	_effect->GlossMap  = _textureGlossBack;
		
	RenderPieceSides(pContext);
	
	_effect->ColorMap  = _textureCustomColorBack;
	_effect->VertexBuffer = _pVertexBufferPieceCustomBack;
	_effect->IndexBuffer  = _pIndexBufferPieceFrontAndBack;
	_effect->VertexStride = sizeof(SpecularMapVertexType);
	_effect->IndexCount   = 6;

	_effect->Begin(pContext);

	_effect->Render();

	_effect->End();
}


void PuzzlePiece::RenderPieceFront(ID3D11DeviceContext1 *pContext)
{
	if (_texturePuzzlePiece != nullptr)
	{
		_effect->FlatTextured = IsCorrect();
		_effect->Reflectiveness = 0.0f;

		_effect->ColorMap  = _texturePuzzlePiece;
		_effect->GlossMap  = _textureGlossBack;

		_effect->VertexBuffer = _pVertexBufferPieceFront;
		_effect->IndexBuffer  = _pIndexBufferPieceFrontAndBack;
		_effect->VertexStride = sizeof(SpecularMapVertexType);
		_effect->IndexCount   = 6;

		_effect->Begin(pContext);

		_effect->Render();

		_effect->End();

		_effect->FlatTextured = false;
	}
}

void PuzzlePiece::RenderPieceBack(ID3D11DeviceContext1 *pContext)
{
	_effect->VertexBuffer = _pVertexBufferPieceBack;
	_effect->IndexBuffer  = _pIndexBufferPieceFrontAndBack;
	_effect->VertexStride = sizeof(SpecularMapVertexType);
	_effect->IndexCount   = 6;

	_effect->Begin(pContext);

	_effect->Render();

	_effect->End();
}

void PuzzlePiece::UpdateBackShaderConstants(ID3D11DeviceContext1* pContext)
{
	// update constants
	pContext->UpdateSubresource(PuzzlePiece::_pMatrixBuffer, 0, nullptr, &_matrixBufferData, 0, 0);
	
	// lighting variables
	pContext->UpdateSubresource(PuzzlePiece::_pLightBuffer, 0, nullptr, &_lightBufferData, 0, 0);
	
	// camera variables
	pContext->UpdateSubresource(PuzzlePiece::_pCameraBuffer, 0, nullptr, &_cameraBufferData, 0, 0);
}

void PuzzlePiece::RenderPieceSides(ID3D11DeviceContext1 *pContext)
{
	if (_state == STATE::NORMAL)
	{
		return;
	}

	_effect->VertexBuffer = _pVertexBufferPieceSides;
	_effect->IndexBuffer  = _pIndexBufferPieceSides;
	_effect->VertexStride = sizeof(SpecularMapVertexType);
	_effect->IndexCount   = 24;

	_effect->Begin(pContext);

	_effect->Render();

	_effect->End();
}

void PuzzlePiece::RenderBoundingBox(ID3D11DeviceContext1* pContext)
{
	/*pContext->RSSetState(PuzzlePiece::_pRasterizerStateWireframe);

	// set the vertex and index buffers, and specify the way they define geometry
    UINT stride = sizeof(BasicVertex);
    UINT offset = 0;
    
	pContext->IASetVertexBuffers(0, 1, &(PuzzlePiece::_pVertexBufferBoundingBox), &stride, &offset);

    // set the index buffer
	pContext->IASetIndexBuffer(PuzzlePiece::_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// set the vertex shader stage state
    pContext->VSSetShader(PuzzlePiece::_pVertexShaderPuzzlePiece, nullptr, 0);

	pContext->VSSetConstantBuffers(0, 1, &(PuzzlePiece::_pConstantBufferPuzzlePiece));

	// set the pixel shader stage state
	pContext->PSSetShader(PuzzlePiece::_pPixelShaderBoundingBox, nullptr, 0);

    pContext->PSSetShaderResources(0, 1, &(PuzzlePiece::_pSRVTexturePuzzlePiece));

	pContext->PSSetSamplers(0, 1, &(PuzzlePiece::_pSampler));

	// draw the piece
	pContext->DrawIndexed(36, 0, 0);*/
}


void PuzzlePiece::Release()
{
	_pVertexBufferPieceFront->Release();
	_pVertexBufferPieceFront = nullptr;
}
