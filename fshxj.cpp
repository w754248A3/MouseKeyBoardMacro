#include "MouseKeyBoardMacro.h"
#include "leikaifeng.h"
#include <vector>
#include <windows.h>



int main(void){


     

    AddKeyBoardData({
        Input{InputFlag::Down, VKCode::Z}
    }, {
        CreateMouseInput(InputFlag::Down, VKCode::MouseLeft)
    });


    AddKeyBoardData({
        Input{InputFlag::Up, VKCode::Z}
    }, {
        CreateMouseInput(InputFlag::Up, VKCode::MouseLeft),
    });

    std::vector<INPUT> mrdown = { CreateMouseInput(InputFlag::Down, VKCode::MouseRight)};

    std::vector<INPUT> mrup = { CreateMouseInput(InputFlag::Up, VKCode::MouseRight)};



    AddKeyBoardData({
        Input{InputFlag::Down, VKCode::X}
    }, {
        [&mrdown]()->void{
            Print("x down");

           

            SendMacro(mrdown);
        }
    });


    AddKeyBoardData({
        Input{InputFlag::Up, VKCode::X}
    }, {
        [&mrup]()->void{
            Print("x up");
            SendMacro(mrup);
        }
    });


    AddMouseData({
        Input{InputFlag::Down, VKCode::MouseRight}
    }, {
        []()->void{
            Print("mouse right down");
        }
    });

    AddMouseData({
        Input{InputFlag::Up, VKCode::MouseRight}
    }, {
        []()->void{
            Print("mouse right up");
        }
    });

    return Start();

}