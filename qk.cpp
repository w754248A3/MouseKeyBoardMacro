
#include "MouseKeyBoardMacro.h"

volatile std::atomic_int g_flag;


void LoopSend(){
    std::vector<INPUT> n1 = {
        CreateMouseInput(InputFlag::Down, VKCode::MouseLeft),
};

std::vector<INPUT> n2 = {
    CreateMouseInput(InputFlag::Up, VKCode::MouseLeft),
};

    while (true) {
        if (g_flag.load(std::memory_order_acquire) == 1) {
           
            SendMacro(n1);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            SendMacro(n2);
        }
        

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

void SetOn(){
    g_flag.store(1, std::memory_order_relaxed);
}

void SetOff(){
    g_flag.store(0, std::memory_order_relaxed);
}


int main() {
    
    SetOff();

    std::thread t(LoopSend);

    bool flag = false;
    bool k_flag_1 = false;

    std::function<void()> end = [&flag, &k_flag_1]() {
        
        if(flag){
            flag = false;
            Print("end");
        }
        else{
            flag = true;
            Print("start");
           
        }

        if(flag && k_flag_1){
            SetOn();
        }
        else{
            SetOff();
        }
   
        
    };


    std::function<void()> kstart = [&k_flag_1]() {
        Print("kstart");
         k_flag_1 = true;
        
    };

    std::function<void()> kend = [&k_flag_1]() {
        Print("kend");
         k_flag_1 = false;
        
         SetOff();
    };


        AddMouseData({
            Input{InputFlag::Up, VKCode::MouseRight},
            },
            {end}
            );

    AddKeyBoardData({
            Input{InputFlag::Up, VKCode::G},
    }, {kstart});

    AddKeyBoardData({
        Input{InputFlag::Up, VKCode::H},
}, {kend});
    
    return Start();
}