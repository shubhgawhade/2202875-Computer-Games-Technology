////////////////////////////////////////////////////////////////////////////////
// Filename: particleshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARTICLESHADERCLASS_H_
#define _PARTICLESHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#pragma once
#include "DeviceResources.h"
#include <d3d11.h>
#include <d3dcompiler.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleShaderClass
////////////////////////////////////////////////////////////////////////////////
class ParticleShaderClass
{
private:
    struct MatrixBufferType
    {
        DirectX::SimpleMath::Matrix world;
        DirectX::SimpleMath::Matrix view;
        DirectX::SimpleMath::Matrix projection;
    };

public:
    ParticleShaderClass();
    ParticleShaderClass(const ParticleShaderClass&);
    ~ParticleShaderClass();

    bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
    bool Render(ID3D11DeviceContext * context, int indexCount, DirectX::SimpleMath::Matrix  *world, DirectX::SimpleMath::Matrix  *view, DirectX::SimpleMath::Matrix  *projection, ID3D11ShaderResourceView* texture1);

private:
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
    bool SetShaderParameters(ID3D11DeviceContext * context, DirectX::SimpleMath::Matrix  *world, DirectX::SimpleMath::Matrix  *view, DirectX::SimpleMath::Matrix  *projection, ID3D11ShaderResourceView* texture1);
    void RenderShader(ID3D11DeviceContext*, int);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>								m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>								m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11Buffer* m_matrixBuffer;
    ID3D11SamplerState* m_sampleState;
};

#endif