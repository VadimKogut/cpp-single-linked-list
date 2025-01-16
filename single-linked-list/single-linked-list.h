#pragma once

// добавьте неоходимые include-директивы сюда

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <stdexcept>
#include <algorithm>
#include <iostream>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {}
        Type value;
        Node* next_node = nullptr;
    };

public:
    SingleLinkedList() = default;

    explicit SingleLinkedList(std::initializer_list<Type> values)  {
        for (auto it = values.end(); it != values.begin(); ) {
            --it;  // Двигаем итератор назад
            PushFront(*it); // Добавляем элементы в обратном порядке
        }
    }
    SingleLinkedList(const SingleLinkedList& other) {
        Node* current = other.head_.next_node;
        Node** insert_position = &head_.next_node;

        while (current) {
            *insert_position = new Node(current->value, nullptr);
            insert_position = &((*insert_position)->next_node);
            current = current->next_node;
        }
        size_ = other.size_;
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList temp(rhs);
            swap(temp);
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    ~SingleLinkedList() {
        Clear();
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* temp = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete temp;
        }
        size_ = 0;
    }

    template <typename ValueType>
    class BasicIterator {
        Node* node_ = nullptr;

        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_(node) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        template <typename OtherType>
        BasicIterator(const BasicIterator<OtherType>& other) noexcept : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator temp(*this);
            ++(*this);
            return temp;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }
    };

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return cbegin();
    }

    [[nodiscard]] Iterator end() noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator(nullptr);
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return begin();
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return end();
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));  // Используем const_cast для получения ConstIterator
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return before_begin();  // Просто вызываем before_begin для получения итератора
    }



    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator(new_node);
    }

    void PopFront() noexcept {
        assert(head_.next_node != nullptr); 
        Node* temp = head_.next_node; 
        head_.next_node = head_.next_node->next_node; 
        delete temp; 
        --size_; 
        
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr );
        assert(pos.node_->next_node != nullptr );

        Node* temp = pos.node_->next_node;
        pos.node_->next_node = temp->next_node;
        delete temp;
        --size_;
        return Iterator(pos.node_->next_node);
    }

private:
    Node head_;
    size_t size_ = 0;
};

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    auto it1 = lhs.begin();
    auto it2 = rhs.begin();
    while (it1 != lhs.end() && it2 != rhs.end()) {
        if (*it1 != *it2) {
            return false;
        }
        ++it1;
        ++it2;
    }
    return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}

template<typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) {
    lhs.swap(rhs);
}

// внешние функции разместите здесь
