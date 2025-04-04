#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
    // int height(AVLNode<Key, Value>* node);
    // bool isLeftChild(AVLNode<Key, Value>* node); 

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;

    while (curr != nullptr){
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        } else if (new_item.first > curr->getKey()) {
            curr = curr->getRight();
        } else {
            // key exists, just setval. meow ^•.•^
            curr->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key, Value>* newnodeset = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    if (new_item.first < parent->getKey()){
        parent->setLeft(newnodeset);
    }
    else { 
        parent->setRight(newnodeset);
    }

    if (parent->getBalance() != 0) {
        parent->setBalance(0);
    }
    else {
        parent->setBalance((newnodeset == parent->getLeft()) ? -1 : 1);
        insertFix(parent, newnodeset);
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == nullptr) return;

    AVLNode<Key, Value>* toDel = node;
    //AVLNode<Key, Value>* child = nullptr;
    //int diff = 0;

    if (node->getLeft() && node->getRight()) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        this->nodeSwap(node, pred);
    }
    //node has one child atp

    AVLNode<Key, Value>* child = node->getLeft() ? node->getLeft() : node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    int diff = 0;
    if (parent) {
        if (parent->getLeft() == node) diff = 1;
        else diff = -1;
    }

    if (child) child->setParent(parent);

    if (!parent) {
        this->root_ = child;
    } else {
        if (parent->getLeft() == node) {
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
    }

    delete node; 
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node)
{
  if (parent == NULL) return;

  AVLNode<Key, Value>* grandparent = parent->getParent();
  if (grandparent == NULL) return;

  if (parent == grandparent->getLeft())
  {
    grandparent->updateBalance(-1);

    if (grandparent->getBalance() == 0) return;
    else if (grandparent->getBalance() == -1) 
      insertFix(grandparent, parent);
    else if (grandparent->getBalance() == -2)  
    {
      if (node == parent->getLeft())
      {
          rotateRight(grandparent);
          parent->setBalance(0);
          grandparent->setBalance(0);
      }
      else
      {
        rotateLeft(parent);
        rotateRight(grandparent); 

          if (node->getBalance() == -1)
          {
            parent->setBalance(0);
            grandparent->setBalance(1);
          }
          else if (node->getBalance() == 0) 
          {
            parent->setBalance(0); 
            grandparent->setBalance(0);
          }
          else
          {
            parent->setBalance(-1); 
            grandparent->setBalance(0);
          }
          node->setBalance(0);
      }
    }
  }
  else
  {
    grandparent->updateBalance(1);

    if (grandparent->getBalance() == 0) return;
    else if (grandparent->getBalance() == 1) {
        insertFix(grandparent, parent);
    }
    else if (grandparent->getBalance() == 2) {
      if (node == parent->getRight())
      {
        rotateLeft(grandparent);
        parent->setBalance(0);
        grandparent->setBalance(0);
      }
      else {
        rotateRight(parent);
        rotateLeft(grandparent);

        if (node->getBalance() == 1)
        {
          parent->setBalance(0);
          grandparent->setBalance(-1);
        }
        else if (node->getBalance() == 0)
        {
          parent->setBalance(0);
          grandparent->setBalance(0);
        }
        else
        {
          parent->setBalance(1);
          grandparent->setBalance(0);
        }
        node->setBalance(0);
      }
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightChild = node->getRight();
    if (!rightChild) return;

    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft()) {
        rightChild->getLeft()->setParent(node);

    }
    rightChild->setParent(node->getParent());
    if (!node->getParent()) {
      this->root_ = rightChild;
    }
    else if (node == node->getParent()->getLeft()) {
      node->getParent()->setLeft(rightChild);
    }
    else {
      node->getParent()->setRight(rightChild);
    }
    rightChild->setLeft(node);
    node->setParent(rightChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) 
{ 
  AVLNode<Key, Value>* leftChild = node->getLeft();
  if (!leftChild) return;

  node->setLeft(leftChild->getRight());
  if (leftChild->getRight())
    leftChild->getRight()->setParent(node);

  leftChild->setParent(node->getParent());

  if (!node->getParent())
    this->root_ = leftChild; 
  else if (node == node->getParent()->getLeft())
    node->getParent()->setLeft(leftChild);
  else
    node->getParent()->setRight(leftChild);

  leftChild->setRight(node);
  node->setParent(leftChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
  if (!node) return;

  // Calculate new balance
  node->updateBalance(diff);
  
  // Calculate parent and ndiff for recursive call
  AVLNode<Key, Value>* parent = node->getParent();
  int ndiff = 0;
  if (parent) {
      ndiff = (node == parent->getLeft()) ? 1 : -1;
  }
  
  // case 1 balance is 0, go up
  if (node->getBalance() == 0) {
      removeFix(parent, ndiff);
      return;
  }
  
  // case 2 balance is abs 1, stops
  if (node->getBalance() == 1 || node->getBalance() == -1) {
      return;
  }
  
  // case 3 balance is abs 2, rotate
  if (node->getBalance() == 2) {
      AVLNode<Key, Value>* right = node->getRight();
    if (right->getBalance() >= 0) {
        rotateLeft(node);
        if (right->getBalance() == 0) {
            node->setBalance(1);
            right->setBalance(-1);
            return; 
        } else {
            node->setBalance(0);
            right->setBalance(0);
            removeFix(parent, ndiff);
        }
    } else {
        AVLNode<Key, Value>* rightLeft = right->getLeft();
        int rlBalance = rightLeft->getBalance();
        
        rotateRight(right);
        rotateLeft(node);
        
        if (rlBalance == 1) {
            node->setBalance(-1);
            right->setBalance(0);
        } else if (rlBalance == 0) {
            node->setBalance(0);
            right->setBalance(0);
        } else { // rlBalance == -1 
          node->setBalance(0);
          right->setBalance(1); 
        } 
        rightLeft->setBalance(0);
        removeFix(parent, ndiff);
    }
} else if (node->getBalance() == -2) {
    AVLNode<Key, Value>* left = node->getLeft();
    if (left->getBalance() <= 0) {
      rotateRight(node);
        if (left->getBalance() == 0) {
          node->setBalance(-1);
          left->setBalance(1);
          return; // 
        } else {
            node->setBalance(0);
            left->setBalance(0);
            removeFix(parent, ndiff);
        }
    } else {
        AVLNode<Key, Value>* leftRight = left->getRight();
        int lrBalance = leftRight->getBalance();
        
        rotateLeft(left);
        rotateRight(node);
        
        if (lrBalance == -1) {
            node->setBalance(1);
            left->setBalance(0);
        } else if (lrBalance == 0) {
            node->setBalance(0);
            left->setBalance(0);
        } else { // lrBalance == 1
            node->setBalance(0);
            left->setBalance(-1);
        }
        leftRight->setBalance(0);
        removeFix(parent, ndiff);
    }
  }
}

// template<class Key, class Value>
// int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node) {
//     if (node == nullptr) return 0;  
//     return 1 + std::max(height(node->getLeft()), height(node->getRight()));
// }

// template<class Key, class Value>
// bool AVLTree<Key, Value>::isLeftChild(AVLNode<Key, Value>* node) {
//     if (!node || !node->getParent()) return false;
//     return node == node->getParent()->getLeft();
// } 


#endif
