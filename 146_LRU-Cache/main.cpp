#include <iostream>

using namespace std;

typedef struct LRULinkNode {
    int key;
    int value;
    struct LRULinkNode *next;
    struct LRULinkNode *prev;
} LRULinkNodeStru;

class LRUCache {
public:
    void putShow(int key) {
        LRULinkNodeStru *flag = head;

        cout << "Put key: " << key << endl;
        while (flag != nullptr) {
            cout << "key = " << flag->key << ", value = " << flag->value << endl;
            flag = flag->next;
        }
        cout << endl;
    }

    void getShow(int key) {
        LRULinkNodeStru *flag = head;

        cout << "Get key: " << key << endl;
        while (flag != nullptr) {
            cout << "key = " << flag->key << ", value = " << flag->value << endl;
            flag = flag->next;
        }
        cout << endl;
    }

    LRUCache(int capacity) {
        LRUCacheCap = capacity;
        LRUCacheLen = 0;
        head = new(LRULinkNodeStru);
        head->key = 0;
        head->value = 0;
        head->next = nullptr;
        head->prev = nullptr;
        rear = head;
    }

    // 当前的搜索策略是在双端链表的两端向中间搜索，两个移动指针相遇时没有找到
    // 变换策略为直接从unordered_map中查找对于key的结点地址 空间换时间
    int get(int key) {
        LRULinkNodeStru *flag = nullptr;
        LRULinkNodeStru *flagHead = head->next;
        LRULinkNodeStru *flagRear = rear;

        while (true) {
            if (flagHead == nullptr || flagRear == nullptr) {
                break;
            }
            if (flagHead->key == key || flagRear->key == key) {
                flag = flagHead->key == key ? flagHead : flagRear;
                if(flag != rear) {
                    flag->prev->next = flag->next;
                    flag->next->prev = flag->prev;
                    flag->prev->next = flag->next;
                    flag->next = rear->next;
                    rear->next = flag;
                    flag->prev = rear;
                    rear = flag;
                }
                return flag->value;
            }
            if (flagHead == flagRear) {
                break;
            }
            flagHead = flagHead->next;
            flagRear = flagRear->prev;
        }

        return -1;
    }

    void put(int key, int value) {
        LRULinkNodeStru *flag = head;
        LRULinkNodeStru *flagHead = head->next;
        LRULinkNodeStru *flagRear = rear;

        while (true) {
            if (flagHead == nullptr || flagRear == nullptr) {
                break;
            }
            if (flagHead->key == key || flagRear->key == key) {
                flag = flagHead->key == key ? flagHead : flagRear;
                flag->value = value;
                if(flag != rear) {
                    flag->prev->next = flag->next;
                    flag->next->prev = flag->prev;
                    flag->prev->next = flag->next;
                    flag->next = rear->next;
                    rear->next = flag;
                    flag->prev = rear;
                    rear = flag;
                }
                return;
            }
            flagHead = flagHead->next;
            flagRear = flagRear->prev;
        }

        flag = head->next;
//        cout << "key = " << key << ", LRUCacheLen = " << LRUCacheLen << endl;
        if (LRUCacheLen == LRUCacheCap) {
            if(flag != nullptr) {
                if (flag->next != nullptr) {
                    flag->next->prev = flag->prev;
                }
                flag->prev->next = flag->next;
                rear = rear == flag ? flag->prev : rear;
                LRUCacheLen -= 1;
                delete flag;
            }
        }

        LRULinkNodeStru *newNode = nullptr;
        newNode = new(LRULinkNodeStru);
        newNode->key = key;
        newNode->value = value;
        newNode->next = nullptr;
        newNode->prev = rear;
        rear->next = newNode;
        rear = newNode;
        LRUCacheLen = LRUCacheLen == LRUCacheCap ? LRUCacheCap : LRUCacheLen + 1;
//        putShow(key);
        return;
    }

private:
    int LRUCacheCap;
    int LRUCacheLen;
    LRULinkNodeStru *head;
    LRULinkNodeStru *rear;
};

int main() {
    LRUCache *lRUCache = new LRUCache(1);

    lRUCache->put(1, 1); // 缓存是 {1=1}
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
