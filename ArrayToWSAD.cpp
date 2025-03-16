#include "MouseKeyBoardMacro.h"



int main(void){


     

    AddKeyBoardData({
        Input{InputFlag::Down, VKCode::ArrayUp}
    }, {
        CreateKeyBoardInput(InputFlag::Down, VKCode::W)
    });


    AddKeyBoardData({
        Input{InputFlag::Up, VKCode::ArrayUp}
    }, {
        CreateKeyBoardInput(InputFlag::Up, VKCode::W),
    });


    AddKeyBoardData({
        Input{InputFlag::Down, VKCode::ArrayDown}
    }, {
        CreateKeyBoardInput(InputFlag::Down, VKCode::S)
    });


    AddKeyBoardData({
        Input{InputFlag::Up, VKCode::ArrayDown}
    }, {
        CreateKeyBoardInput(InputFlag::Up, VKCode::S)
    });

    AddKeyBoardData({
        Input{InputFlag::Down, VKCode::ArrayLeft}
    }, {
        CreateKeyBoardInput(InputFlag::Down, VKCode::A)
    });

    AddKeyBoardData({
        Input{InputFlag::Up, VKCode::ArrayLeft}
    }, {
        CreateKeyBoardInput(InputFlag::Up, VKCode::A)
    });

    AddKeyBoardData({
        Input{InputFlag::Down, VKCode::ArrayRight}
    }, {
        CreateKeyBoardInput(InputFlag::Down, VKCode::D)
    });

    AddKeyBoardData({
        Input{InputFlag::Up, VKCode::ArrayRight}
    }, {
        CreateKeyBoardInput(InputFlag::Up, VKCode::D)
    });



    AddMouseData({
        Input{InputFlag::Down, VKCode::MouseRight}
    }, {
        CreateKeyBoardInput(InputFlag::Down, VKCode::LeftAlt)
    });

    AddMouseData({
        Input{InputFlag::Up, VKCode::MouseRight}
    }, {
        CreateKeyBoardInput(InputFlag::Up, VKCode::LeftAlt)
    });

    return Start();

}