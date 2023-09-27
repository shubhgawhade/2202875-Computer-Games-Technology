//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

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

//float xAxis = 0.0f;
// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    //xAxis += 0.1f;

    Vector3 eye(0.0f, 0.0f, 5.0f);
    //Vector3 eye(xAxis, 0.0f, 5.0f);
	Vector3 at(0.0f, 0.0f, 0.0f);

	m_view = Matrix::CreateLookAt(eye, at, Vector3::UnitY);

	m_world = Matrix::Identity;

	m_batchEffect->SetView(m_view);
	m_batchEffect->SetWorld(Matrix::Identity);

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

    // Draw sprite
    m_deviceResources->PIXBeginEvent(L"Draw sprite");
    m_sprites->Begin();
		m_font->DrawString(m_sprites.get(), L"DirectXTK Demo Window", XMFLOAT2(10, 10), Colors::Yellow);
    m_sprites->End();
    m_deviceResources->PIXEndEvent();
	

	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullClockwise());
//	context->RSSetState(m_states->Wireframe());

	m_batchEffect->Apply(context);

	context->IASetInputLayout(m_batchInputLayout.Get());
	
	// CENTERED CUBE

    //Arrays of geomtry data.
	//const int vCountCubeCentered = 6;
	//const int iCountCubeCentered = 6;
	//uint16_t iArrayCubeCentered[iCountCubeCentered];
	//VertexPositionColor vArrayCubeCentered[vCountCubeCentered];
	//iArrayCubeCentered[0] = 0;
	//iArrayCubeCentered[1] = 1;
    //iArrayCubeCentered[2] = 2;
    //iArrayCubeCentered[3] = 3;
    //iArrayCubeCentered[4] = 4;
    //iArrayCubeCentered[5] = 5;
	//vArrayCubeCentered[0] = VertexPositionColor(Vector3(-0.5f, 0.5f, 0.5f), Colors::Yellow);	
	//vArrayCubeCentered[1] = VertexPositionColor(Vector3(-0.5f, -0.5f, 0.5f), Colors::Yellow);
    //vArrayCubeCentered[2] = VertexPositionColor(Vector3(0.5f, -0.5f, 0.5f), Colors::Yellow);
    //vArrayCubeCentered[3] = VertexPositionColor(Vector3(0.5f, -0.5f, 0.5f), Colors::Purple);
    //vArrayCubeCentered[4] = VertexPositionColor(Vector3(0.5f, 0.5f, 0.5f), Colors::Purple);
    //vArrayCubeCentered[5] = VertexPositionColor(Vector3(-0.5f, 0.5f, 0.5f), Colors::Purple);
    //
    ////begin geometric object
    m_batch->Begin();
    //m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArrayCubeCentered, iCountCubeCentered, vArrayCubeCentered, vCountCubeCentered);

    // CUBE MOVED TO THE LEFT

    const int vCountCubeLeft = 6;
    const int iCountCubeLeft = 6;
    uint16_t iArrayCubeLeft[iCountCubeLeft];
    VertexPositionColor vArrayCubeLeft[vCountCubeLeft];
    iArrayCubeLeft[0] = 0;
    iArrayCubeLeft[1] = 1;
    iArrayCubeLeft[2] = 2;
    iArrayCubeLeft[3] = 3;
    iArrayCubeLeft[4] = 4;
    iArrayCubeLeft[5] = 5;
    vArrayCubeLeft[0] = VertexPositionColor(Vector3(-2.5f, 0.5f, 0.5f), Colors::Yellow);
    vArrayCubeLeft[1] = VertexPositionColor(Vector3(-2.5f, -0.5f, 0.5f), Colors::Yellow);
    vArrayCubeLeft[2] = VertexPositionColor(Vector3(-1.5f, -0.5f, 0.5f), Colors::Yellow);
    vArrayCubeLeft[3] = VertexPositionColor(Vector3(-1.5f, -0.5f, 0.5f), Colors::Purple);
    vArrayCubeLeft[4] = VertexPositionColor(Vector3(-1.5f, 0.5f, 0.5f), Colors::Purple);
    vArrayCubeLeft[5] = VertexPositionColor(Vector3(-2.5f, 0.5f, 0.5f), Colors::Purple);
    
    m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArrayCubeLeft, iCountCubeLeft, vArrayCubeLeft, vCountCubeLeft);


    // Star Centered

    // Triangle Top
    //const int vCountStarTop = 3;
    //const int iCountStarTop = 3;
    //uint16_t iArrayStarTop[iCountStarTop];
    //VertexPositionColor vArrayStarTop[vCountStarTop];
    //iArrayStarTop[0] = 0;
    //iArrayStarTop[1] = 1;
    //iArrayStarTop[2] = 2;
    //vArrayStarTop[0] = VertexPositionColor(Vector3(0.f, 0.5f, 0.5f), Colors::Yellow);
    //vArrayStarTop[1] = VertexPositionColor(Vector3(-0.5f, -0.5f, 0.5f), Colors::Yellow);
    //vArrayStarTop[2] = VertexPositionColor(Vector3(0.5f, -0.5f, 0.5f), Colors::Yellow);
    //
    //m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArrayStarTop, iCountStarTop, vArrayStarTop, vCountStarTop);
    //
    //// Triangle Bottom
    //
    //const int vCountStarBottom = 3;
    //const int iCountStarBottom = 3;
    //uint16_t iArrayStarBottom[iCountStarBottom];
    //VertexPositionColor vArrayStarBottom[vCountStarBottom];
    //iArrayStarBottom[0] = 0;
    //iArrayStarBottom[1] = 1;
    //iArrayStarBottom[2] = 2;
    //vArrayStarBottom[0] = VertexPositionColor(Vector3(0.f, -0.75f, 0.5f), Colors::Yellow);
    //vArrayStarBottom[1] = VertexPositionColor(Vector3(0.5f, 0.25f, 0.5f), Colors::Yellow);
    //vArrayStarBottom[2] = VertexPositionColor(Vector3(-0.5f, 0.25f, 0.5f), Colors::Yellow);
    //
    //m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArrayStarBottom, iCountStarBottom, vArrayStarBottom, vCountStarBottom);

    // Circle
    float radius = 3;
    int sides = 3;
    float angle = 360 / sides;

    const int vCountCircle = 9;
    const int iCountCircle = 9;
    uint16_t iArrayCircle[iCountCircle];
    VertexPositionColor vArrayCircle[vCountCircle];
    
    for (int i = 0; i < iCountCircle; i++)
    {
        iArrayCircle[i] = i;
    }

    vArrayCircle[0] = VertexPositionColor(Vector3(0.f, 0.f, 0.5f), Colors::Red);
    vArrayCircle[1] = VertexPositionColor(Vector3(radius * cos(XMConvertToRadians(angle)), radius * sin(XMConvertToRadians(angle)), 0.5f), Colors::Red);
    angle += 360 / sides;
    vArrayCircle[2] = VertexPositionColor(Vector3(radius * cos(XMConvertToRadians(angle)), radius * sin(XMConvertToRadians(angle)), 0.5f), Colors::Red);
    vArrayCircle[3] = VertexPositionColor(Vector3(0.f, 0.f, 0.5f), Colors::Red);
    vArrayCircle[4] = VertexPositionColor(Vector3(radius * cos(XMConvertToRadians(angle)), radius * sin(XMConvertToRadians(angle)), 0.5f), Colors::Red);
    angle += 360 / sides;
    vArrayCircle[5] = VertexPositionColor(Vector3(radius * cos(XMConvertToRadians(angle)), radius * sin(XMConvertToRadians(angle)), 0.5f), Colors::Red);
    vArrayCircle[6] = VertexPositionColor(Vector3(0.f, 0.f, 0.5f), Colors::Red);
    vArrayCircle[7] = VertexPositionColor(Vector3(radius * cos(XMConvertToRadians(angle)), radius * sin(XMConvertToRadians(angle)), 0.5f), Colors::Red);
    angle += 360 / sides;
    vArrayCircle[8] = VertexPositionColor(Vector3(radius * cos(XMConvertToRadians(angle)), radius * sin(XMConvertToRadians(angle)), 0.5f), Colors::Red);

    //const int vCountCircle = 6;
    //const int iCountCircle = 6;
    //uint16_t iArrayCircle[iCountCircle];
    //VertexPositionColor vArrayCircle[vCountCircle];
    //
    //float angle = 0;
    //for (int i = 0; i < vCountCircle;)
    //{
    //    angle += XMConvertToRadians(360 / sides);
    //    vArrayCircle[i++] = VertexPositionColor(Vector3(0.f, 0.f, 0.5f), Colors::Red);
    //    vArrayCircle[i++] = VertexPositionColor(Vector3(radius * cos(angle), radius * sin(angle), 0.5f), Colors::Red);
    //    vArrayCircle[i++] = VertexPositionColor(Vector3(radius * cos(angle), radius * sin(angle), 0.5f), Colors::Red);
    //
    //    //iArrayCircle[i] = i;
    //    //vArrayCircle[i] = VertexPositionColor(Vector3(radius * cos(360/i), radius * sin(360 / i), 0.5f), Colors::Red);
    //}

    m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArrayCircle, iCountCircle, vArrayCircle, vCountCircle);

	
	m_batch->End();

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

    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

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

    m_batchEffect->SetProjection(m_projection);
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_batchEffect.reset();
    m_font.reset();
	m_batch.reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
