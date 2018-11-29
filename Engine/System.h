#pragma once

// @note : ���� ������� �ʴ� API���� ��� �ִ� Win32 �쵥�� �������� �������� ���� �ӵ��� ���̱� ����
// Pre-Processing Directive
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "input.h"
#include "graphics.h"

class System
{
public:
	System();
	System( const System& sys );
	~System();

	bool Initialize();
	void ShutDown();
	void Run();

	LRESULT CALLBACK MessageHandler( HWND , UINT, WPARAM, LPARAM );

private:
	bool Frame();
	void InitializeWindows( int& screenWidth , int& screenHeight );
	void ShutDownWindows();

private:
	LPCWSTR		m_applicationName;
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;

	Input*		m_pInput;
	Graphics*	m_pGraphics;
};

static LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );

static System* ApplicationHandle = 0;
