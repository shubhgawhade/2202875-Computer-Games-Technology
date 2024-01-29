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

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	Vector3 eye(0.0f, 0.0f, 5.0f);
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

    // Draw Text to the screen
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


	context->IASetInputLayout(m_batchInputLayout.Get());
	
	//Arrays of geomtry data.
	const int vCount = 3;
	const int iCount = 3;
	uint16_t iArray[iCount];
	VertexPositionColor vArray[vCount];
	iArray[0] = 0;
	iArray[1] = 1;
	iArray[2] = 2;
	vArray[0] = VertexPositionColor(Vector3(0.f, 0.5f, 0.5f),	Colors::Yellow);	
	vArray[1] = VertexPositionColor(Vector3(-0.5f, -0.5f, 0.5f),Colors::Red);
	vArray[2] = VertexPositionColor(Vector3(0.5f, -0.5f, 0.5f), Colors::Blue);

				
	//begin geometric object
	m_batch->Begin();

		//apply loaded shader
		context->IASetInputLayout(m_layout);							//set the input layout for the shader to match out geometry
		SetShaderParameters(&m_world, &m_view, &m_projection);			//send the world, view and projection matrices into the shader
		context->VSSetShader(m_vertexShader.Get(), 0, 0);				//turn on vertex shader
		context->PSSetShader(m_pixelShader.Get(), 0, 0);				//turn on pixel shader

		m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArray, iCount, vArray, vCount);
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

	//SHADER
	unsigned int numElements;

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

	//LOAD SHADER:	VERTEX
	auto vertexShaderBuffer = DX::ReadData(L"colour_vs.cso");
	DX::ThrowIfFailed(device->CreateVertexShader(vertexShaderBuffer.data(), vertexShaderBuffer.size() ,NULL, &m_vertexShader));
	
	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the MeshClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer.data(), vertexShaderBuffer.size(), &m_layout);

	//LOAD SHADER:	PIXEL
	auto pixelShaderBuffer = DX::ReadData(L"colour_ps.cso");		//Yes we are re-using the same buffer
	device->CreatePixelShader(pixelShaderBuffer.data(), pixelShaderBuffer.size(), NULL, &m_pixelShader);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	m_matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	m_matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_matrixBufferDesc.MiscFlags = 0;
	m_matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	device->CreateBuffer(&m_matrixBufferDesc, NULL, &m_matrixBuffer);
	
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

void Game::SetShaderParameters(DirectX::SimpleMath::Matrix  *world, DirectX::SimpleMath::Matrix  *view, DirectX::SimpleMath::Matrix  *projection)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	DirectX::SimpleMath::Matrix  tworld, tview, tproj;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();

	tworld	= world->Transpose();
	tview	= view->Transpose();
	tproj	= projection->Transpose();

	// Lock the constant buffer so it can be written to.
	result = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world	= tworld;// worldMatrix;
	dataPtr->view	= tview;
	dataPtr->projection = tproj;

	// Unlock the constant buffer.
	context->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer); 
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
