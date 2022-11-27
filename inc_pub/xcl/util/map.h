#pragma once

#include <xcl/util/rb_tree.h>

namespace xcl
{
    template <typename Key, typename Value>
    struct MapData {
        Key key;
        Value value;

        MapData(const Key& k, const Value& v) : key(k), value(v)
        {}
        MapData(Key&& k, Value&& v)
            : key(static_cast<Key&&>(k)), value(static_cast<Value&&>(v))
        {}
        MapData(const MapData& md) : key(md.key), value(md.value)
        {}
        MapData(MapData&& md)
            : key(static_cast<Key&&>(md.key)),
              value(static_cast<Value&&>(md.value))
        {}
    };

    template <typename Pointer, typename Reference>
    class MapIter final
    {
    public:
        MapIter(RbTree* tree, RbNode* node);

        MapIter(const MapIter&);

        MapIter(MapIter&&);

        ~MapIter();

    public:
        MapIter
        operator++(int);

        MapIter&
        operator++();

        MapIter
        operator--(int);

        MapIter&
        operator--();

        Pointer
        operator->() const;

        Reference
        operator*() const;

        bool
        operator==(const MapIter&) const;

        bool
        operator!=(const MapIter&) const;

    private:
        RbNode* current_;
        RbTree* const tree_;
    };

    template <typename Key, typename Value, typename Comparator>
    class Map
    {
    public:
        typedef MapIter<MapData<Key, Value>*, MapData<Key, Value>&> Iter;
        typedef MapIter<const MapData<Key, Value>*, const MapData<Key, Value>&>
            ConstIter;

    public:
        Map();
        Map(const Map& map);
        Map(Map&& map);

    public:
        Value&
        operator[](const Key& key);

        Map&
        operator=(const Map& map);

        Map&
        operator=(Map&& map);

    public:
        Iter
        add(const Key& key, const Value& value);

        Iter
        add(Iter pos, const Key& key, const Value& value);

        Iter
        emplace(const Key& key, Value&& value);

        Iter
        emplace(Iter pos, const Key& key, Value&& value);

        Iter
        remove(const Key& key);

        Iter
        remove(Iter pos);

        Iter
        find(const Key& key);

        int32_t
        size() const;

        bool
        empty() const;

        void
        clear();

        Iter
        begin();

        Iter
        end();

        ConstIter
        start() const;

        ConstIter
        finish() const;

    private:
        RbTree tree_obj_;
        Comparator comp_;
    };

    template <typename Key, typename Value, typename Comparator>
    Map<Key, Value, Comparator>::Map() : tree_obj_(), comp_()
    {
        __RbTree_init(&tree_obj_, nullptr);
    }
} // namespace xcl
