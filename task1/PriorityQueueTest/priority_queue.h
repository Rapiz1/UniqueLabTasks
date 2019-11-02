#ifndef UNIQUE_FIB_HEAP_H
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#ifdef DBG
#include "assert.h"
#endif

#define UNIQUE_FIB_HEAP_H
namespace fib_heap {
typedef unsigned int size_type;
template <typename T>
class PriorityQueue;
template <typename T>
class Node {
  T value_;
  int degree_;
  bool mark_;
  Node *left_, *right_, *parent_, *child_;
  void Concatenate(Node* node);
  void Detach();
  void Attach(Node* parent);

 public:
  friend class PriorityQueue<T>;
  Node(const T& value);
  ~Node();
};
template <typename T>
class PriorityQueue {
  Node<T>* top_;
  size_type size_;
  void Consolidate();

 public:
  PriorityQueue();
  ~PriorityQueue();
  bool empty() const;
  size_type size() const;
  T top() const;
  void push(const T& value);
  void pop();
};

template <typename T>
Node<T>::Node(const T& value) {
  value_ = value;
  degree_ = 0;
  mark_ = false;
  left_ = right_ = this;
  child_ = parent_ = nullptr;
}
template <typename T>
Node<T>::~Node() {
  if (!child_) return;
  for (Node<T>* p = child_->right_; p != child_; p = p->right_) delete p;
  delete child_;
}
template <typename T>
void Node<T>::Detach() {
  if (parent_) {
    parent_->degree_--;
    if (parent_->child_ == this)
      parent_->child_ = this == this->right_ ? nullptr : this->right_;
  }
  left_->right_ = right_;
  right_->left_ = left_;
  parent_ = nullptr;
  left_ = right_ = this;
}
template <typename T>
void Node<T>::Attach(Node<T>* parent) {
  mark_ = false;
  parent->degree_++;
  if (parent->child_) {
    Concatenate(parent->child_);
  }
  else {
    parent_ = parent;
    parent_->child_ = this;
  }
}
template <typename T>
void Node<T>::Concatenate(Node<T>* node) {
  Node<T>*right = right_, *left = node->left_;
  right_ = node;
  node->left_ = this;
  right->left_ = left;
  left->right_ = right;
}

template <typename T>
PriorityQueue<T>::PriorityQueue() {
  top_ = nullptr;
  size_ = 0;
}
template <typename T>
PriorityQueue<T>::~PriorityQueue() {
  while (!empty())
    pop();
}
template <typename T>
inline size_type PriorityQueue<T>::size() const {
  return size_;
}
template <typename T>
inline bool PriorityQueue<T>::empty() const {
  return !size();
}
template <typename T>
inline T PriorityQueue<T>::top() const {
  return top_->value_;
}
template <typename T>
void PriorityQueue<T>::push(const T& value) {
  Node<T>* node = new Node<T>(value);
  if (top_) {
    node->Concatenate(top_);
    if (value > top()) top_ = node;
  } else
    top_ = node;
  size_++;
}
template <typename T>
void PriorityQueue<T>::Consolidate() {
  Node<T>*p = top_, *start = top_;
  std::vector<Node<T>* > rt_list;
  do {
    if (p->value_ > top_->value_) top_ = p;
    rt_list.push_back(p);
    p = p->right_;
  } while (p != start);
  int a_size = log2(size_) + 10;
  std::vector<Node<T>*> a(a_size,nullptr);
  #ifdef DBG
  assert(top_->degree_ < a_size);
  #endif
  for (auto p : rt_list) {
    Node<T>* x = p;
    int d = x->degree_;
    while (a[d] != nullptr) {
      Node<T>* y = a[d];
      if (x->value_ < y->value_) std::swap(x, y);
      y->Detach();
      y->Attach(x);
      a[d] = nullptr;
      d++;
    }
    a[d] = x;
  }
  top_->Detach();
  for (int i= 0; i < a_size; i++) {
    if (!a[i]) continue;
    if (a[i] == top_) continue;
    a[i]->Detach();
    a[i]->Concatenate(top_);
  }
}
template <typename T>
void PriorityQueue<T>::pop() {
  if (empty()) {
    std::cerr << "What's wrong with you?\n";
    return;
  }
  if (size_ > 1) {
    Node<T>* child = top_->child_;
    if (child) {
      top_->child_ = nullptr;
      child->parent_ = nullptr;
      child->Concatenate(top_);
    }
    Node<T>* p = top_->right_;
    #ifdef DBG
    assert(p != top_);
    #endif
    top_->Detach();
    delete top_;
    top_ = p;
    Consolidate();
  } else {
    delete top_;
    top_ = nullptr;
  }
  size_--;
}
}  // namespace fib_heap
namespace uni {
using fib_heap::PriorityQueue;
}
#endif
