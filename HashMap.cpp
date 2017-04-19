#include <vector>
#include <utility>
#include <stdexcept>

using std::vector;
using std::pair;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
 public:
    template <typename NodeType>
    class ListNode {
     public:
        ListNode(NodeType data_, ListNode *prev_ = nullptr, ListNode *next_ = nullptr) :
            _data(data_), _prev(prev_), _next(next_) {}

        ListNode() : _data(NodeType()), _prev(nullptr), _next(nullptr) {}

        NodeType& data() {
            return _data;
        }

        ListNode*& prev() {
            return _prev;
        }

        ListNode*& next() {
            return _next;
        }

     private:
        NodeType _data;
        ListNode *_prev, *_next;
    };

    template <typename T>
    class MyIterator {
        ListNode<T>* p;

     public:
        MyIterator() : p(nullptr) {}
        MyIterator(ListNode<T>* p_) : p(p_) {}
        ~MyIterator() {}
        MyIterator& operator++() {
            p = p->next();
            return *this;
        }
        MyIterator operator++(int) {
            MyIterator tmp(*this);
            operator++();
            return tmp;
        }
        MyIterator& operator--() {
            p = p->prev;
            return *this;
        }
        MyIterator operator--(int) {
            MyIterator tmp(*this);
            operator--();
            return tmp;
        }

        bool operator==(const MyIterator& rhs) const { return p == rhs.p; }
        bool operator!=(const MyIterator& rhs) const { return p != rhs.p; }
        const T& operator*() const { return p->data(); }
        T& operator*() { return p->data(); }
        T* operator->() const { return &(p->data()); }
    };

    template <typename NodeType>
    class MyList {
     public:
        MyList() : _first(nullptr), _last(nullptr), _node_number(0) {}

        ~MyList() {
            clear();
        }

        void push_back(NodeType elem) {
            ++_node_number;
            if (_last == nullptr) {
                _first = new ListNode<NodeType>(elem);
                _last = _first;
            } else {
                _last->next() = new ListNode<NodeType>(elem, _last, nullptr);
                _last = _last->next();
            }
        }

        void clear() {
            for (auto i = 0; i < _node_number; ++i) {
                ListNode<NodeType> *current = _first;
                _first = _first->next();
                delete current;
            }
            _node_number = 0;
            _first = _last = nullptr;
        }

        void erase(ListNode<NodeType> *elem) {
            if (elem == nullptr) {
                return;
            }
            ListNode<NodeType> *prev = elem->prev();
            ListNode<NodeType> *next = elem->next();
            if (_first == elem) {
                _first = next;
            }
            if (_last == elem) {
                _last = prev;
            }
            delete elem;
            --_node_number;
            if (_node_number == 0) {
                _first = _last = nullptr;
            }
            if (prev != nullptr) {
                prev->next() = next;
            }
            if (next != nullptr) {
                next->prev() = prev;
            }
        }

        typedef MyIterator<const NodeType> const_iterator;
        typedef MyIterator<NodeType> iterator;


        iterator begin() const {
            return _first;
        }

        iterator end() const {
            return nullptr;
        }

        const_iterator cbegin() const {
            return _first;
        }

        const_iterator cend() const {
            return nullptr;
        }

        ListNode<NodeType>*& first() {
            return _first;
        }

        ListNode<NodeType>* first() const {
            return _first;
        }

        ListNode<NodeType>*& last() {
            return _last;
        }

        ListNode<NodeType>* last() const {
            return _last;
        }

        size_t& node_number() {
            return _node_number;
        }

        size_t node_number() const {
            return _node_number;
        }

     private:
        ListNode<NodeType> *_first;
        ListNode<NodeType> *_last;
        size_t _node_number;
    };

 public:
    HashMap(Hash _hasher = Hash()) : hasher(_hasher) {}

    template<typename Iterator>
    HashMap(Iterator begin, Iterator end, Hash _hasher = Hash()) : hasher(_hasher) {
        for (auto it = begin; it != end; ++it) {
            insert(*it);
        }
    }

    HashMap(const HashMap<KeyType, ValueType, Hash> & Map)
        : HashMap(Map.begin(), Map.end(), Map.hasher) {}

    HashMap & operator=(const HashMap<KeyType, ValueType, Hash> & Map) {
        if (this != &Map) {
            clear();
            for (auto it = Map.begin(); it != Map.end(); ++it) {
                insert(*it);
            }
        }
        return *this;
    }

    HashMap(std::initializer_list<pair<const KeyType, ValueType>> list,
        Hash _hasher = Hash()) : HashMap(list.begin(), list.end(), _hasher) {}

    size_t size() const {
        return element_count;
    }

    bool empty() const {
        return element_count == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    typedef MyIterator<pair<const KeyType, ValueType>> const_iterator;
    typedef MyIterator<pair<const KeyType, ValueType>> iterator;

    void insert(pair<const KeyType, ValueType> element) {
        size_t bucket_id = hasher(element.first) % max_bucket_count;
        for (auto it = hash_table[bucket_id].first(); it != nullptr; it = it->next()) {
            if (it->data()->data().first == element.first) {
                return;
            }
        }
        ++element_count;
        pairs.push_back(element);
        hash_table[bucket_id].push_back(pairs.last());
    }

    void erase(const KeyType key) {
        size_t bucket_id = hasher(key) % max_bucket_count;
        for (auto it = hash_table[bucket_id].first(); it != nullptr; it = it->next()) {
            if (it->data()->data().first == key) {
                auto value = it->data();
                hash_table[bucket_id].erase(it);
                pairs.erase(value);
                --element_count;
                return;
            }
        }
    }

    iterator begin() {
        return pairs.begin();
    }

    iterator end() {
        return pairs.end();
    }

    const_iterator begin() const {
        return pairs.begin();
    }

    const_iterator end() const {
        return pairs.end();
    }

    const_iterator find(const KeyType key) const {
        size_t bucket_id = hasher(key) % max_bucket_count;
        for (auto it = hash_table[bucket_id].first(); it != nullptr; it = it->next()) {
            if (it->data()->data().first == key) {
                return it->data();
            }
        }
        return pairs.end();
    }

    iterator find(const KeyType key) {
        size_t bucket_id = hasher(key) % max_bucket_count;
        for (auto it = hash_table[bucket_id].first(); it != nullptr; it = it->next()) {
            if (it->data()->data().first == key) {
                return it->data();
            }
        }
        return pairs.end();
    }

    ValueType& operator[](const KeyType key) {
        auto it = find(key);
        if (it == end()) {
            insert(std::make_pair(key, ValueType()));
            return pairs.last()->data().second;
        }
        return it->second;
    }

    const ValueType& at(const KeyType key) const {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("Out");
        }
        return it->second;
    }

    void clear() {
        while (element_count != 0) {
            erase(begin()->first);
        }
    }

 private:
    const size_t max_bucket_count = 100000;
    Hash hasher;
    size_t element_count = 0;
    vector<MyList<ListNode<pair<const KeyType, ValueType>>*>> hash_table =
        vector<MyList<ListNode<pair<const KeyType, ValueType>>*>>(max_bucket_count);
    MyList<pair<const KeyType, ValueType>> pairs;
};


