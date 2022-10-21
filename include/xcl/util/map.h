#pragma once

#include <xcl/util/rb_tree.h>

namespace xcl
{
    template <typename Key, typename Value, typename Comparator>
    class Map
    {
        class MapIter;
        class MapConstIter;

    public:
        typedef MapIter Iterator;
        typedef MapConstIter ConstIterator;

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
        Iterator
        add(const Key& key, const Value& value);

        Iterator
        add(Iterator pos, const Key& key, const Value& value);

        Iterator
        emplace(const Key& key, Value&& value);

        Iterator
        emplace(Iterator pos, const Key& key, Value&& value);

        Iterator
        remove(const Key& key);

        Iterator
        remove(Iterator pos);

        Iterator
        find(const Key& key);

        int32_t
        size() const;

        bool
        empty() const;

        void
        clear();

        Iterator
        begin();

        Iterator
        end();

        ConstIterator
        cBegin() const;

        ConstIterator
        cEnd() const;

        void
        destroy();

    private:
        RbTree tree_;
        Comparator comparator_;
    };

    template <typename Key, typename Value, typename Comparator>
    Map<Key, Value, Comparator>::Map() : tree_(), comparator_()
    {
        __RbTree_init(&tree_);
    }
} // namespace xcl
