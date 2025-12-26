#pragma once

#ifndef _MOUSEKEYBOARDMACRO
#define _MOUSEKEYBOARDMACRO

#include <atomic>
#include <functional>
#include <iostream>
#include <minwindef.h>
#include <string>
#include <cstring>
#include <array>
#include <bit>
#include <thread>
#include <vector>
#include <queue>
#include <algorithm>
#include <span>
#include <winnt.h>

#define WIN32_LEAN_AND_MEAN        
#include <windows.h>

#include "leikaifeng.h"
#include "AcAuto.h"
#include <cstdint>
#include <errhandlingapi.h>

class CreateWindowHandle {

    static void _CreateWindowClass(HINSTANCE moduleHandle, LPCWSTR windowsClassName) {
	    WNDCLASSEXW wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style          = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc    = DefWindowProcW;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = moduleHandle;
        wcex.hIcon          = nullptr;
        wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName   = nullptr;
        wcex.lpszClassName  = windowsClassName;
        wcex.hIconSm        = nullptr;

        if (FALSE == RegisterClassExW(&wcex)) {
            Exit("create window class error");
        }
    }

    static auto _CreateWindow(HINSTANCE moduleHandle, LPCWSTR windowsClassName) {
       
        auto windowsHandle = CreateWindowExW(0L, windowsClassName, L"Window", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, moduleHandle, nullptr);

        if (windowsHandle == FALSE) {
            Exit("create window error");
        }

        return windowsHandle;
    }

    HWND m_windowHandle;

public:
    CreateWindowHandle() {
        auto moduleHandle = static_cast<HINSTANCE>(GetModuleHandleW(nullptr));
      
        WCHAR windowsClassName[] = L"fsdfsrewrwegfdgfd";

        CreateWindowHandle::_CreateWindowClass(moduleHandle, windowsClassName);

        m_windowHandle = CreateWindowHandle::_CreateWindow(moduleHandle, windowsClassName);
    }

    auto GetHandle() {
        return m_windowHandle;
    }
};

void CreateMouseRawInput(HWND handle) {

    RAWINPUTDEVICE rid;

    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;
    rid.hwndTarget = handle;
    
    if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
        Exit("create mouse raw input error");
    }
}

void CreateKeyboardRawInput(HWND handle) {

    RAWINPUTDEVICE rid;

    rid.usUsagePage = 0x01;
    rid.usUsage = 0x06;
    rid.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;
    rid.hwndTarget = handle;

    if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE) {
        Exit("create keyboard raw input error");
    }
}

enum class InputFlag : unsigned char
{
    Down = 0x00,
  
    Up = KEYEVENTF_KEYUP
};


enum class VKCode : unsigned char {

    MouseWheel = 0x00,

    MouseLeft = 0x01,

    MouseRight = 0x02,

    MouseMiddle = 0x04,

    H = 0x48,

    C = 0x43,

    R = 0x52,

    G = 0x47,

    Z = 0x5A,

    X = 0x58,

    RightShift = VK_RSHIFT,
    
    LeftShift = VK_LSHIFT,

    Shift = 16,

    ArrayUp = 0x26,
    
    ArrayDown = 40,
    
    ArrayLeft = 37,
    
    ArrayRight = 39,

    P = 0x50,

    Q = 81,

    N1 = 49,

    N2 = 50,


    W = 0x57,
    S = 0x53,
    A = 0x41,
    D = 0x44,

    J = 0x4A,
    K = 0x4B,
    Alt = VK_MENU,

    LeftAlt = VK_LMENU,
};


class Input {

    static_assert(
        std::endian::native == std::endian::little &&
        std::is_same_v<std::underlying_type_t<InputFlag>, unsigned char> &&
        std::is_same_v<std::underlying_type_t<VKCode>, unsigned char>, "error");
    using NT = uint16_t;

    constexpr static NT OFFSET = sizeof(NT) * 8 / 2;

    NT m_value;

public:
    Input() :m_value(0) {}


    Input(InputFlag flag, VKCode code) {

        auto value = static_cast<NT>(flag);

        value <<= OFFSET;

        value |= static_cast<NT>(code);

        m_value = value;

    }

    NT GetValue() {
        return m_value;
    }
};


class MyMacro {
    using ITEM = std::vector<std::function<void()>>;
    using VS =std::vector<ITEM>;
    
    VS m_vs;

    ACAutomaton m_aam;

public:

    void Add(const std::vector<Input>& key, const ITEM& value) {

        std::vector<uint32_t> b;
        b.reserve(key.size());

        std::ranges::transform(key, std::back_inserter(b),
            [](auto x) {
                return x.GetValue();
            }
        );

        auto id = static_cast<uint32_t>(m_vs.size());
        m_vs.push_back(value);

        
        m_aam.insert(b, id);
    }

    void Complete() {
        m_aam.build();
    }

    void Send(Input key) {

        const auto& index_vs = m_aam.search_step(key.GetValue());


        for (auto& index :index_vs) {
         
            const auto& vs = m_vs[index];

            for (auto& f :vs) {
         
            f();
        }
        }
    }

};


void SendMacro(std::vector<INPUT>& item) {
    
    auto res = SendInput(static_cast<UINT>(item.size()), item.data(), sizeof(INPUT));

    auto err = GetLastError();

    if(res ==0){
        Print("send input error", GetWin32ErrorMessage(err));
    }
}

auto GetScanCode(VKCode code) {
    auto value = MapVirtualKeyW(static_cast<UINT>(code), MAPVK_VK_TO_VSC);

    if (value == 0) {
        Exit("get scan code error");
 
    }
  
    return value;
}



INPUT CreateKeyBoardInput(InputFlag flag, VKCode code) {


    KEYBDINPUT keyInput = {};

    keyInput.wVk = static_cast<WORD>(code);
   
    keyInput.wScan = static_cast<WORD>(GetScanCode(code));

    keyInput.dwFlags = static_cast<DWORD>(flag);


    keyInput.time = 0;
    
    keyInput.dwExtraInfo = 0;
   

    INPUT input = {};

    input.type = INPUT_KEYBOARD;

    input.ki = keyInput;

    return input;

}


INPUT CreateMouseInput(InputFlag flag, VKCode code) {

    MOUSEINPUT mouseInput = {};

    if (code == VKCode::MouseLeft) {
        if (flag == InputFlag::Down)
        {
            mouseInput.dwFlags = MOUSEEVENTF_LEFTDOWN;
        }
        else {
            mouseInput.dwFlags = MOUSEEVENTF_LEFTUP;
        }
    }
    else if (code == VKCode::MouseRight){
        if (flag == InputFlag::Down)
        {
            mouseInput.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        }
        else {
            mouseInput.dwFlags = MOUSEEVENTF_RIGHTUP;
        }
    }
    else{
        Exit("other mouse code can not write");
    }

    INPUT input = {};


    input.type = INPUT_MOUSE;

    input.mi = mouseInput;

    return input;
}

class Info {

public:
    static auto& GetMouseData() {
        static MyMacro data{};
        return data;
    }

    static auto& GetKeyBoardData() {
        static MyMacro data{};

        return data;
    }

};




void MouseMacro(Input input) {

    Info::GetMouseData().Send(input);

}


void KeyBoardMacro(Input input) {

    Info::GetKeyBoardData().Send(input);
}


void KeyboardRawInput(RAWKEYBOARD& data) {
   
   
    if ((data.Flags & RI_KEY_BREAK) == RI_KEY_MAKE) {
       
        
        KeyBoardMacro(Input{ InputFlag::Down, static_cast<VKCode>(data.VKey) });

    }
    else if ((data.Flags & RI_KEY_BREAK) == RI_KEY_BREAK) {
      
        KeyBoardMacro(Input{ InputFlag::Up, static_cast<VKCode>(data.VKey) });

    }
    else {
        
    }
}


void MouseRawInput(RAWMOUSE& data) {
    
    if (data.usButtonFlags == 0) {

    }
    else if (data.usButtonFlags == RI_MOUSE_LEFT_BUTTON_DOWN) {
       
        MouseMacro(Input{ InputFlag::Down, VKCode::MouseLeft });
    }
    else if (data.usButtonFlags == RI_MOUSE_LEFT_BUTTON_UP) {
     
        MouseMacro(Input{ InputFlag::Up, VKCode::MouseLeft });
    }
    else if (data.usButtonFlags == RI_MOUSE_MIDDLE_BUTTON_DOWN) {
        
        MouseMacro(Input{ InputFlag::Down, VKCode::MouseMiddle });
    }
    else if (data.usButtonFlags == RI_MOUSE_MIDDLE_BUTTON_UP) {

        MouseMacro(Input{ InputFlag::Up, VKCode::MouseMiddle });
    }
    else if (data.usButtonFlags == RI_MOUSE_WHEEL) {

        if (data.usButtonData == 120) {

            MouseMacro(Input{ InputFlag::Up, VKCode::MouseWheel });
        }
        else {

            MouseMacro(Input{ InputFlag::Down, VKCode::MouseWheel });
        }
    }
    else if (data.usButtonFlags == RI_MOUSE_RIGHT_BUTTON_DOWN) {
      
        MouseMacro(Input{ InputFlag::Down, VKCode::MouseRight });
    }
    else if (data.usButtonFlags == RI_MOUSE_RIGHT_BUTTON_UP) {
       
        MouseMacro(Input{ InputFlag::Up, VKCode::MouseRight });
    }
    
}

template<UINT SIZE>
void frowRawInput(std::array<char, SIZE>& buffer, LPARAM lParam) {

    UINT dwSize = SIZE;

    if (static_cast<UINT>(-1) == GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER))) {
        Exit("get raw input data error");
    }
   
    auto raw = reinterpret_cast<RAWINPUT*>(buffer.data());

    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        KeyboardRawInput(raw->data.keyboard);
    }
    else if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        MouseRawInput(raw->data.mouse);
    }
    else {
        Exit("other raw input message");
    }
}

void AddMouseData(std::vector<Input> key, std::vector<std::function<void()>> value) {
    Info::GetMouseData().Add(key, value);
}

void AddKeyBoardData(std::vector<Input> key, std::vector<std::function<void()>> value) {
    Info::GetKeyBoardData().Add(key, value);
}

void AddKeyBoardData(std::vector<Input> key, std::vector<INPUT> value) {

    std::function<void()> func = [value=value] ()mutable {
        SendMacro(value);
    };

    Info::GetKeyBoardData().Add(key, {func});
}

void AddMouseData(std::vector<Input> key, std::vector<INPUT> value) {

    std::function<void()> func = [value=value] ()mutable {
        SendMacro(value);
    };

    Info::GetMouseData().Add(key, {func});
}

int Start() {

    Info::GetKeyBoardData().Complete();

    Info::GetMouseData().Complete();

    CreateWindowHandle window{};


    CreateMouseRawInput(window.GetHandle());

    CreateKeyboardRawInput(window.GetHandle());

    constexpr UINT SIZE = 1024;
    std::array<char, SIZE> buffer{};

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_INPUT) {

            frowRawInput<SIZE>(buffer, msg.lParam);

            if (GET_RAWINPUT_CODE_WPARAM(msg.wParam) == RIM_INPUT) {
                DispatchMessage(&msg);
            }
            else {

            }

        }
        else {
            DispatchMessage(&msg);
        }   
    }

    return (int)msg.wParam;
}


#endif