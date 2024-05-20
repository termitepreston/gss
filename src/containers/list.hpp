#pragma once

#include <cassert>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>

template <class T> struct node {
    T value_;
    node<T> *next_;
    node<T> *prev_;

    node(T value) : next_{nullptr}, prev_{nullptr}, value_{value} {}
};

template <class T> class list {
  private:
    node<T> *head_;
    node<T> *tail_;

  public:
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = node<T> *;
        using reference = T &;

        reference operator*() const { return node_ptr_->value_; }
        pointer operator->() { return &node_ptr_->value_; }

        iterator &operator++() {
            node_ptr_ = node_ptr_->next_;
            return *this;
        }
        iterator &operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator &a, const iterator &b) {
            return a.node_ptr_ == b.node_ptr_;
        }
        friend bool operator!=(const iterator &a, const iterator &b) {
            return a.node_ptr_ != b.node_ptr_;
        }

        iterator(node<T> *node_ptr) : node_ptr_{node_ptr} {}

      private:
        pointer node_ptr_;
    };

    list();

    list(list<T> &other);

    list<T> &operator=(list<T> &&other);

    ~list();

    size_t size() const noexcept;
    bool empty() const noexcept;

    void erase_all();

    void push_back(T value);

    void push_front(T value);

    void pop_back();
    T &back();

    void pop_front();
    T &front();

    iterator begin() { return iterator(head_); }
    iterator end() { return iterator(nullptr); }

    std::string print();
};

template <class T> list<T>::list() : head_{nullptr}, tail_{nullptr} {}

template <class T>
list<T>::list(list<T> &other) : head_{nullptr}, tail_{nullptr} {

    for (T elem : other)
        push_back(elem);
}

template <class T> list<T> &list<T>::operator=(list<T> &&other) {
    head_ = other.head_;
    tail_ = other.tail_;

    other.head_ = other.tail_ = nullptr;
    return *this;
}

template <class T> list<T>::~list() { erase_all(); }

template <class T> void list<T>::erase_all() {
    if (!head_)
        return;

    while (head_ != tail_) {
        auto old_tail = tail_;
        tail_ = tail_->prev_;

        delete old_tail;
    }

    delete head_;
    head_ = tail_ = nullptr;
}

template <class T> void list<T>::push_back(T value) {
    // both cases require the creation of of a new node.
    auto new_node = new node<T>(value);

    // if we have an empty list...
    if (!head_) {
        head_ = tail_ = new_node;
        return;
    }

    assert(!head_->prev_ && !tail_->next_);

    auto old_tail = tail_;
    tail_ = new_node;
    tail_->prev_ = old_tail;
    old_tail->next_ = tail_;
}

template <class T> void list<T>::push_front(T value) {
    // both cases require the creation of a new node.
    auto new_node = new node<T>(value);

    // we have an empty list.
    // !head should imply !tail
    if (!head_) {
        head_ = tail_ = new_node;
        return;
    }

    // we have atleast one element.
    assert(!head_->prev_ && !tail_->next_);

    auto old_head = head_;
    head_ = new_node;
    head_->next_ = old_head;
    old_head->prev_ = head_;
}

template <class T> void list<T>::pop_back() {

    // first check that the list is not empty.
    if (!head_)
        throw std::out_of_range{"Stack underflow."};

    // check if the list only has one element.
    if (tail_ == head_) {

        delete head_;
        head_ = tail_ = nullptr;
        return;
    }

    auto old_tail = tail_;
    tail_ = tail_->prev_;
    tail_->next_ = nullptr;

    delete old_tail;
}

template <class T> T &list<T>::back() {
    if (!tail_)
        throw std::out_of_range{"Empty list"};

    return tail_->value_;
}

template <class T> void list<T>::pop_front() {
    // first check that the list is not empty.
    if (!head_)
        throw std::out_of_range{"Empty list."};

    // check if the list only has one element.
    if (tail_ == head_) {

        delete head_;
        head_ = tail_ = nullptr;
        return;
    }

    auto old_head = head_;
    head_ = head_->next_;
    head_->prev_ = nullptr;

    delete old_head;
}

template <class T> T &list<T>::front() {
    if (!head_)
        throw std::out_of_range{"Empty list."};

    return head_->value_;
}

template <class T> std::string list<T>::print() {
    std::stringstream ss;

    if (!head_) {
        return "";
    }

    for (node<T> *node = head_; node != nullptr; node = node->next_) {
        ss << node->value_ << (node == tail_ ? "" : "->");
    }

    return ss.str();
}

template <class T> size_t list<T>::size() const noexcept {

    size_t count = 0;
    for (node<T> *n = head_; n != nullptr; n = n->next_, count++)
        ;

    return count;
}

template <class T> bool list<T>::empty() const noexcept { return size() == 0; }