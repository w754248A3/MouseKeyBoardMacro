#pragma once
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <vector>
#ifndef _ACAUTO
#define _ACAUTO

using namespace std;

// 节点结构体
struct Node {
  // 使用 map 存储子节点，key 是数字，value 是子节点在 trie 数组中的下标
  // 如果对性能极其敏感且数字分布稀疏，可以换成 unordered_map
  unordered_map<uint32_t, uint32_t> children;

  // 失败指针：指向当前节点的最长真后缀节点
  uint32_t fail;

  // 存储以当前节点结尾的 序列ID 列表
  // 比如匹配到了 [2,7,4,6,8]，这里就会存它的 ID
  vector<uint32_t> matched_pattern_ids;

  Node() : fail(0) {}
};

class ACAutomaton {
private:
  vector<Node> trie;      // 节点池，下标 0 为根节点
  uint32_t current_state; // 当前搜索到的节点下标

public:
  ACAutomaton() {
    // 初始化根节点，下标为 0
    trie.emplace_back();
    current_state = 0;
  }

  // 1. 插入模式串（构建 Trie 树）
  void insert(const vector<uint32_t> &pattern, uint32_t id) {
    uint32_t node_idx = 0; // 从根节点开始
    for (uint32_t num : pattern) {
      // 如果没有这条路，创建新节点
      if (trie[node_idx].children.find(num) == trie[node_idx].children.end()) {
        trie[node_idx].children[num] = static_cast<uint32_t>(trie.size());
        trie.emplace_back();
      }
      // 走到下一个节点
      node_idx = trie[node_idx].children[num];
    }
    // 在该模式串的末尾节点记录 ID
    trie[node_idx].matched_pattern_ids.push_back(id);
  }

  // 2. 构建失败指针 (核心逻辑)
  void build() {
    queue<uint32_t> q;

    // 先把根节点的所有直接子节点入队，并把它们的 fail 指向根节点
    for (auto &pair : trie[0].children) {
      uint32_t child_idx = pair.second;
      trie[child_idx].fail = 0;
      q.push(child_idx);
    }

    while (!q.empty()) {
      uint32_t u = q.front();
      q.pop();

      // 遍历 u 的所有子节点
      for (auto &pair : trie[u].children) {
        uint32_t key = pair.first; // 路径上的数字
        uint32_t v = pair.second;  // 子节点下标

        // 寻找 v 的 fail 指针：
        // 看 u 的 fail 节点有没有 key 这条路
        uint32_t f = (trie[u].fail);
        while (f != 0 && trie[f].children.find(key) == trie[f].children.end()) {
          f = (trie[f].fail); // 不断回退
        }

        // 如果找到了路，或者退回到了根节点
        if (trie[f].children.find(key) != trie[f].children.end()) {
          trie[v].fail = trie[f].children[key];
        } else {
          trie[v].fail = 0;
        }

        // 【重要】状态传递：
        // 如果 fail
        // 指向的节点也是某个模式串的结尾，那么当前节点肯定也匹配了那个模式串
        // 比如匹配了 [2,7,4,6,8]，那肯定也隐含匹配了 [7,4,6,8]
        // 我们把 fail 节点的匹配结果合并过来
        uint32_t fail_idx = (trie[v].fail);
        if (!trie[fail_idx].matched_pattern_ids.empty()) {
          vector<uint32_t> &fails_matches = trie[fail_idx].matched_pattern_ids;
          trie[v].matched_pattern_ids.insert(trie[v].matched_pattern_ids.end(),
                                             fails_matches.begin(),
                                             fails_matches.end());
        }

        q.push(v);
      }
    }
  }

  // 3. 搜索函数：每传入一个数字，返回匹配到的所有 ID
  const vector<uint32_t>& search_step(uint32_t number) {
    // 如果当前路不通，且不是根节点，就沿着 fail 指针找
    while (current_state != 0 && trie[current_state].children.find(number) ==
                                     trie[current_state].children.end()) {
      current_state = trie[current_state].fail;
    }

    // 如果找到了路，走下去
    if (trie[current_state].children.find(number) !=
        trie[current_state].children.end()) {
      current_state = (trie[current_state].children[number]);
    }
    // 否则保持在根节点 (current_state 已经是 0 或者回退到了 0)

    // 返回当前节点记录的所有匹配 ID
    return trie[current_state].matched_pattern_ids;
  }
};

#endif