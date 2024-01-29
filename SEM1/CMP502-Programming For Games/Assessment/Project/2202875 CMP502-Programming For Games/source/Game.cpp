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

Vector3 position;
Vector3 rotation;

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	Width = width;
	Height = height;

	m_input.Initialise(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	//setup light
	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(2.0f, 1.0f, 1.0f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera01.setPosition(Vector3(0.0f, 2.0f, 26.0f));
	m_Camera01.setRotation(Vector3(-90.0f, 180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up.
	position = m_Camera01.getPosition(); //get the position
	rotation = m_Camera01.getRotation(); //get the rotation
	//m_Camera01.active = true;

    // m_Camera02.setPosition(Vector3(0.0f, 0.0f, -4.0f));
    // m_Camera02.setRotation(Vector3(-90.0f, -180.0f, 0.0f)); 
    //m_Camera02.active = false;
	
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

    // m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"Resources/Audio/adpcmdroid.xwb");
	// m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"Resources/Audio/MusicMono_adpcm.wav");
	m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"Resources/Audio/ProjectAudio.xwb");

	m_effectJetpack = m_waveBank->CreateInstance(0u, SoundEffectInstance_Use3D);
	m_effectJetpackBoost = m_waveBank->CreateInstance(1,SoundEffectInstance_Use3D);
	m_effectRocket = m_waveBank->CreateInstance(2,SoundEffectInstance_Use3D);
	m_effectChillBG = m_waveBank->CreateInstance(3);

	m_effectChillBG->Play(true);
	
	// m_effect1 = m_soundEffect->CreateInstance();
	// m_effect1->Play(true);
	
	// emitter.SetPosition(Vector3(0,0,0));

    // m_effect1->Play(true);
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	// if(m_deviceResources->GetScreenViewport().Height != Height || m_deviceResources->GetScreenViewport().Width != Width)
	// {
	// 	Height = m_deviceResources->GetScreenViewport().Height;
	// 	Width = m_deviceResources->GetScreenViewport().Width;
	// }

	if(m_gameInputCommands.pause)
	{
		int response = MessageBox(NULL, L"CONTROLS: \n\nW - Forward \nA - Left\nS - Backwards\nD - Right\nQ - Down\nE - Up\nL Shift - Boost\n\nP - Pause\nEsc - Quit\n\nHit ENTER to Un-Pause", L"INFO!", MB_OK | MB_ICONWARNING);
		if(response)
		{
			m_gameInputCommands.pause = false;
		}
		
		return;
	}
	
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game
	
	//Update all game objects
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

	//Render all game content. 
    Render();

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

	
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{	
	Vector3 dir;
	bool isMoving = false;

	if (m_gameInputCommands.left)
	{
		dir -= (m_Camera01.getRight() * m_timer.GetElapsedSeconds()); //add the left vector
		isMoving = true;

		// emitter.SetPosition(Vector3(m_Camera01.getPosition().x + 2, m_Camera01.getPosition().y, m_Camera01.getPosition().z));
;		// m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.right)
	{
		dir += (m_Camera01.getRight() * m_timer.GetElapsedSeconds()); //add the right vector
		isMoving = true;

		// emitter.SetPosition(Vector3(m_Camera01.getPosition().x - 2, m_Camera01.getPosition().y, m_Camera01.getPosition().z));
		// m_Camera01.setPosition(position);
	}
	if(m_gameInputCommands.up)
	{
		dir += (m_Camera01.getRight().Cross(m_Camera01.getForward()) * m_timer.GetElapsedSeconds()); //add the up vector
		isMoving = true;

		// emitter.SetPosition(Vector3(m_Camera01.getPosition().x, m_Camera01.getPosition().y - 2, m_Camera01.getPosition().z));
		// m_Camera01.setPosition(position);
	}
	if(m_gameInputCommands.down)
	{
		dir -= (m_Camera01.getRight().Cross(m_Camera01.getForward()) * m_timer.GetElapsedSeconds()); //add the down vector
		isMoving = true;

		// emitter.SetPosition(Vector3(m_Camera01.getPosition().x, m_Camera01.getPosition().y + 2, m_Camera01.getPosition().z));
		// m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.forward)
	{
        dir += (m_Camera01.getForward() * m_timer.GetElapsedSeconds()); //add the forward vector
		isMoving = true;

		// emitter.SetPosition(Vector3(m_Camera01.getPosition().x, m_Camera01.getPosition().y, m_Camera01.getPosition().z + 2));
		// m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.back)
	{
		dir -= (m_Camera01.getForward()* m_timer.GetElapsedSeconds()); //add the back vector
		isMoving = true;

		// emitter.SetPosition(Vector3(m_Camera01.getPosition().x, m_Camera01.getPosition().y, m_Camera01.getPosition().z - 2));
		// m_Camera01.setPosition(position);
	}

	// emitter.SetPosition(Vector3(0,0,0));
	// emitter.Update(m_Camera01.getPosition(), m_Camera01.getForward().Cross(-m_Camera01.getRight()), 1.0f);
	if(isMoving)
	{
		m_effectJetpack->SetVolume(0.5f);
		m_effectJetpack->Play(true);
		// m_effectJetpack->Apply3D(listener,emitter);


		if(m_gameInputCommands.boost)
		{
			dir *= m_Camera01.getBoostSpeed();

			m_effectJetpackBoost->SetVolume(0.5f);
			m_effectJetpackBoost->Play(true);
			// m_effectJetpackBoost->Apply3D(listener,emitter);

		}
		else
		{
			m_effectJetpackBoost->Stop();
		}
	}
	else
	{
		m_effectJetpack->Stop();
		m_effectJetpackBoost->Stop();
	}

	// m_effectJetpack->Apply3D(listener,emitter);
	// m_effectJetpackBoost->Apply3D(listener,emitter);
	
	// Normalize movement direction to prevent faster diagonal speed
	dir.Normalize();

	dir *= m_Camera01.getMoveSpeed();
	
	position += dir;
	m_Camera01.setSmoothPosition(position);

	rotation += Vector3(0, -m_gameInputCommands.mouseX * m_Camera01.getRotationSpeed(),-m_gameInputCommands.mouseY * m_Camera01.getRotationSpeed());

	rotation.z = std::min(90.0f, rotation.z);
	rotation.z = std::max(-90.0f, rotation.z);

	m_Camera01.setSmoothRotation(rotation);
    
    // if (m_gameInputCommands.camera2)
    // {
    //     m_Camera02.Update();	//camera update.
    //
    //     m_view = m_Camera02.getCameraMatrix();
    // }
    // else
    // {
    //     m_Camera01.Update();	//camera update.
    //
    //     m_view = m_Camera01.getCameraMatrix();
    // }

	m_Camera01.Update();	//camera update.
	m_view = m_Camera01.getCameraMatrix();

	listener.SetPosition(Vector3(m_Camera01.getPosition()));
	listener.SetOrientation(m_Camera01.getForward(), m_Camera01.getForward().Cross(-m_Camera01.getRight()));
	
	m_world = Matrix::Identity;

#ifdef DXTK_AUDIO
    // m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    // if (m_audioTimerAcc < 0)
    // {
    //     if (m_retryDefault)
    //     {
    //         m_retryDefault = false;
    //         if (m_audEngine->Reset())
    //         {
    //             // Restart looping audio
    //             m_effect1->Play(true);
    //         }
    //     }
    //     else
    //     {
    //         m_audioTimerAcc = 4.f;
    //
    //         m_waveBank->Play(m_audioEvent++);
    //
    //         if (m_audioEvent >= 11)
    //             m_audioEvent = 0;
    //     }
    // }
#endif

  
	if (m_input.Quit())
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
	
	//Set Rendering states. 
	context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
//	context->RSSetState(m_states->Wireframe());



	ModelClass model;
	ComPtr<ID3D11ShaderResourceView> texture;
	
	int submodel = -1;
	for(int i = 0; i < 9; i++)
	{
		// SUN
		if(i==0)
		{
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			// SimpleMath::Matrix sunPos = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
			SimpleMath::Matrix sunRot = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.15f);
			SimpleMath::Matrix sunScale = SimpleMath::Matrix::CreateScale(6);
			m_world = sunRot * m_world * sunScale;
			model= m_planet;
			texture = sunTexture;
		}
		// MERCURY
		else if(i==1)
		{
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			SimpleMath::Matrix mercuryRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(43.7f));
			SimpleMath::Matrix mercuryRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix mercuryRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 2.4f);
			SimpleMath::Matrix localRotations = mercuryRotLocalZ * mercuryRotLocalX;
			SimpleMath::Matrix mercuryPos = SimpleMath::Matrix::CreateTranslation(-9.7f, 0.0f, 0.0f);
			SimpleMath::Matrix mercuryRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix mercuryRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix mercuryRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix mercuryRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 1.5f);
			SimpleMath::Matrix globalRotations = mercuryRotZ * mercuryRotX * mercuryRotY;
			SimpleMath::Matrix mercuryScale = SimpleMath::Matrix::CreateScale(0.4f);
			// m_world = mercuryScale * mercuryRotLocalYAxis * localRotations * m_world * mercuryPos * mercuryRotY;
			m_world = mercuryScale * mercuryRotLocalYAxis * localRotations * m_world * mercuryPos * mercuryRotAroundYAxis * globalRotations;
			model = m_planet;
			texture = mercuryTexture;
		}
		// VENUS
		else if(i==2)
		{
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			SimpleMath::Matrix venusRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix venusRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix venusRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix venusRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * -0.6f);
			SimpleMath::Matrix localRotations = venusRotLocalZ * venusRotLocalX * venusRotLocalY;
			SimpleMath::Matrix venusPos = SimpleMath::Matrix::CreateTranslation(-12.2f, 0.0f, 0.0f);
			SimpleMath::Matrix venusRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix venusRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix venusRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix venusRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.6f);
			SimpleMath::Matrix globalRotations = venusRotZ * venusRotX * venusRotY;
			SimpleMath::Matrix venusScale = SimpleMath::Matrix::CreateScale(1.24f);
			m_world = venusScale * venusRotLocalYAxis * localRotations * m_world * venusPos * venusRotAroundYAxis * globalRotations;
			model = m_planet;
			texture = venusTexture;
		}
		// EARTH
		else if(i==3)
		{
			if(submodel == -1)
			{
				submodel = 2;
			}
			
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			SimpleMath::Matrix earthRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(180 + 23.4f));
			SimpleMath::Matrix earthRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix earthRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix earthRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 6.0f);
			SimpleMath::Matrix earthLocalRotations = earthRotLocalZ * earthRotLocalX * earthRotLocalY;
			SimpleMath::Matrix earthPos = SimpleMath::Matrix::CreateTranslation(-16.1f, 0.0f, 0.0f);
			SimpleMath::Matrix earthRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(12.0f));
			SimpleMath::Matrix earthRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix earthRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix earthRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.45f);
			SimpleMath::Matrix earthGlobalRotations = earthRotZ * earthRotX * earthRotY;
			SimpleMath::Matrix earthScale = SimpleMath::Matrix::CreateScale(1.0f);

			if(submodel == 2)
			{
				m_world = earthScale * earthRotLocalYAxis * earthLocalRotations * m_world * earthPos * earthRotAroundYAxis * earthGlobalRotations;
				model = m_planet;
				texture = earthTexture;

				submodel--;
			}
			else if(submodel == 1)
			{
				//prepare transform for object.
				m_world = SimpleMath::Matrix::Identity; //set world back to identity
				SimpleMath::Matrix moonRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-6.68f));
				SimpleMath::Matrix moonRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
				SimpleMath::Matrix moonRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
				SimpleMath::Matrix moonRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 6.0f);
				SimpleMath::Matrix moonLocalRotations = moonRotLocalZ * moonRotLocalX * moonRotLocalY;
				SimpleMath::Matrix moonPos = SimpleMath::Matrix::CreateTranslation(-1.8f, 0.0f, 0.0f);
				SimpleMath::Matrix moonRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(5.14f));
				SimpleMath::Matrix moonRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
				SimpleMath::Matrix moonRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
				SimpleMath::Matrix moonRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 6.0f);
				SimpleMath::Matrix moonGlobalRotations = moonRotZ * moonRotX * moonRotY;
				SimpleMath::Matrix moonScale = SimpleMath::Matrix::CreateScale(0.27f);
				
				m_world = moonScale * moonRotLocalYAxis * moonLocalRotations * m_world * moonPos * moonRotAroundYAxis *
					earthPos * earthRotAroundYAxis * earthGlobalRotations;
				
				model = m_planet;
				texture = moonTexture;

				submodel--;
			}
			else if(submodel == 0)
			{
				//prepare transform for object.
				m_world = SimpleMath::Matrix::Identity; //set world back to identity
				SimpleMath::Matrix rocketRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-90.0f));
				SimpleMath::Matrix rocketRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
				SimpleMath::Matrix rocketRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
				SimpleMath::Matrix rocketLocalRotations = rocketRotLocalZ * rocketRotLocalX * rocketRotLocalY;
				SimpleMath::Matrix rocketPos = SimpleMath::Matrix::CreateTranslation(19.0f, 0.0f, 0.0f);
				SimpleMath::Matrix rocketRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
				SimpleMath::Matrix rocketRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
				SimpleMath::Matrix rocketRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(25.0f));
				SimpleMath::Matrix rocketRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.45f);
				SimpleMath::Matrix rocketGlobalRotations = rocketRotZ * rocketRotX * rocketRotY;
				SimpleMath::Matrix rocketScale = SimpleMath::Matrix::CreateScale(0.7f);
				
				m_world = rocketScale * rocketLocalRotations * m_world * rocketPos * rocketRotAroundYAxis * rocketGlobalRotations;

				Vector3 s;
				Quaternion r;
				Vector3 p;
				m_world.Decompose(s, r, p);

				m_effectRocket->Play(true);
				emitter.Position = p;
				m_effectRocket->Apply3D(listener, emitter);
				
				model = m_rocket;
				texture = rocketTexture;

				context->OMSetBlendState(m_states->Additive(), nullptr, 0xFFFFFFFF);
				
				SimpleMath::Matrix rocketParticlesRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-90.0f));
				
				DirectX::SimpleMath::Matrix m_world1;
				m_world1 = rocketParticlesRotX * rocketLocalRotations * m_world1 * rocketPos * rocketRotAroundYAxis * rocketGlobalRotations;
				m_ParticleSystem.Render(context);
				m_ParticleSystem.Frame(5, m_deviceResources->GetD3DDeviceContext());
				m_ParticleShader.Render(context, m_ParticleSystem.GetIndexCount(), &m_world1, &m_view, &m_projection, 
								  fxtexture.Get());
				
				context->OMSetBlendState(m_states->AlphaBlend(), nullptr, 0xFFFFFFFF);

				submodel--;
			}
		}
		// MARS
		else if(i==4)
		{			
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			SimpleMath::Matrix marsRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix marsRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix marsRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix marsRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 5.75f);
			SimpleMath::Matrix localRotations = marsRotLocalZ * marsRotLocalX * marsRotLocalY;
			SimpleMath::Matrix marsPos = SimpleMath::Matrix::CreateTranslation(-24.47f, 0.0f, 0.0f);
			SimpleMath::Matrix marsRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix marsRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix marsRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix marsRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.23f);
			SimpleMath::Matrix globalRotations = marsRotZ * marsRotX * marsRotY;
			SimpleMath::Matrix marsScale = SimpleMath::Matrix::CreateScale(0.52f);

			m_world = marsScale * marsRotLocalYAxis * localRotations * m_world * marsPos * marsRotAroundYAxis * globalRotations;
			model = m_planet;
			texture = marsTexture;
		}
		// JUPITER
		else if(i==5)
		{
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			SimpleMath::Matrix jupiterRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-7.16f));
			SimpleMath::Matrix jupiterRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix jupiterRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix jupiterRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 15.6f);
			SimpleMath::Matrix localRotations = jupiterRotLocalZ * jupiterRotLocalX * jupiterRotLocalY;
			SimpleMath::Matrix jupiterPos = SimpleMath::Matrix::CreateTranslation(-31.72f, 0.0f, 0.0f);
			SimpleMath::Matrix jupiterRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix jupiterRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix jupiterRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix jupiterRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.16f);
			SimpleMath::Matrix globalRotations = jupiterRotZ * jupiterRotX * jupiterRotY;
			SimpleMath::Matrix jupiterScale = SimpleMath::Matrix::CreateScale(2.73f);
			m_world = jupiterScale * jupiterRotLocalYAxis * localRotations * m_world * jupiterPos * jupiterRotAroundYAxis * globalRotations;
			model = m_planet;
			texture = jupiterTexture;
		}
		// SATURN
		else if(i==6)
		{
			if(submodel == -1)
			{
				submodel = 1;
			}
			
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			SimpleMath::Matrix saturnRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(8.0f));
			SimpleMath::Matrix saturnRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix saturnRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix saturnRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 15.0f);
			SimpleMath::Matrix localRotations = saturnRotLocalZ * saturnRotLocalX * saturnRotLocalY;
			SimpleMath::Matrix saturnPos = SimpleMath::Matrix::CreateTranslation(-58.43f, 0.0f, 0.0f);
			SimpleMath::Matrix saturnRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix saturnRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix saturnRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix saturnRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.05f);
			SimpleMath::Matrix globalRotations = saturnRotZ * saturnRotX * saturnRotY;
			SimpleMath::Matrix saturnScale = SimpleMath::Matrix::CreateScale(2.15f);

			if(submodel == 1)
			{
				m_world = saturnScale * saturnRotLocalYAxis * localRotations * m_world * saturnPos * saturnRotAroundYAxis * globalRotations;
				model = m_planet;
				texture = saturnTexture;
				
				submodel--;
			}
			else if(submodel == 0)
			{
				//prepare transform for object.
				m_world = SimpleMath::Matrix::Identity; //set world back to identity
				SimpleMath::Matrix saturnRingRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(8.5f));
				SimpleMath::Matrix saturnRingRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
				SimpleMath::Matrix saturnRingRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(2.9f));
				SimpleMath::Matrix saturnRingRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 15.0f);
				SimpleMath::Matrix localRotations = saturnRingRotLocalZ * saturnRingRotLocalX * saturnRingRotLocalY;

				m_world = saturnScale * saturnRingRotLocalYAxis * localRotations * m_world * saturnPos * saturnRotAroundYAxis * globalRotations;
				model = m_saturnRing;
				texture = saturnRingTexture;
				
				submodel--;
			}
		}
		// URANUS
		else if(i==7)
		{
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			SimpleMath::Matrix uranusRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix uranusRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix uranusRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(-90.0f));
			SimpleMath::Matrix uranusRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 8.5f);
			SimpleMath::Matrix localRotations = uranusRotLocalZ * uranusRotLocalX * uranusRotLocalY;
			SimpleMath::Matrix uranusPos = SimpleMath::Matrix::CreateTranslation(-72.27f, 0.0f, 0.0f);
			SimpleMath::Matrix uranusRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(-8.0f));
			SimpleMath::Matrix uranusRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix uranusRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix uranusRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.01f);
			SimpleMath::Matrix globalRotations = uranusRotZ * uranusRotX * uranusRotY;
			SimpleMath::Matrix uranusScale = SimpleMath::Matrix::CreateScale(0.87f);
			m_world = uranusScale * uranusRotLocalYAxis * localRotations * m_world * uranusPos * uranusRotAroundYAxis * globalRotations;
			model = m_planet;
			texture = uranusTexture;
		}
		// NEPTUNE
		else if(i==8)
		{
			//prepare transform for object.
			m_world = SimpleMath::Matrix::Identity; //set world back to identity
			SimpleMath::Matrix neptuneRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix neptuneRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix neptuneRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix neptuneRotLocalYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 9.01f);
			SimpleMath::Matrix localRotations = neptuneRotLocalZ * neptuneRotLocalX * neptuneRotLocalY;
			SimpleMath::Matrix neptunePos = SimpleMath::Matrix::CreateTranslation(90.0f, 0.0f, 0.0f);
			SimpleMath::Matrix neptuneRotX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(0.0f));
			SimpleMath::Matrix neptuneRotY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(0.0f));
			SimpleMath::Matrix neptuneRotZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(0.0f));
			SimpleMath::Matrix neptuneRotAroundYAxis = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 0.005f);
			SimpleMath::Matrix globalRotations = neptuneRotZ * neptuneRotX * neptuneRotY;
			SimpleMath::Matrix neptuneScale = SimpleMath::Matrix::CreateScale(1.19f);
			m_world = neptuneScale * neptuneRotLocalYAxis * localRotations * m_world * neptunePos * neptuneRotAroundYAxis * globalRotations;
			model = m_planet;
			texture = neptuneTexture;
		}

		if(submodel > -1)
		{
			i--;
		}

		// Turn our shaders on, set parameters
		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, texture.Get());

		//render our sun model
		model.Render(context);		
	}
	
	// ASTEROID BELT
	if(!asteroidsInitiaized)
	{
		numberOfAsteroids = 500;
		m_asteroids = new AsteroidClass[numberOfAsteroids];

		// INITIALIZE ASTEROIDS
		for (int i = 0; i< numberOfAsteroids; i++)
		{
			// Random type
			
			srand (time(NULL) * i);
			int asteroidChosen = rand() % 6;
			switch (asteroidChosen)
			{
			case 0:
		
				m_asteroids[i].model = m_asteroid1;
				
				break;
		
			case 1:
		
				m_asteroids[i].model = m_asteroid2;
				
				break;
		
			case 2:
		
				m_asteroids[i].model = m_asteroid3;
				
				break;
		
			case 3:
		
				m_asteroids[i].model = m_asteroid4;
				
				break;
				
			case 4:
		
				m_asteroids[i].model = m_asteroid5;
				
				break;
				
			case 5:
		
				m_asteroids[i].model = m_asteroid6;
				
				break;

			// case 6:
			//
			// 	m_asteroids[i].model = m_asteroid7;
			// 	
			// 	break;
			//
			// case 7:
			//
			// 	m_asteroids[i].model = m_asteroid8;
			// 	
			// 	break;
			}
			
			// Random Scale
			srand (time(NULL) * i);
			float randomScale = 0;
			do
			{
				int randomScaleInt = rand() % 75;
				randomScale = static_cast<float>(randomScaleInt)/100.0f;
				m_asteroids[i].scale = randomScale;
			}
			while (randomScale < 0.08f);

			// Random Local Rotation
			for(int j = 0; j < 3; j++)
			{
				srand (time(NULL) * i * j);
				int randomRotInt = rand() % 360;
				int randomRotFloat = rand() % 10;
				float randomRot = static_cast<float>(randomRotInt) + static_cast<float>(randomRotFloat)/10.0f;

				if(j == 0)
				{
					m_asteroids[i].localRotation.x = randomRot;
				}
				else if(j == 1)
				{
					m_asteroids[i].localRotation.y = randomRot;
				}
				else if(j == 2)
				{
					m_asteroids[i].localRotation.z = randomRot;
				}
			}

			// Rotation Axis and speed
			srand (time(NULL) * i);
			int randomRotAxis = rand() % 4;
			int randomRotSpeedInt = rand() % 5;
			int randomRotSpeedFloat = rand() % 10;
			float randomRotSpeed = static_cast<float>(randomRotSpeedInt) + static_cast<float>(randomRotSpeedFloat)/10.0f;

			m_asteroids[i].axialSpeed.x = randomRotAxis;
			m_asteroids[i].axialSpeed.y = randomRotSpeed;

			// Random Distance 25-27m
			srand (time(NULL) * i);
			int randomDistanceInt = rand() % 4 + 25;
			int randomDistanceFloat = rand() % 10;
			int randomDir = rand() % 2;
			float randomDistance = static_cast<float>(randomDistanceInt) + static_cast<float>(randomDistanceFloat)/10.0f;
			if(randomDir == 0) randomDistance *= -1;
			m_asteroids[i].initDistance = randomDistance;

			// Random Revolution Speed
			srand (time(NULL) * i);
			int randomRevoutionSpeedInt = rand() % 1;
			int randomRevoutionSpeedFloat = rand() % 100 + 1;
			float randomRevoutionSpeed = static_cast<float>(randomRevoutionSpeedInt) + static_cast<float>(randomRevoutionSpeedFloat)/10.0f;

			m_asteroids[i].revolutionSpeed = randomRevoutionSpeed;
		}

		asteroidsInitiaized = true;
	}

	// Calculate and render initiaized asteroids
	for(int i = 0; i < numberOfAsteroids; i++)
	{
		m_world = Matrix::Identity;
		SimpleMath::Matrix asteroidScale = SimpleMath::Matrix::CreateScale(m_asteroids[i].scale);
		SimpleMath::Matrix asteroidRotLocalX = SimpleMath::Matrix::CreateRotationX(XMConvertToRadians(m_asteroids[i].localRotation.x));
		SimpleMath::Matrix asteroidRotLocalY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(m_asteroids[i].localRotation.y));
		SimpleMath::Matrix asteroidRotLocalZ = SimpleMath::Matrix::CreateRotationZ(XMConvertToRadians(m_asteroids[i].localRotation.z));

		SimpleMath::Matrix asteroidLocalRotation;
		switch (static_cast<int>(m_asteroids[i].axialSpeed.x))
		{
		case 0:

			 asteroidLocalRotation =SimpleMath::Matrix::CreateRotationZ(m_timer.GetTotalSeconds());
			
			break;

		case 1:

			asteroidLocalRotation = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds());
			
			break;

		case 2:

			asteroidLocalRotation = SimpleMath::Matrix::CreateRotationZ(m_timer.GetTotalSeconds());

			break;
		}
		
		asteroidLocalRotation = asteroidLocalRotation * m_asteroids[i].axialSpeed.y;
		SimpleMath::Matrix asteroidLocalRotations = asteroidRotLocalZ * asteroidRotLocalX * asteroidRotLocalY;
		SimpleMath::Matrix asteroidPos = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, m_asteroids[i].initDistance);	
	
		// Revolution
		SimpleMath::Matrix asteroidRevolution = SimpleMath::Matrix::CreateRotationY(m_timer.GetTotalSeconds()/25 * m_asteroids[i].revolutionSpeed);
	
		m_world = asteroidScale * asteroidLocalRotation * asteroidLocalRotations * asteroidPos * m_world * asteroidRevolution;
		
		texture = asteroidTexture;

		// Turn our shaders on, set parameters
		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, texture.Get());
	
		//render our models
		model = m_asteroids[i].model;
		model.Render(context);	
	}
	
	// ADDITIVE STATE FOR BACKGROUND AND EFFECTS
	context->OMSetBlendState(m_states->Additive(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	// SKYBOX
	for (int i =0; i< 2; i++)
	{
		m_world = SimpleMath::Matrix::Identity; //set world back to identity
		SimpleMath::Matrix skyBoxRotation;
		SimpleMath::Matrix skyBoxScale;
		if(i%2==0)
		{
			skyBoxRotation = Matrix::CreateRotationZ(XMConvertToRadians(-50));
			skyBoxRotation *= Matrix::CreateRotationY(m_timer.GetTotalSeconds()/50);
			skyBoxScale = SimpleMath::Matrix::CreateScale(100.0f, 100.0f, 100.0f);
			texture = starsTexture.Get();
		}
		else
		{
			skyBoxRotation = Matrix::CreateRotationZ(XMConvertToRadians(-80));
			skyBoxRotation *= Matrix::CreateRotationY(-m_timer.GetTotalSeconds()/80);
			skyBoxScale = SimpleMath::Matrix::CreateScale(99.0f, 99.0f, 99.0f);
			texture = starsTexture1.Get();
		}
		SimpleMath::Matrix skyBoxTranslation = Matrix::CreateTranslation(m_Camera01.getPosition());
		
		m_world = m_world * skyBoxRotation * skyBoxScale * skyBoxTranslation;
	
		m_BasicShaderPair.EnableShader(context);
		m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, texture.Get());
		m_skyBox.Render(context);
	}

	// PARTICLE SYSTEM
	// m_world = SimpleMath::Matrix::Identity; //set world back to identity
	// SimpleMath::Matrix newPosition3 = SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	// // SimpleMath::Matrix lookAt = Matrix::CreateLookAt(Vector3(0,0,2), m_Camera01.getForward(), m_Camera01.getForward().Cross(m_Camera01.getRight()));
	// SimpleMath::Matrix particleRotation = Matrix::CreateRotationY(m_timer.GetTotalSeconds() * 1);
	// m_world = particleRotation * m_world * newPosition3;
	// m_ParticleSystem.Render(context);
	// m_ParticleSystem.Frame(5, m_deviceResources->GetD3DDeviceContext());
	// m_ParticleShader.Render(context, m_ParticleSystem.GetIndexCount(), &m_world, &m_view, &m_projection, 
	// 				  fxtexture.Get());
	

	// Draw Text to the screen
	m_deviceResources->PIXBeginEvent(L"Draw sprite");
	m_sprites->Begin();
	m_font->DrawString(m_sprites.get(), L"DirectXTK Space Exploration", XMFLOAT2(10, 10), Colors::Yellow);
	m_sprites->End();
	m_deviceResources->PIXEndEvent();
	
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

    context->ClearRenderTargetView(renderTarget, Colors::Black);
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
	Height = height;
	Width = width;

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
	// m_BasicModel.InitializeSphere(device);
	//
	// m_BasicModel2.InitializeModel(device,"Resources/Models/drone.obj");
	// m_BasicModel3.InitializeBox(device, 10.0f, 0.1f, 10.0f);	//box includes dimensions

	m_skyBox.InitializeModel(device, "Resources/Models/skybox.obj");
	m_astronaut.InitializeModel(device, "Resources/Models/astronaut.obj");

	m_planet.InitializeModel(device,"Resources/Models/sun.obj");
	m_saturnRing.InitializeModel(device,"Resources/Models/saturnRing.obj");
	m_rocket.InitializeModel(device,"Resources/Models/rocket1.obj");
	m_asteroid1.InitializeModel(device,"Resources/Models/asteroid1.obj");
	m_asteroid2.InitializeModel(device,"Resources/Models/asteroid2.obj");
	m_asteroid3.InitializeModel(device,"Resources/Models/asteroid3.obj");
	m_asteroid4.InitializeModel(device,"Resources/Models/asteroid4.obj");
	m_asteroid5.InitializeModel(device,"Resources/Models/asteroid5.obj");
	m_asteroid6.InitializeModel(device,"Resources/Models/asteroid6.obj");
	// m_asteroid7.InitializeModel(device,"Resources/Models/asteroid7.obj");
	// m_asteroid8.InitializeModel(device,"Resources/Models/asteroid8.obj");

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");

	// Inititalize 
	m_ParticleShader.InitializeShader(device, L"particle_vs.cso", L"particle_ps.cso");
	
	// Initialize the particle system object.
	m_ParticleSystem.Initialize(device);
	
	//load Textures
	m_fxFactory->CreateTexture(L"Resources/Textures/Grass Albedo.png",context, grassAlbedo.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/Grass Normal.png",context, grassNormal.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/EvilDrone_Diff.png",context, m_texture2.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/starsTexture4.png",context, starsTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/starsTextureClose.png",context, starsTexture1.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/particles.png",context, fxtexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/sun.jpg",context, sunTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/mercury.jpg",context, mercuryTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/venus.jpg",context, venusTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/earth1.jpg",context, earthTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/moon.jpg",context, moonTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/mars.jpg",context, marsTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/jupiter.jpg",context, jupiterTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/saturn.jpg",context, saturnTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/saturnRing.png",context, saturnRingTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/uranus.jpg",context, uranusTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/neptune.jpg",context, neptuneTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/rocket1.png",context, rocketTexture.ReleaseAndGetAddressOf());
	m_fxFactory->CreateTexture(L"Resources/Textures/asteroid.png",context, asteroidTexture.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Resources/Textures/seafloor.dds", nullptr,	m_texture1.ReleaseAndGetAddressOf());
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
