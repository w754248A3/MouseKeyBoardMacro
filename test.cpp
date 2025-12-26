#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <unordered_map>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include "AcAuto.h"
using namespace std;

// ================== 测试部分 ==================

int main() {
    // 1. 准备序列组 A
    vector<vector<uint32_t>> patterns = {
        {1,2,3,4,5,6,7,8,9}, // ID: 0
        {1,2,1,2},
        {1,2},     // ID: 1
        {3,4,5},        // ID: 2
        {5,6,7,8}     ,
        {4,5},
        {7,8}    // ID: 3
    };

    ACAutomaton ac;

    // 2. 插入所有模式串
    cout << "构建 AC 自动机..." << endl;
    for (uint32_t i = 0; i < patterns.size(); ++i) {
        ac.insert(patterns[i], i);
    }
    
    // 3. 构建 Fail 指针
    ac.build();

    // 4. 模拟数列 B 的流式增长
    vector<uint32_t> stream_B = {1,2,1,2,3,4,5,1,2,3,4,5,6,7,8,9};
    
    cout << "\n开始流式匹配:" << endl;
    for (uint32_t i = 0; i < stream_B.size(); ++i) {
        uint32_t num = stream_B[i];
        
        // 每进来一个数字，调用一次 search_step
        vector<uint32_t> matches = ac.search_step(num);
        
        // 输出结果
        if (!matches.empty()) {
            cout << "B[" << i << "] = " << num << " 时，匹配成功! 包含序列: " << endl;
            for (uint32_t id : matches) {
                cout << "  -> 序列 ID " << id << ": [ ";
                for(uint32_t x : patterns[(size_t)id]) cout << x << " ";
                cout << "]" << endl;
            }
        }
    }

    return 0;
}