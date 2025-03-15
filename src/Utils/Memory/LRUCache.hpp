#pragma once

#include <unordered_map>
#include <list>

template<typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(std::size_t capacity);

    template<typename F, typename... Args>
    Value getOrInsert(F createValueFunc, Key key, const Args &... args);

    void clear();

private:
    struct CacheValue {
        std::list<Key>::iterator position; // iterator in the list
        Value value; // cached value
    };

    // Capacity of the cache
    size_t capacity;

    // Hash table for quick lookup of cache items
    std::unordered_map<Key, CacheValue> cache;

    // Doubly linked list to maintain the order of usage
    std::list<Key> order;

    // Function to insert a new key-value pair into the cache
    void insert(const Key &key, const Value &value);

    // Function to update the order of usage when an item is accessed
    void updateOrder(Key key);

};

template<typename Key, typename Value>
LRUCache<Key, Value>::LRUCache(std::size_t capacity) : capacity(capacity) {}

template<typename Key, typename Value>
template<typename F, typename... Args>
Value LRUCache<Key, Value>::getOrInsert(F createValueFunc, const Key key, const Args &... args) {
    auto it = cache.find(key);
    if (it != cache.end()) {
        order.splice(order.begin(), order, it->second.position);
        return it->second.value;
    }

    Value value = createValueFunc(args...);

    if (cache.size() >= capacity) {
        Key lruKey = order.back();
        order.pop_back();
        cache.erase(lruKey);
    }

    order.push_front(key);
    cache.emplace(key, CacheValue{order.begin(), std::move(value)});
    return cache[key].value;
}

template<typename Key, typename Value>
void LRUCache<Key, Value>::insert(const Key &key, const Value &value) {
    if (cache.size() >= capacity) {
        Key lruKey = order.back();
        order.pop_back();
        cache.erase(lruKey);
    }

    auto position = order.insert(order.begin(), key);
    cache[key] = {position, value};
}

template<typename Key, typename Value>
void LRUCache<Key, Value>::updateOrder(Key key) {
    order.push_front(key);
}

template<typename Key, typename Value>
void LRUCache<Key, Value>::clear() {
    cache.clear();
    order.clear();
}
