#include "pch.h"
#include "Input.h"


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetVisible(false);
	m_mouse->SetWindow(window);
	m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	m_quitApp = false;

	m_GameInput.pause		= false;
	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.up			= false;
	m_GameInput.down		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;
	m_GameInput.boost		= false;
	m_GameInput.rotRight	= false;
	m_GameInput.rotLeft		= false;
	m_GameInput.camera1		= true;
	// m_GameInput.camera2		= false;
	m_GameInput.mouseX		= 0.f;
	m_GameInput.mouseY		= 0.f;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	// float screenRatio = static_cast<float>(mouse.y) / static_cast<float>(Height);
	
	m_GameInput.mouseX = mouse.x;	
	// m_GameInput.mouseY = -1 + screenRatio * 2;
	m_GameInput.mouseY = mouse.y;
	
	
	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;
	
	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.forward	 = true;
	else		m_GameInput.forward = false;

	//S key
	if (kb.S)	m_GameInput.back = true;
	else		m_GameInput.back = false;

	//Q key
	if (kb.Q)	m_GameInput.down = true;
	else		m_GameInput.down = false;

	//E key
	if (kb.E)	m_GameInput.up = true;
	else		m_GameInput.up = false;

	if(m_KeyboardTracker.IsKeyPressed(DirectX::Keyboard::LeftShift))
	{
		m_GameInput.boost = true;
	}
	else if(m_KeyboardTracker.IsKeyReleased(DirectX::Keyboard::LeftShift))
	{
		m_GameInput.boost = false;
	}

	//C key released
	// if (m_KeyboardTracker.released.C)	m_GameInput.camera2 = !m_GameInput.camera2;
	// else								m_GameInput.camera1 = !m_GameInput.camera1;

	if(m_KeyboardTracker.released.P)
	{
		m_GameInput.pause = true;
	}
	else
	{
		m_GameInput.pause = false;
	}
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
