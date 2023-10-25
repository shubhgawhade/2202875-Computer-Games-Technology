#pragma once

#include "DeviceResources.h"
#include "Light.h"

//Class from which we create all shader objects used by the framework
//This single class can be expanded to accomodate shaders of all different types with different parameters
class Shader
{
public:
	Shader();
	~Shader();

	//we could extend this to load in only a vertex shader, only a pixel shader etc.  or specialised init for Geometry or domain shader. 
	//All the methods here simply create new versions corresponding to your needs
	bool InitStandard(ID3D11Device * device, WCHAR * vsFilename, WCHAR * psFilename);		//Loads the Vert / pixel Shader pair
	bool SetShaderParameters(ID3D11DeviceContext * context, DirectX::SimpleMath::Matrix  *world, DirectX::SimpleMath::Matrix  *view, DirectX::SimpleMath::Matrix  *projection, Light *sceneLight1, ID3D11ShaderResourceView* texture1);
	void EnableShader(ID3D11DeviceContext * context);

private:
	//standard matrix buffer supplied to all shaders
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	//buffer for information of a single light
	struct LightBufferType
	{
		DirectX::SimpleMath::Vector4 ambient;
		DirectX::SimpleMath::Vector4 diffuse;
		DirectX::SimpleMath::Vector3 position;
		float padding;
	};

	//buffer to pass in camera world Position
	struct CameraBufferType
	{
		DirectX::SimpleMath::Vector3 cameraPosition;
		float padding;
	};

	//Shaders
	Microsoft::WRL::ComPtr<ID3D11VertexShader>								m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>								m_pixelShader;
	ID3D11InputLayout*														m_layout;
	ID3D11Buffer*															m_matrixBuffer;
	ID3D11SamplerState*														m_sampleState;
	ID3D11Buffer*															m_lightBuffer;
};

