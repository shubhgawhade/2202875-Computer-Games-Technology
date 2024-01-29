//
// Game.h
//
#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Shader.h"
#include "modelclass.h"
#include "Light.h"
#include "Input.h"
#include "Camera.h"

#define DXTK_AUDIO
#include "Audio.h"
#include "particleshaderclass.h"
#include "ParticleSystemClass.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);
	int Height;
	int Width;

#ifdef DXTK_AUDIO
    void NewAudioDevice();
#endif

    // Properties
    void GetDefaultSize( int& width, int& height ) const;
	
private:

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	}; 

    void Update(DX::StepTimer const& timer);
    void Render();
    void Clear();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	//input manager. 
	Input									m_input;
	InputCommands							m_gameInputCommands;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;	
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

	// Scene Objects
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	std::unique_ptr<DirectX::GeometricPrimitive>                            m_testmodel;

	//lights
	Light																	m_Light;

	//Cameras
    Camera																	m_Camera01;
    // Camera																	m_Camera02;

	//textures 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        grassAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        grassNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        fxtexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        starsTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        starsTexture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        sunTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        mercuryTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        venusTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        earthTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        moonTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        marsTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        jupiterTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        saturnTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        saturnRingTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        uranusTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        neptuneTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        rocketTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        asteroidTexture;

	ParticleShaderClass m_ParticleShader;
	ParticleSystemClass m_ParticleSystem;
	D3D11_BLEND_DESC blendStateDescription;


	//Shaders
	Shader																	m_BasicShaderPair;
	Shader																	m_BasicShaderPair1;
	// ModelClass																m_BasicModel;
	// ModelClass																m_BasicModel2;
	// ModelClass																m_BasicModel3;
	ModelClass																m_skyBox;
	ModelClass																m_astronaut;
	ModelClass																m_planet;
	ModelClass																m_saturnRing;
	ModelClass																m_rocket;
	ModelClass																m_asteroid1;
	ModelClass																m_asteroid2;
	ModelClass																m_asteroid3;
	ModelClass																m_asteroid4;
	ModelClass																m_asteroid5;
	ModelClass																m_asteroid6;
	// ModelClass																m_asteroid7;
	// ModelClass																m_asteroid8;
	// ModelClass																m_mercuryPlanet;

	bool asteroidsInitiaized = false;
    class AsteroidClass
    {
    public:
	    ModelClass model;
    	float scale;
    	float initDistance;
    	SimpleMath::Vector3 localRotation;
    	SimpleMath::Vector2 axialSpeed;
    	float revolutionSpeed;
    };

	int numberOfAsteroids;
	AsteroidClass*																m_asteroids;

#ifdef DXTK_AUDIO
	AudioEmitter emitter;
	AudioListener listener;
	
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundJetpack;
    // std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effectJetpack;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effectJetpackBoost;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effectRocket;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effectChillBG;
#endif
    

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;
};