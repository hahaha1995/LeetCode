#include <iostream>
#include <unordered_map>

using namespace std;

typedef struct LRULinkNode {
    int key;
    int value;
    struct LRULinkNode *next;
    struct LRULinkNode *prev;
} LRULinkNodeStru;

typedef struct DoubleLinklist {
    int LRUCacheCap; // LRU 容量
    int LRUCacheLen; // LRU 当前长度
    LRULinkNodeStru *head; // LRU 头结点
    LRULinkNodeStru *rear; // LRU 尾结点
} LRUStru;

class LRUCache {
public:
    LRULinkNodeStru* makeNewNode(int key, int value) {
        auto newNode = new LRULinkNodeStru();
        newNode->key = key;
        newNode->value = value;
        newNode->next = LRU.rear->next;
        newNode->prev = LRU.rear;
        LRU.rear->next = newNode;
        LRU.rear = newNode;

        return newNode;
    }

    LRUCache(int capacity) {
        LRU.LRUCacheCap = capacity;
        LRU.LRUCacheLen = 0;
        LRU.head = new LRULinkNodeStru();
        LRU.head->key = 0;
        LRU.head->value = 0;
        LRU.head->next = nullptr;
        LRU.head->prev = nullptr;
        LRU.rear = LRU.head;
    }

    void updateLatestModNode(LRULinkNodeStru *LRUNode) {
        if (LRUNode != LRU.rear) {
            LRUNode->prev->next = LRUNode->next;
            LRUNode->next->prev = LRUNode->prev;
            LRUNode->next = nullptr;
            LRUNode->prev = LRU.rear;
            LRU.rear->next = LRUNode;
            LRU.rear = LRUNode;
        }
    }

    // 当前的搜索策略是在双端链表的两端向中间搜索，两个移动指针相遇时没有找到
    // 变换策略为直接从unordered_map中查找对于key的结点地址 空间换时间
    int get(int key) {
        auto LRUNodeIt = LRUKeyToNodeAddrMap.find(key);
        if (LRUNodeIt != LRUKeyToNodeAddrMap.end()) {
            auto LRUNode = LRUNodeIt->second;
            auto ret = LRUNode->value;

            /* 获取结点的值后，还要更新对应结点在LRU中的位置 map不用动
             * 1、如果结点已经是尾结点，那就是最新的，不需要变更位置
             * 2、如果结点非尾结点，需要移动结点至LRU尾部，同时变更LRU rear标志位 */
            updateLatestModNode(LRUNode);

            cout << "get pair: <" << key << ", " << ret << ">" << endl;
            return ret;
        }
        cout << "get pair: <" << key << ", " << -1 << ">" << endl;
        return -1;
    }

    void put(int key, int value) {
        auto LRUNodeIt = LRUKeyToNodeAddrMap.find(key);

        /* 查找对于key是否存在，若存在就更新value值以及结点位置
         * 若不存在，判断LRU 长度是否到达容量
         * 1、小于容量就新插入结点同时插入map
         * 2、 等于容量就删除head后第一个元素同时删除map中的映射关系，再新插入结点同时插入map */
        if (LRUNodeIt != LRUKeyToNodeAddrMap.end()) {
            auto LRUNode = LRUNodeIt->second;
            LRUNode->value = value;
            updateLatestModNode(LRUNode);
            return;
        }

        if (LRU.LRUCacheLen == LRU.LRUCacheCap) {
            auto temp = LRU.head->next;
            auto keyTmp = temp->key;
            if (LRU.LRUCacheCap == 1) {
                LRU.rear = LRU.head;

                temp->prev = nullptr;
                LRU.head->next = nullptr;
            } else {
                LRU.head->next = temp->next;
                temp->next->prev = LRU.head;

                temp->prev = nullptr;
                temp->next = nullptr;
            }
            delete temp;
            LRU.LRUCacheLen--;

            LRUKeyToNodeAddrMap.erase(keyTmp);
        }

        auto newNode = makeNewNode(key, value);
        LRU.LRUCacheLen++;
        LRUKeyToNodeAddrMap.insert(make_pair(key, newNode));
        cout << "put success: <" << key << ", " << value << ">" << endl;
        return;
    }

private:
    LRUStru LRU; // LRU 缓存结构
    unordered_map<int, LRULinkNodeStru *> LRUKeyToNodeAddrMap; // 记录key和对应LRU结点地址的结构
};

int main() {
    LRUCache *lRUCache = new LRUCache(2);

    lRUCache->put(1, 0); // 缓存是 {1=1}
    lRUCache->put(2, 2); // 缓存是 {1=1, 2=2}
    cout << lRUCache->get(1) << ", ";    // 返回 1
    lRUCache->put(3, 3); // 该操作会使得关键字 2 作废，缓存是 {1=1, 3=3}
    cout << lRUCache->get(2) << ", ";    // 返回 -1 (未找到)
    lRUCache->put(4, 4); // 该操作会使得关键字 1 作废，缓存是 {4=4, 3=3}
    cout << lRUCache->get(1) << ", ";    // 返回 -1 (未找到)
    cout << lRUCache->get(3) << ", ";    // 返回 3
    cout << lRUCache->get(4) << endl;;    // 返回 4
    return 0;
}
