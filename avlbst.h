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
    void rotateLeft(AVLNode<Key,Value>* x);
    void rotateRight(AVLNode<Key,Value>* x);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key,Value>(new_item.first,new_item.second,NULL);
        return;
    }

    AVLNode<Key,Value>* parent = NULL;
    AVLNode<Key,Value>* current = static_cast<AVLNode<Key,Value>*>(this->root_);

    while(current != NULL) {
        parent = current;
        if(new_item.first <current->getKey()) {
            current = current->getLeft();
        }
        else if (new_item.first > current->getKey()) {
            current = current->getRight();
        }
        else {
            current->setValue(new_item.second);
            return;
        }
    }

    AVLNode<Key,Value>* newNode = new AVLNode<Key,Value>(new_item.first,new_item.second,parent);
    if(new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }

    AVLNode<Key,Value>* child = newNode;

    while(parent != NULL) {
        if(child == parent->getLeft()) {
            parent->updateBalance(-1);
        }
        else {
            parent->updateBalance(1);
        }

        int8_t balance = parent->getBalance();

        if(balance == 0 ) {
            break;
        }

        else if (balance == 2) {
            if(child->getBalance() == -1) {
                rotateRight(parent->getRight());
                rotateLeft(parent);
                break;
            }
            else {
                rotateLeft(parent);
                break;
            }
        }
        else if (balance == -2) {
            if(child->getBalance() == 1) {
                rotateLeft(parent->getLeft());
                rotateRight(parent);
                break;
            }
            else {
                rotateRight(parent);
                break;
            }
        }

        child = parent;
        parent = parent->getParent();
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
    AVLNode<Key,Value>* node = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    if(node == NULL) {
        return;
    }

    AVLNode<Key,Value>* rebalanceStart = NULL;

    if(node->getLeft() != NULL && node->getRight() != NULL) {
        AVLNode<Key,Value>* pred = static_cast<AVLNode<Key,Value>*>(this->predecessor(node));
        nodeSwap(node,pred);
    }

    AVLNode<Key,Value>* parent = node->getParent();
    AVLNode<Key,Value>* child = NULL;
    if(node->getLeft() != NULL) {
        child = node->getLeft();
    }
    else {
        child = node->getRight();
    }

    if(child != NULL) {
        child->setParent(parent);
    }
    if(parent == NULL) {
        this->root_ = child;
    }

    else if (parent->getLeft() == node) {
        parent->setLeft(child);
        rebalanceStart = parent;
        parent->updateBalance(1);
    }
    else
    {
        parent->setRight(child);
        rebalanceStart = parent;
        parent->updateBalance(-1);
    }

    delete node;

    /* while(rebalanceStart != NULL) {
        int8_t balance = rebalanceStart->getBalance();

        if(balance == -2) {
            AVLNode<Key,Value>* left = rebalanceStart->getLeft();
            if(left->getBalance() <= 0) {
                rotateRight(rebalanceStart);


            }
            else {
                rotateLeft(rebalanceStart->getLeft());
                rotateRight(rebalanceStart);
            }
        }
        else if (balance == 2) {
            AVLNode<Key,Value>* right = rebalanceStart->getRight();
            if(right->getBalance() >= 0) {
                rotateLeft(rebalanceStart);
            }
            else {
                rotateRight(rebalanceStart->getRight());
                rotateLeft(rebalanceStart);
            }
        }
        else if (balance == 0) {
            rebalanceStart = rebalanceStart->getParent();
        }
        else {
            break;
        }
    
    } */

    while(rebalanceStart != NULL) {
        int8_t balance = rebalanceStart->getBalance();

        if(balance == -2) {
            AVLNode<Key,Value>* left = rebalanceStart->getLeft();
            int8_t leftBalance = 0;
            if(left != NULL) {
                leftBalance = left->getBalance();
            }
            if(leftBalance == 0) {
                rotateRight(rebalanceStart);
                break;
            }
            else if (leftBalance <= 0) {
                rotateRight(rebalanceStart);
                rebalanceStart = rebalanceStart->getParent();
            }
            else {
                rotateLeft(left);
                rotateRight(rebalanceStart);
                rebalanceStart = rebalanceStart->getParent();
            }
        }
        else if (balance == 2) {
            AVLNode<Key,Value>* right = rebalanceStart->getRight();
            int8_t rightBalance = 0;

            if(right != NULL) {
                rightBalance = right->getBalance();
            }

            if(rightBalance == 0) {
                rotateLeft(rebalanceStart);
                break;
            }
            else if (rightBalance >= 0) {
                rotateLeft(rebalanceStart);
                rebalanceStart = rebalanceStart->getParent();
            }

            else {
                rotateRight(right);
                rotateLeft(rebalanceStart);
                rebalanceStart = rebalanceStart->getParent();
            }
        }

        else if (balance == 0) {
            rebalanceStart = rebalanceStart->getParent();
        }

        else {
            break;
        }
    }
}

template<class Key, class Value>
void AVLTree<Key,Value>::rotateLeft(AVLNode<Key,Value>* x) {
    AVLNode<Key,Value>* y = x->getRight();
    AVLNode<Key,Value>* B = y->getLeft();

    y->setLeft(x);
    y->setParent(x->getParent());
    x->setParent(y);
    x->setRight(B);

    if(B != NULL) {
        B->setParent(x);
    }
    if(y->getParent() == NULL) {
        this->root_ = y;
    }
    else if (y->getParent()->getLeft() == x) {
        y->getParent()->setLeft(y);
    }
    else {
        y->getParent()->setRight(y);
    }

    x->setBalance(x->getBalance() - 1 - std::max<int8_t>(0, y->getBalance()));
    y->setBalance(y->getBalance() - 1 + std::min<int8_t>(0, x->getBalance()));
}

template<class Key, class Value>
void AVLTree<Key,Value>::rotateRight(AVLNode<Key,Value>* x) {
    AVLNode<Key,Value>* y = x->getLeft();
    AVLNode<Key,Value>* B = y->getRight();

    y->setRight(x);
    y->setParent(x->getParent());
    x->setParent(y);
    x->setLeft(B);

    if(B != NULL) {
        B->setParent(x);
    }
    if(y->getParent() == NULL) {
        this->root_ = y;
    }
    else if (y->getParent()->getLeft() == x) {
        y->getParent()->setLeft(y);
    }
    else {
        y->getParent()->setRight(y);
    }

    x->setBalance(x->getBalance() + 1 - std::min<int8_t>(0, y->getBalance()));
    y->setBalance(y->getBalance() + 1 + std::max<int8_t>(0, x->getBalance()));
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
