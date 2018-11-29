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
	// ��� Ű�� ������ �ʴ� ���·� �����.
	for( int i = 0; i < MAX_KEY ; ++i )
	{
		m_keys[i] = false;
	}
}

void Input::KeyDown( unsigned int input )
{
	// �ش� Ű�� ���ȴ�
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
