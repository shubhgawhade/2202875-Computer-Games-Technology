////////////////////////////////////////////////////////////////////////////////
// Filename: ParticleSystemClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARTICLESYSTEMCLASS_H_
#define _PARTICLESYSTEMCLASS_H_

//////////////
// INCLUDES //
//////////////
#include "pch.h"
#include "Camera.h"
//#include <d3dx10math.h>
//#include <fstream>
//using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleSystemClass
////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

class ParticleSystemClass
{
private:

    struct ParticleType
    {
        float positionX, positionY, positionZ;
        float rotationX, rotationY, rotationZ;
        float red, green, blue;
        float velocity;
        bool active;
    };

    struct VertexType
    {
        DirectX::SimpleMath::Vector3 position;
        // DirectX::SimpleMath::Vector3 rotation;
        DirectX::SimpleMath::Vector2 texture;
        DirectX::SimpleMath::Vector4 color;
    };

public:
    ParticleSystemClass();
    ~ParticleSystemClass();

    bool Initialize(ID3D11Device*);
    bool Frame(float, ID3D11DeviceContext*);
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();
    // SimpleMath::Matrix LookAt();

private:
    bool InitializeParticleSystem();

    bool InitializeBuffers(ID3D11Device*);

    void EmitParticles(float);
    void UpdateParticles(float);
    void KillParticles();

    bool UpdateBuffers(ID3D11DeviceContext*);

    void RenderBuffers(ID3D11DeviceContext*);

private:
    Camera																	m_Camera;
    
    float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
    float m_particleVelocity, m_particleVelocityVariation;
    float m_particleSize, m_particlesPerSecond;
    int m_maxParticles;
    int m_currentParticleCount;
    float m_accumulatedTime;
    ID3D11ShaderResourceView* m_Texture;
    ParticleType* m_particleList;
    bool willUpdate;
    float temp_y;
    int m_vertexCount, m_indexCount;
    VertexType* m_vertices;
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    SimpleMath::Matrix lookAt;
};

#endif