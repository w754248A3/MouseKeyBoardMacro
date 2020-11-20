#include <iostream>
#include <string>
#include <array>
#include <bit>
#include <vector>
#include <queue>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN        
#include <windows.h>

#include "../../../include/leikaifeng.h"

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

    RightShift = VK_RSHIFT,
    
    LeftShift = VK_LSHIFT,

    Shift = 16,

    ArrayUp = 0x26,
    
    ArrayDown = 40,
    
    ArrayLeft = 37,
    
    ArrayRight = 39,

    P = 0x50,

    Q = 81,


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

class LinkMap {


    template <typename T, size_t SIZE>
    requires(std::is_default_constructible_v<T>&& std::is_trivially_copy_assignable_v<T>&& SIZE != 0)
        class BufFix {

        constexpr static size_t LENGTH = SIZE * 4;

        constexpr static size_t ONE_BLACK_BYTES_SIZE = sizeof(T) * SIZE;

        constexpr static size_t MOVE_LENGTH = LENGTH - SIZE;

        T m_buffer[LENGTH];

        size_t m_index;

    public:
        BufFix() : m_buffer(), m_index(SIZE) {}

        void Add(T value) {

            m_buffer[m_index] = value;
            m_index++;

            if (m_index == LENGTH) {

                std::memcpy(m_buffer, m_buffer + MOVE_LENGTH, ONE_BLACK_BYTES_SIZE);

                m_index = SIZE;
            }
            else {

            }
        }

        //必须保证std::vector的长度小于SIZE
        bool Cmp(const std::vector<T>& value) {

            auto data = value.data();

            auto data_bytes_size = value.size() * sizeof(T);

            auto buffer = reinterpret_cast<char*>(&m_buffer[m_index]) - data_bytes_size;

            return 0 == std::memcmp(buffer, data, data_bytes_size);
        }
    };

    constexpr static size_t SIZE = 8;

    class Node {
        std::vector<Input> m_key;

        std::vector<INPUT> m_value;

    public:
        Node(const std::vector<Input>& key, const std::vector<INPUT>& value) : m_key(key), m_value(value) {}
    


        auto& GetKey() {
            return m_key;
        }

        auto& GetValue() {
            return m_value;
        }

    };


    std::vector<Node> m_nodes;

    BufFix<Input, SIZE> m_keys;


    
public:
    LinkMap() : m_nodes(), m_keys() {
      
    }

    void Add(const std::vector<Input>& key, const std::vector<INPUT>& value) {
       
        if (key.size() > SIZE) {
            Exit("key item too long");
        }
        
        m_nodes.emplace_back(key, value);
    }

    std::vector<INPUT>* Get(Input key) {

        m_keys.Add(key);


        for (auto& node : m_nodes) {
         
            if (m_keys.Cmp(node.GetKey())) {

                return &node.GetValue();

            }
        }

        return nullptr;
    }
};

auto GetScanCode(VKCode code) {
    auto value = MapVirtualKeyW(static_cast<UINT>(code), MAPVK_VK_TO_VSC);

    if (value == 0) {
        Exit("get scan code error");
    }
    else {
        return value;
    }
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


class Info {

public:
    static auto& GetMouseData() {
        static LinkMap data{};
        return data;
    }

    static auto& GetKeyBoardData() {
        static LinkMap data{};

        return data;
    }

};


void SendMacro(std::vector<INPUT>* item) {
    if (item != nullptr) {
       
        SendInput(static_cast<UINT>(item->size()), item->data(), sizeof(INPUT));
    }
}

void MouseMacro(Input input) {

   decltype(auto) data = Info::GetMouseData();

   SendMacro(data.Get(input));
}


void KeyBoardMacro(Input input) {

    decltype(auto) data = Info::GetKeyBoardData();

    SendMacro(data.Get(input));
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

    if (-1 == GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER))) {
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

void AddMouseData(std::vector<Input> key, std::vector<INPUT> value) {
    Info::GetMouseData().Add(key, value);
}

void AddKeyBoardData(std::vector<Input> key, std::vector<INPUT> value) {
    Info::GetKeyBoardData().Add(key, value);
}

int Start() {
    CreateWindowHandle window{};


    CreateMouseRawInput(window.GetHandle());

    CreateKeyboardRawInput(window.GetHandle());


    std::array<char, 1024> buffer{};

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == WM_INPUT) {

            frowRawInput(buffer, msg.lParam);

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


int main() {
    
    AddMouseData({
        Input{InputFlag::Down, VKCode::MouseMiddle},
        },
        {
            CreateKeyBoardInput(InputFlag::Down, VKCode::H),
            CreateKeyBoardInput(InputFlag::Up, VKCode::H),
            
        });

    AddMouseData({
       Input{InputFlag::Up, VKCode::MouseMiddle},
        },
        {
            CreateKeyBoardInput(InputFlag::Down, VKCode::H),
            CreateKeyBoardInput(InputFlag::Up, VKCode::H),
            CreateKeyBoardInput(InputFlag::Down, VKCode::C),
            CreateKeyBoardInput(InputFlag::Up, VKCode::C),
        });

    
    AddMouseData({
        Input{InputFlag::Down, VKCode::MouseMiddle},
        Input{InputFlag::Down, VKCode::MouseLeft},
        },
        {
            CreateKeyBoardInput(InputFlag::Down, VKCode::R),
            CreateKeyBoardInput(InputFlag::Up, VKCode::R),
        });



    auto down = {
        CreateKeyBoardInput(InputFlag::Down, VKCode::P),
        CreateKeyBoardInput(InputFlag::Down, VKCode::C),
    };

    auto up = {
        CreateKeyBoardInput(InputFlag::Up, VKCode::C),
        CreateKeyBoardInput(InputFlag::Up, VKCode::P),   
    };

    AddKeyBoardData({

        Input{InputFlag::Down, VKCode::ArrayUp},
        Input{InputFlag::Up, VKCode::ArrayUp},
        Input{InputFlag::Down, VKCode::ArrayUp},
       
        },
        down);

    AddKeyBoardData({

       Input{InputFlag::Down, VKCode::ArrayUp},
       Input{InputFlag::Down, VKCode::ArrayUp},
       Input{InputFlag::Up, VKCode::ArrayUp},

        },
        up);


    AddKeyBoardData({

      Input{InputFlag::Down, VKCode::ArrayDown},
      Input{InputFlag::Up, VKCode::ArrayDown},

        },
        up);

    AddKeyBoardData({

      Input{InputFlag::Down, VKCode::ArrayLeft},
      Input{InputFlag::Up, VKCode::ArrayLeft},

        },
        up);

    AddKeyBoardData({

      Input{InputFlag::Down, VKCode::ArrayRight},
      Input{InputFlag::Up, VKCode::ArrayRight},

        },
        up);


    
    return Start();
}