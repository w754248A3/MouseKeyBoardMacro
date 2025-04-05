#include "MouseKeyBoardMacro.h"
#include "leikaifeng.h"
#include <boost/json/array.hpp>
#include <boost/json/impl/serialize.ipp>
#include <boost/json/object.hpp>
#include <boost/json/src.hpp>
#include <boost/json/value.hpp>
#include <cstdint>
#include <string_view>
#include <vector>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <ios>
#include <windows.h>

// 定义常量字符串
const std::string_view IS_UP = "isUp";
const std::string_view CODE = "code";
const std::string_view IS_KEY = "isKey";
const std::string_view MATCH = "match";
const std::string_view SEND = "send";
const std::string_view KEY = "key";
const std::string_view MOUSE = "mouse";

std::vector<char> ReadFileBytes(const std::wstring& filePath) {
    // 打开文件并立即定位到文件末尾获取大小
   
    std::ifstream file(filePath.data(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading");
    }

    // 获取文件大小（此时文件指针在末尾）
    const auto fileSize = file.tellg();
    if (fileSize == -true) {
        throw std::runtime_error("Failed to determine file size");
    }

    if(fileSize > 1024*1024){
        throw std::runtime_error("File size is too large");
    }

    // 重置文件指针到文件开头
    file.seekg(false, std::ios::beg);

    // 准备存储容器
    std::vector<char> buffer{};

    buffer.resize((size_t)fileSize);
    // 一次性读取全部内容
    if (!file.read(buffer.data(), fileSize)) {
        throw std::runtime_error("Failed to read file contents");
    }

    return buffer;
}

void WriteFileBytes(const std::wstring& filePath, const char* buffer, long long size) {
    // 以二进制写入模式打开文件（自动清空现有内容）
    std::ofstream file(filePath.data(), std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing");
    }

    // 一次性写入全部数据
    if (!file.write(buffer, size)) {
        throw std::runtime_error("Failed to write file contents");
    }
}


void WriteJson(const std::wstring& filePath, const boost::json::value& obj){
    const auto text = boost::json::serialize(obj);
    WriteFileBytes(filePath, text.data(), (long long )text.size());
}


boost::json::value ReadJson(const std::wstring& filePath){
    const auto buffer = ReadFileBytes(filePath);
    
    const auto text = std::string(buffer.begin(), buffer.end());
    Print(text);
    return boost::json::parse(text);
}

auto GetInput(const boost::json::array& matchArray){
    std::vector<Input> vs{};
    for(const auto& item : matchArray){
        const auto& match = item.as_object();

        const auto& isUp = match.at(IS_UP).as_bool();

        const auto& code = match.at(CODE).as_int64();

        Input input{isUp ? InputFlag::Up : InputFlag::Down, (VKCode)code};

        vs.push_back(input);
    }

    return  vs;
}

auto GetSendInput(const boost::json::array& sendArray){
    std::vector<INPUT> vs{};

    for(const auto& item : sendArray){
        const auto& match = item.as_object();

        const auto& isUp = match.at(IS_UP).as_bool();

        const auto& code = match.at(CODE).as_int64();

        const auto& isKey = match.at(IS_KEY).as_bool();

        const InputFlag flag = isUp ? InputFlag::Up : InputFlag::Down;

        if(isKey){
            vs.push_back(CreateKeyBoardInput(flag, (VKCode)code));
        }else{
            vs.push_back(CreateMouseInput(flag, (VKCode)code));
        }

    }

    return  vs;
}

void parseSetting(const std::wstring& filePath){
    
    const auto obj = ReadJson(filePath).as_object();

    auto key = obj.find(KEY);

    if(key != obj.end()){
        for(const auto& item : key->value().as_array()){

            const auto& obj = item.as_object();

            const auto& match = obj.at(MATCH).as_array();

            const auto& send = obj.at(SEND).as_array();

            AddKeyBoardData(GetInput(match), GetSendInput(send));
        }


    }

    auto mouse = obj.find(MOUSE);

    if(mouse != obj.end()){
        for(const auto& item : mouse->value().as_array()){

            const auto& obj = item.as_object();

            const auto& match = obj.at(MATCH).as_array();

            const auto& send = obj.at(SEND).as_array();

            AddMouseData(GetInput(match), GetSendInput(send));
        }
    }
    



    Start();
}

void createDefSetting(){



    const std::wstring filePath = L"setting.json";

    boost::json::object body{};

    {
        boost::json::array vs{};

        boost::json::object obj{};
       

        boost::json::array match{};

        match.push_back(boost::json::object{ {IS_UP, false}, {CODE, (int)VKCode::X} });
        match.push_back(boost::json::object{ {IS_UP, true}, {CODE, (int)VKCode::X} });
        

        boost::json::array send{};

        send.push_back(boost::json::object{ {IS_KEY, false}, {IS_UP, false}, {CODE, (int)VKCode::MouseRight} });
        send.push_back(boost::json::object{ {IS_KEY, false}, {IS_UP, true}, {CODE, (int)VKCode::MouseRight} });

        obj[MATCH] = match;

        obj[SEND] = send;


        vs.push_back(obj);


        body[KEY] = vs;
    }

    

    {
        boost::json::array vs{};

        boost::json::object obj{};

        

        boost::json::array match{};

        match.push_back(boost::json::object{ {IS_UP, false}, {CODE, (int)VKCode::MouseRight} });
        match.push_back(boost::json::object{ {IS_UP, true}, {CODE, (int)VKCode::MouseRight} });

        boost::json::array send{};


        send.push_back(boost::json::object{ {IS_KEY, true}, {IS_UP, false}, {CODE, (int)VKCode::X} });
        send.push_back(boost::json::object{ {IS_KEY, true}, {IS_UP, true}, {CODE, (int)VKCode::X} });
        obj[MATCH] = match;

        obj[SEND] = send;


        vs.push_back(obj);



        body[MOUSE] = vs;
    }


    WriteJson(filePath, body);

}

   

int main(int argc, char* argv[])
{
    
    
    try {

        if(argc != 2){


            

            createDefSetting();

            Print("need setting.json file path");


            return 0;
        }
        
        

        parseSetting(UTF8::GetWideCharFromMultiByte(argv[1]));

        
    }
    catch (const std::exception& e) {
        Print(e.what());

        return 1;
    }

}
