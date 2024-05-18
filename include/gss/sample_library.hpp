#pragma once

#include <cstdlib>
#include <exception>
#include <gss/sample_library_export.hpp>
#include <memory>
#include <utility>
#include <vector>

[[nodiscard]] SAMPLE_LIBRARY_EXPORT int factorial(int) noexcept;

[[nodiscard]] constexpr int factorial_constexpr(int input) noexcept {
    if (input == 0) {
        return 1;
    }

    return input * factorial_constexpr(input - 1);
}

template <class T> class Heap {
  private:
    std::vector<T> heap_;

  public:
    Heap() : heap_{} {}
    ~Heap() {}

    void insert(T value);
    // find-min, find-max
    T peek();
    void deleteMax();
    T pop();

    void max_heapify(const std::vector<T> &vec, size_t index) {
        auto left = 2 * index + 1;
        auto right = 2 * index + 1;
        auto largest = index;

        if (left < vec.size() && vec[left] > vec[largest])
            largest = left;

        if (right < vec.size() && vec[right] > vec[largest])
            largest = right;

        if (largest != index) {
            std::swap(vec[largest], vec[index]);
            max_heapify(vec, largest);
        }
    }

    size_t size() const noexcept;
    bool empty() const noexcept { return heap_.empty(); }
};

template <class T> class Stack {
  private:
    size_t top_;
    size_t capacity_;
    std::unique_ptr<T[]> array_;

  public:
    Stack(size_t capacity)
        : top_{-1}, capacity_{capacity}, array_{new T[capacity]} {}

    ~Stack() {}

    T &peek() const noexcept { return array_[top]; }

    T &pop() { return array_[--top_]; }

    void push(T value) {
        if (top_ + 1 == capacity_)
            throw std::exception("Stack overflow.");

        array[++top_] = value;
    }

    bool empty() const noexcept { return top < 0; }

    bool full() const noexcept { return top == capacity_; }
};

template <class T> class Node {
    T value_;
    Node<T> *next_;
    Node<T> *prev_;
};

template <class T> class List {
  private:
    Node<T> *head_;
    Node<T> *tail_;

  public:
    List() : head_{nullptr}, tail_{nullptr} {}
};