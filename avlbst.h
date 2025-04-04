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
    AVLNode<Key, Value>* avlInsert(AVLNode<Key, Value>* node, const std::pair<const Key, Value>& new_item, bool &heightIncreased);
    AVLNode<Key, Value>* avlRemove(AVLNode<Key, Value>* node, const Key& key, bool &heightDecreased);
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* x);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* y);
    AVLNode<Key, Value>* rebalance(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    bool heightIncreased = false;
    this->root_ = avlInsert(static_cast<AVLNode<Key,Value>*>(this->root_),new_item,heightIncreased);
    if(this->root_ != NULL) this->root_->setParent(NULL);
}
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::avlInsert(AVLNode<Key, Value>* node,const std::pair<const Key, Value>& new_item, bool &heightIncreased)
{
    if(node == NULL) {
         heightIncreased = true;
         return new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    }
    if(new_item.first < node->getKey()) {
         bool childHeightIncreased = false;
         AVLNode<Key, Value>* leftChild = avlInsert(static_cast<AVLNode<Key, Value>*>(node->getLeft()), new_item, childHeightIncreased);
         node->setLeft(leftChild);
         if(leftChild != NULL) leftChild->setParent(node);
         if(childHeightIncreased) {
             node->updateBalance(1);
             if(node->getBalance() == 0) heightIncreased = false;
             else if(node->getBalance() == 1) heightIncreased = true;
             else if(node->getBalance() == 2) {
                 node = rebalance(node);
                 heightIncreased = false;
             }
         } 
	 else heightIncreased = false;
    } else if(new_item.first > node->getKey()) {
         bool childHeightIncreased = false;
         AVLNode<Key, Value>* rightChild = avlInsert(static_cast<AVLNode<Key, Value>*>(node->getRight()), new_item, childHeightIncreased);
         node->setRight(rightChild);
         if(rightChild != NULL) rightChild->setParent(node);
         if(childHeightIncreased) {
             node->updateBalance(-1);
             if(node->getBalance() == 0) heightIncreased = false;
             else if(node->getBalance() == -1) heightIncreased = true;
             else if(node->getBalance() == -2) {
                 node = rebalance(node);
                 heightIncreased = false;
             }
         } 
	 else heightIncreased = false;
    } 
    else {
         // Key already exists; update value.
         node->setValue(new_item.second);
         heightIncreased = false;
    }
    return node;
}


template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = static_cast<AVLNode<Key, Value>*>(x->getRight());
    x->setRight(y->getLeft());
    if(y->getLeft() != NULL) y->getLeft()->setParent(x);
    y->setParent(x->getParent());
    if(x->getParent() == NULL) {
         // x was root; caller will update the tree's root.
    } 
    else if(x == x->getParent()->getLeft()) x->getParent()->setLeft(y);
    else x->getParent()->setRight(y);
    y->setLeft(x);
    x->setParent(y);
    int8_t b = y->getBalance();
    x->setBalance(x->getBalance() + 1 - std::min(b, (int8_t)0));
    y->setBalance(y->getBalance() + 1 + std::max(x->getBalance(), (int8_t)0));
    return y;
}

/*
 * Rotate the subtree rooted at y to the right.
 */
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y)
{
    AVLNode<Key, Value>* x = static_cast<AVLNode<Key, Value>*>(y->getLeft());
    y->setLeft(x->getRight());
    if(x->getRight() != NULL) x->getRight()->setParent(y);
    x->setParent(y->getParent());
    if(y->getParent() == NULL) {
         // y was root.
    } 
    else if(y == y->getParent()->getRight()) y->getParent()->setRight(x);
    else y->getParent()->setLeft(x);
    x->setRight(y);
    y->setParent(x);
    int8_t b = x->getBalance();
    y->setBalance(y->getBalance() - 1 - std::max(b, (int8_t)0));
    x->setBalance(x->getBalance() - 1 + std::min(y->getBalance(), (int8_t)0));
    return x;
}

/*
 * Rebalance the subtree rooted at node if its balance factor is ±2.
 */
template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    if(node->getBalance() == 2) {
         if(static_cast<AVLNode<Key,Value>*>(node->getLeft())->getBalance() < 0) node->setLeft(rotateLeft(static_cast<AVLNode<Key,Value>*>(node->getLeft())));
         return rotateRight(node);
    }
    else if(node->getBalance() == -2) {
         if(static_cast<AVLNode<Key,Value>*>(node->getRight())->getBalance() > 0) node->setRight(rotateRight(static_cast<AVLNode<Key,Value>*>(node->getRight())));
         return rotateLeft(node);
    }
    return node;
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    bool heightDecreased = false;
    this->root_ = avlRemove(static_cast<AVLNode<Key,Value>*>(this->root_),key,heightDecreased);
    if(this->root_ != NULL) this->root_->setParent(NULL);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::avlRemove(AVLNode<Key, Value>* node, const Key& key, bool &heightDecreased)
{
    if(node == NULL) {
         heightDecreased = false;
         return NULL;
    }
    if(key < node->getKey()) {
         bool childDecreased = false;
         AVLNode<Key, Value>* leftChild = avlRemove(static_cast<AVLNode<Key,Value>*>(node->getLeft()), key, childDecreased);
         node->setLeft(leftChild);
         if(leftChild != NULL) leftChild->setParent(node);
         if(childDecreased) node->updateBalance(-1);
         heightDecreased = (node->getBalance() == 0);
    }
    else if(key > node->getKey()) {
         bool childDecreased = false;
         AVLNode<Key, Value>* rightChild = avlRemove(static_cast<AVLNode<Key,Value>*>(node->getRight()), key, childDecreased);
         node->setRight(rightChild);
         if(rightChild != NULL) rightChild->setParent(node);
         if(childDecreased) node->updateBalance(1);
         heightDecreased = (node->getBalance() == 0);
    }
    else {
         if(node->getLeft() != NULL && node->getRight() != NULL) {
              AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(node));
	      std::pair<const Key, Value> temp = pred->getItem();
	      const_cast<Key&>(node->getKey()) = temp.first;
	      node->setValue(temp.second);
	      bool childDecreased = false;
	      node->setLeft(avlRemove(static_cast<AVLNode<Key,Value>*>(node->getLeft()),temp.first,childDecreased));
	      if(node->getLeft()!=NULL) node->getLeft()->setParent(node);
         }
	 else {
	      AVLNode<Key, Value>* child = (node->getLeft() != NULL) ? static_cast<AVLNode<Key, Value>*>(node->getLeft()) : static_cast<AVLNode<Key, Value>*>(node->getRight());

 	      delete node;
              heightDecreased = true;
              return child;
	 }
    }
    if(node == NULL) return node;
    if(node->getBalance() == 2 || node->getBalance() == -2) {
         node = rebalance(node);
         heightDecreased = (node->getBalance() == 0);
    }
    return node;
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
