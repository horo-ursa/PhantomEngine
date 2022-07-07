#include "stdafx.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    // create a "Hello World" message box using MessageBox()
    MessageBox(NULL,
        L"Hello World!",
        L"Just another Hello World program!",
        MB_ICONINFORMATION | MB_OK);

    // return 0 to Windows
    return 0;
};