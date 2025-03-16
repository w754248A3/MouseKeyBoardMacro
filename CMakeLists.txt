cmake_minimum_required(VERSION 3.16)

project(MouseKeyBoardMacro CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(MY_DIR_PATH ${PROJECT_SOURCE_DIR})

function(my_set_common_properties target)

    # set_target_properties(${target} PROPERTIES 
    #                         CMAKE_CXX_STANDARD 20
    #                         CMAKE_CXX_STANDARD_REQUIRED ON)
  
    target_compile_definitions(${target} PUBLIC
        _WIN32_WINNT=0x0602
        NTDDI_VERSION=0x06020000
    )
   
    target_compile_options(${target} PUBLIC
        -pedantic-errors
        -Wsign-conversion
        -Woverflow
        -Wconversion
        -Werror
        -Wextra
        #-fconcepts
    )

    target_link_options(${target} PUBLIC
    -static)
   
    target_include_directories(${target} PUBLIC
        ${MY_DIR_PATH}/include
        ${MY_DIR_PATH}/../../CppHttpFileServer/include
    )

    target_link_directories(${target} PUBLIC
    
    )
    


    target_link_libraries(${target} PUBLIC

    )

   

endfunction()


function(my_set_target_out targetname)
    # 设置输出目录
    set_target_properties(${targetname} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${MY_DIR_PATH}/out
    OUTPUT_NAME "${targetname}"
    )
endfunction()


add_executable(qk ./qk.cpp)


my_set_common_properties(qk)


my_set_target_out(qk)


add_executable(ArrayToWSAD ./ArrayToWSAD.cpp)


my_set_common_properties(ArrayToWSAD)


my_set_target_out(ArrayToWSAD)
