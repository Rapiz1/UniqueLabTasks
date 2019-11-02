#ifndef UNIQUE_SET_H_
#define UNIQUE_SET_H_
#include <algorithm>
#include <iterator>
#ifdef DBG
#include "assert.h"
#endif
namespace llrb {
typedef unsigned int size_type;
template <typename T>
class Set;
template <typename T>
class Iterator;
template <typename T>
class Node {
  T value_;
  bool is_red_;
  Node* ch_[2];
  Node* parent_;
  bool IsRightLeaning() const;
  bool IsMultipleRed() const;
  bool IsRightChild() const;
  bool Is4Node() const;
  void Detach();
  void Attach(Node* parent, bool on_right);
  Node<T>* Rotate();
  void ColorFlip();

 public:
  friend class Set<T>;
  friend class Iterator<T>;
  Node(const T& value, bool is_red);
  ~Node();
  bool is_red() const;
};
template <typename T>
class Iterator {
  const Node<T>* node_;
  const Set<T>* set_;
  void Forward() {
    node_ = set_->Successor(node_);
  }
  void Backward() {
    if (!node_) {
      node_ = set_->root_;
      while (node_->ch_[1]) node_ = node_->ch_[1];
    }
    else node_ = set_->Predecessor(node_);
  }
 public:
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef T value_type;
  typedef Node<T>* pointer;
  typedef Node<T>& reference;
  Iterator(const Set<T>* set = nullptr, const Node<T>* node = nullptr) : set_(set), node_(node){};
  T operator*() const { return node_->value_; }
  bool operator==(const Iterator<T>& rhs) const {
    return node_ == rhs.node_ && set_ == rhs.set_;
  }
  bool operator!=(const Iterator<T>& rhs) const {
    return !(node_ == rhs.node_ && set_ == rhs.set_);
  }
  Iterator& operator++() {
    Forward();
    return *this;
  }
  Iterator operator++(int) {
    const Node<T>* ret = node_;
    Forward();
    return Iterator(set_, ret);
  }
  Iterator& operator--() {
    Backward();
    return *this;
  }
  Iterator operator--(int) {
    const Node<T>* ret = node_;
    Backward();
    return Iterator(set_, ret);
  }

};
template <typename T>
class Set {
  int size_;
  Node<T>* root_;
  Node<T>* InsertNode(Node<T>* node, T value);
  Node<T>* DeleteNode(Node<T>* node, T value);
  Node<T>* MoveRedRight(Node<T>* node);
  Node<T>* MoveRedLeft(Node<T>* node);
  //Node<T>* DeleteMax(Node<T>* node);
  Node<T>* DeleteMin(Node<T>* node);
  const Node<T>* Successor(const Node<T>* node) const;
  const Node<T>* Predecessor(const Node<T>* node) const;
  Node<T>* max(Node<T>* node) const;
  Node<T>* min(Node<T>* node) const;
  Node<T>* Fix(Node<T>* node);

 public:
  typedef Iterator<T> iterator;
  friend class Iterator<T>;
  Set();
  ~Set();
  iterator begin() const;
  iterator end() const;
  bool empty() const;
  size_type size() const;
  void clear();
  void insert(const T& value);
  size_type erase(const T& value);
  size_type count(const T& value);
  iterator lower_bound (const T& value) const;
  iterator upper_bound (const T& value) const;
  iterator find(const T& value)const;
};
template <typename T>
Node<T>::Node(const T& value, bool is_red) {
  value_ = value;
  is_red_ = is_red;
  parent_ = ch_[0] = ch_[1] = nullptr;
}
template <typename T>
Node<T>::~Node() {
  delete ch_[0];
  delete ch_[1];
}
template <typename T>
bool Node<T>::IsRightLeaning() const {
  return ch_[1] && ch_[1]->is_red_;
}
template <typename T>
bool Node<T>::IsMultipleRed() const {
  return ch_[0] && ch_[0]->is_red_ && ch_[0]->ch_[0] && ch_[0]->ch_[0]->is_red_;
}
template <typename T>
bool Node<T>::Is4Node() const {
  return ch_[0] && ch_[1] && ch_[0]->is_red_ && ch_[1]->is_red_;
}
template <typename T>
void Node<T>::Detach() {
  if (parent_) {
    parent_->ch_[IsRightChild()] = nullptr;
    parent_ = nullptr;
  }
}
template <typename T>
void Node<T>::Attach(Node* parent, bool on_right) {
  parent_ = parent;
  if (parent_) parent_->ch_[on_right] = this;
}
template <typename T>
bool Node<T>::IsRightChild() const {
  if (parent_)
    return parent_->ch_[1] == this;
  else
    return 0;
}
template <typename T>
void Node<T>::ColorFlip() {
  is_red_ ^= 1;
  if (ch_[0]) ch_[0]->is_red_ ^= 1;
  if (ch_[1]) ch_[1]->is_red_ ^= 1;
}
template <typename T>
Node<T>* Node<T>::Rotate() {
  int pos = IsRightChild();
  Node *child = ch_[pos ^ 1], *parent = parent_;
  Detach();
  if (child) {
    child->Detach();
    child->Attach(parent, pos);
  }
  is_red_ = parent->is_red_;
  parent->is_red_ = true;
  Node* ancestor = parent->parent_;
  int parent_pos = parent->IsRightChild();
  parent->Detach();
  parent->Attach(this, pos ^ 1);
  if (ancestor) Attach(ancestor, parent_pos);
  return this;
}

template <typename T>
Set<T>::Set() {
  root_ = nullptr;
  size_ = 0;
}
template <typename T>
Set<T>::~Set() {
  delete root_;
}
template <typename T>
Iterator<T> Set<T>::begin() const {
  Node<T>* node = root_;
  while (node && node->ch_[0]) node = node->ch_[0];
  return Iterator<T>(this, node);
}
template <typename T>
Iterator<T> Set<T>::end() const {
  return Iterator<T>(this, nullptr);
}
template <typename T>
Node<T>* Set<T>::MoveRedRight(Node<T>* node) {
  node->ColorFlip();
  if (node->ch_[0] && node->ch_[0]->ch_[0] && node->ch_[0]->ch_[0]->is_red_) {
    node = node->ch_[0]->Rotate();
    node->ColorFlip();
  }
  return node;
}
template <typename T>
Node<T>* Set<T>::MoveRedLeft(Node<T>* node) {
  node->ColorFlip();
  if (node->ch_[1] && node->ch_[1]->ch_[0] && node->ch_[1]->ch_[0]->is_red_) {
    node->ch_[1] = node->ch_[1]->ch_[0]->Rotate();
    node = node->ch_[1]->Rotate();
    node->ColorFlip();
  }
  return node;
}
template <typename T>
Node<T>* Set<T>::Fix(Node<T>* node) {
  if (node->IsRightLeaning()) node = node->ch_[1]->Rotate();
  if (node->IsMultipleRed()) node = node->ch_[0]->Rotate();
  if (node->Is4Node()) node->ColorFlip();
  return node;
}
/*
template <typename T>
Node<T>* Set<T>::DeleteMax(Node<T>*& node) {
  if (node->ch_[0] && node->ch_[0].is_red_)
    node = node->ch_[0]->Rotate();
  if (!node->ch_[1]) {
    delete node;
    return node = nullptr;
  }
  if (!(node->ch_[1] && node->ch_[1]->is_red_) && !(node->ch_[1] &&
node->ch_[1]->ch_[0] && node->ch_[1]->ch_[0]->is_red_)) node =
MoveRedRight(node); node->ch_[1] = DeleteMax(node->ch_[1]);

  Fix(node);
  return node;
}
*/
template <typename T>
Node<T>* Set<T>::DeleteMin(Node<T>* node) {
  if (!node->ch_[0]) {
    delete node;
    return node = nullptr;
  }
  if (!(node->ch_[0] && node->ch_[0]->is_red_) &&
      !(node->ch_[0] && node->ch_[0]->ch_[0] && node->ch_[0]->ch_[0]->is_red_))
    node = MoveRedLeft(node);

  node->ch_[0] = DeleteMin(node->ch_[0]);

  return Fix(node);
}
template <typename T>
Node<T>* Set<T>::max(Node<T>* node)const {
  while (node->ch_[1]) node = node->ch_[1];
  return node;
}
template <typename T>
Node<T>* Set<T>::min(Node<T>* node)const {
  while (node->ch_[0]) node = node->ch_[0];
  return node;
}
template <typename T>
const Node<T>* Set<T>::Successor(const Node<T>* node) const {
  if (node->ch_[1]) {
    node = node->ch_[1];
    while (node->ch_[0]) node = node->ch_[0];
    return node;
  }
  else {
    while (node->parent_ && node->IsRightChild()) node = node->parent_;
    return node->parent_;
  }
}
template <typename T>
const Node<T>* Set<T>::Predecessor(const Node<T>* node) const {
  if (node->ch_[0]) {
    node = node->ch_[0];
    while (node->ch_[1]) node = node->ch_[1];
    return node;
  }
  else {
    while (node->parent_ && !node->IsRightChild()) node = node->parent_;
    return node->parent_;
  }
}
template <typename T>
Node<T>* Set<T>::DeleteNode(Node<T>* node, T value) {
  if (value < node->value_) {
    if (!(node->ch_[0] && node->ch_[0]->is_red_) &&
        !(node->ch_[0] && node->ch_[0]->ch_[0] &&
          node->ch_[0]->ch_[0]->is_red_))
      node = MoveRedLeft(node);
    node->ch_[0] = DeleteNode(node->ch_[0], value);
  } else {
    if (node->ch_[0] && node->ch_[0]->is_red_) node = node->ch_[0]->Rotate();
    if (value == node->value_ && !node->ch_[1]) {
      delete node;
      return nullptr;
    }
    if (!(node->ch_[1] && node->ch_[1]->is_red_) &&
        !(node->ch_[1] && node->ch_[1]->ch_[0] &&
          node->ch_[1]->ch_[0]->is_red_))
      node = MoveRedRight(node);
    if (value == node->value_) {
      node->value_ =
          min(node->ch_[1])->value_;  // Predecessor(node->ch_[1])->value_;
      node->ch_[1] = DeleteMin(node->ch_[1]);
    } else
      node->ch_[1] = DeleteNode(node->ch_[1], value);
  }
  return Fix(node);
}
template <typename T>
Node<T>* Set<T>::InsertNode(Node<T>* node, T value) {
  if (node == nullptr) {
    return node = new Node<T>(value, true);
  }

  if (node->value_ == value) return nullptr;
  Node<T>* child = InsertNode(node->ch_[value > node->value_], value);
  if (child != nullptr) child->Attach(node, value > node->value_);

  if (node->IsRightLeaning() && !(node->ch_[0] && node->ch_[0]->is_red_))
    node = node->ch_[1]->Rotate();
  if (node->IsMultipleRed()) node = node->ch_[0]->Rotate();
  if (node->Is4Node()) node->ColorFlip();
  return node;
}

template <typename T>
inline size_type Set<T>::size() const {
  return size_;
}
template <typename T>
inline bool Set<T>::empty() const {
  return !size();
}
template <typename T>
void Set<T>::clear() {
  delete root_;
  root_ = nullptr;
  size_ = 0;
}
template <typename T>
void Set<T>::insert(const T& value) {
  if (count(value)) return;
  root_ = InsertNode(root_, value);

  root_->is_red_ = false;
  size_++;
}
template <typename T>
size_type Set<T>::count(const T& value) {
  Node<T>* p = root_;
  while (p) {
    if (p->value_ == value) return 1;
    p = p->ch_[value > p->value_];
  }
  return 0;
}
template <typename T>
size_type Set<T>::erase(const T& value) {
  if (!count(value)) return 0;
  if (!(root_->ch_[0] && root_->ch_[0]->is_red_) &&
      !(root_->ch_[1] && root_->ch_[1]->is_red_))
    root_->is_red_ = true;
  root_ = DeleteNode(root_, value);
  size_--;
  if (!empty()) root_->is_red_ = false;
  return 1;
}
template<typename T>
Iterator<T> Set<T>::lower_bound (const T& value) const {
  /*
  Node<T>* node = root_;
  while (node) {
    if (value < node->value_) {
      if (node->ch_[0]) node = node->ch_[0];
      else return iterator(this, node);
    }
    else if (value > node->value_) {
      if (node->ch_[1]) node = node->ch_[1];
      else return iterator(this, Successor(node));
    }
    else
      return iterator(this, node);
  }
  return end();
  */
  Node<T> *node = root_, *ret = nullptr;
  while (node) {
    if (value <= node->value_) {
      ret = node;
      node = node->ch_[0];
    }
    else if (value > node->value_) {
      node = node->ch_[1];
    }
  }
  return iterator(this, ret);
}
template<typename T>
Iterator<T> Set<T>::upper_bound (const T& value) const {
  Node<T> *node = root_, *ret = nullptr;
  while (node) {
    if (value < node->value_) {
      ret = node;
      node = node->ch_[0];
    }
    else if (value >= node->value_) {
      node = node->ch_[1];
    }
  }
  return iterator(this, ret);
}
template<typename T>
Iterator<T> Set<T>::find(const T& value)const {
  Node<T>* node = root_;
  while (node) {
    if (value == node->value_) break;
    else node = node->ch_[value > node->value_];
  }
  return iterator(this, node); 
}

}
namespace uni {
using llrb::Set;
}
#endif
