#include "stdafx.h"
#include "Input.h"
#define MAX_KEY 256

Input::Input()
{
}

Input::Input( const Input& other )
{
}


Input::~Input()
{
}

void Input::Initialize()
{
	// 모든 키를 눌리지 않는 상태로 만든다.
	for( int i = 0; i < MAX_KEY ; ++i )
	{
		m_keys[i] = false;
	}
}

void Input::KeyDown( unsigned int input )
{
	// 해당 키가 눌렸다
	m_keys[input] = true;
}

void Input::KeyUp( unsigned int input )
{
	m_keys[input] = false;
}

bool Input::IsKeyDown( unsigned int input )
{
	return m_keys[input];
}
