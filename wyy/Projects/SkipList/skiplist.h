#include <ctime>
#include <cstdlib>

template <typename K>
struct Less {
    bool operator()(const K& a, const K& b) const {
        return a < b;
    }
};

template <typename K, typename V, typename Comp = Less<K>>
class SkipList {
private:
    struct SkipListNode {
        int level;
        const K key;
        V value;
        SkipListNode** forward;

        SkipListNode() : key{0}, value{0}, forward{nullptr} {

        }

        SkipListNode(K k, V v, int level_, SkipListNode* next_ = nullptr)
            : key{k}, value{v}, level(level_) {
            forward = new SkipListNode* [level + 1];
            for (int i = 0; i <= level; ++i) forward[i] = next_;
        }

        ~SkipListNode() {
            delete [] forward;
        }
    };
    using node = SkipListNode;

    void init() {
        srand((uint32_t)time(nullptr));
        level = length = 0;
        head->forward = new node* [MAXL + 1];
        for (int i = 0; i <= MAXL; i++)
            head->forward[i] = tail;
    }

    int randomLevel() {
        int lev = 1;
        while((rand() & S) < PS)
            ++lev;
        // std::min(lev, MAXL);

        return MAXL > lev ? lev : MAXL;
    }

    int level;
    int length;
    static const int MAXL = 32;
    static const int P = 4;
    static const int S = 0xFFFF;
    static const int PS = S / P;
    node* head, *tail;
    Comp less;

    node* find(const K& key, node** update) {
        node* p = head;
        for (int i = level; i >= 0; i--) {
            while (p->forward[i] != tail && less(p->forward[i]->key, key))
                p = p->forward[i];
            update[i] = p;
        }
        p = p->forward[0];
        return p;
    }

public:
    struct Iter {
        node *p;
        Iter() : p(nullptr) {};
        Iter(node *rhs) : p(rhs) {};

        node* operator ->() const {return (p);}
        node* operator *() const {return *p;}
        bool operator == (const Iter& rhs) { return rhs.p == p;}
        bool operator != (const Iter& rhs) {return !(rhs.p == p);}
        void operator ++() {p = p->forward[0];}
        void operator ++(int) { p = p->forward[0]; }
    };

    SkipList() : head(new node()), tail(new node()), less{} {
        init();
    }
    SkipList(Comp _less) : head{new node()}, tail{new node()},  less{_less} {
        init();
    }
    void insert(const K& key, const V& value) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if(p->key == key) {
            p->value = value;
            return ;
        }
        int lv = randomLevel();
        if(lv > level) {
            lv = ++level;
            update[lv] = head;
        }
        node* newNode = new node(key, value, lv);
        for (int i = lv; i >= 0; i--) {
            p = update[i];
            newNode->forward[i] = p->forward[i];
            p->forward[i] = newNode;
        }
        ++length;
    }

    bool earse(const K& key) {
        node *update[MAXL + 1];
        node *p = find(key, update);
        if (p->key != key) {
            return false;
        }
        for (int i = 0; i <= p->level; i++)
            update[i]->forward[i] = p->forward[i];
        delete p;
        while(level && head->forward[level] == tail)
            --level;
        --length;
        return true;
    }

    Iter find(const K& key) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if(p == tail || p->key != key)
            return tail;
        return Iter(p);
    }
    bool count(const K& key) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if (p == tail)return false;
        return key == p->key;
    }
    Iter end() {
        return Iter(tail);
    }   
    Iter begin() {
        return Iter(head->forward[0]);
    }
};