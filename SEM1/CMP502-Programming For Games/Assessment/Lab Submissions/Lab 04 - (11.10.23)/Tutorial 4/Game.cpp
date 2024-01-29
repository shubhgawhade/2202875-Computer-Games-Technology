//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();


#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input


	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

	//Render all game content. 
    Render();
}

// TASK 4
// Rotation variables
float rotAroundAxisDeg = 0;
float rotInAxisDeg = 0;
bool rotateClockwise = false;
bool axisRotation = false;

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	Vector3 eye(0.0f, 0.0f, 4.0f);
	Vector3 at(0.0f, 0.0f, 0.0f);

	m_view = Matrix::CreateLookAt(eye, at, Vector3::UnitY);

	m_world = Matrix::Identity;

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

    auto kb = m_keyboard->GetState();

    // TASK 4
#pragma region KEYBOARD INPUTS

    if (kb.A)
    {
        rotateClockwise = true;
    }
    else if (kb.D)
    {
        rotateClockwise = false;
    }

    if (kb.Q)
    {
        axisRotation = true;
    }
    else if (kb.E)
    {
        axisRotation = false;
    }

    if (rotateClockwise)
    {
        rotAroundAxisDeg -= 0.5f;

        if (axisRotation)
        {
            rotInAxisDeg -= 0.5f;
        }
    }
    else
    {
        rotAroundAxisDeg += 0.5f;

        if (axisRotation)
        {
            rotInAxisDeg += 0.5f;
        }
    }

#pragma endregion

    if (kb.Escape)
    {
        ExitGame();
    }
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{	
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Draw Text to the screen
    m_deviceResources->PIXBeginEvent(L"Draw sprite");
    m_sprites->Begin();
		m_font->DrawString(m_sprites.get(), L"DirectXTK Demo Window", XMFLOAT2(10, 10), Colors::Yellow);
    m_sprites->End();
    m_deviceResources->PIXEndEvent();
	
	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
//	context->RSSetState(m_states->Wireframe());


	// Turn our shaders on,  set parameters
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection);

    //prepare transform for first object. 
    //SimpleMath::Matrix newPosition = SimpleMath::Matrix::CreateTranslation(0.0f, 1.0f, 0.0f);
    //m_world = m_world * newPosition;

	//render our model
    //m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection);
	m_BasicModel.Render(context);

	//prepare transform for second object.
    SimpleMath::Matrix newscale1 = SimpleMath::Matrix::CreateScale(0.8f, 0.8f, 0.8f);
    SimpleMath::Matrix newRotation1 = SimpleMath::Matrix::CreateRotationX(1.57f);
    SimpleMath::Matrix newPosition1 = SimpleMath::Matrix::CreateTranslation(2.0f, 1.0f, 0.0f);
	m_world = m_world * newscale1 * newRotation1 * newPosition1;
    
	//setup and draw teapot
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection);
	m_BasicModel2.Render(context);

    // TASK 4
#pragma region ROTATING TEAPOT

    m_world = SimpleMath::Matrix::Identity;
    SimpleMath::Matrix newscale3 = SimpleMath::Matrix::CreateScale(0.5f, 0.5f, 0.5f);
    SimpleMath::Matrix radiusOfRot = SimpleMath::Matrix::CreateTranslation(0.f, 0.0f, 1.0f);
    SimpleMath::Matrix rotateInAxis = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(rotInAxisDeg));
    SimpleMath::Matrix newRotation3 = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(rotAroundAxisDeg));
    SimpleMath::Matrix rotateAroundPos = SimpleMath::Matrix::CreateTranslation(2.0f, 1.0f, 0.0f);

    //scale * rotation in axis * translate to radius * rotation around * final translation
    m_world = m_world * newscale3 * rotateInAxis * radiusOfRot * newRotation3 * rotateAroundPos;

    //setup and draw teapot
    m_BasicShaderPair.EnableShader(context);
    m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection);
    m_BasicModel2.Render(context);

#pragma endregion

	//prepare transform for second object. 
	m_world = SimpleMath::Matrix::Identity; //set world back to identity
	SimpleMath::Matrix newPosition2 = SimpleMath::Matrix::CreateTranslation(0.0f, -0.6f, 0.0f);
	m_world = m_world * newPosition2;
    
	//setup and draw cube
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection);
	m_BasicModel3.Render(context);


    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our test model
	m_BasicModel.InitializeSphere(device);
	m_BasicModel2.InitializeTeapot(device);
	m_BasicModel3.InitializeBox(device, 10.0f, 0.1f, 10.0f);	//box includes dimensions

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );
}


void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
