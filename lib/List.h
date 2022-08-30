// list

#pragma once

#include <iostream>
#include <exception>

#include "Allocator.h"
#include "Iterator.h"


template <class T, class _Alloc = _Allocator<T>>
class List {
public:
    class Node {
    public:
        T _data;
        Node *_prev;
        Node *_next;
        Node(): _prev{NULL}, _next{NULL} { }
        Node(const T& data, Node* prev = NULL, Node* next = NULL): 
            _data{data}, _prev{prev}, _next{next} { }
        // don't allow copy constructor
        Node(const Node& n) = delete;
    };

    // iterators
    typedef _Iterator<Node> Iterator;
    typedef _Iterator<const Node> ConstIterator;

protected:
    Node *_head;
    size_t _size;

public:
    // default
    List(): _head{new Node()}, _size{0} { }
    // from size
    List(const size_t& n, const T& value): _head{new Node()}, _size{n} {
        // set a pseudo head
        Node *p = _head;
        for(int i = 0; i < n; ++i) {
            p->_next = new Node(value, p);
            p = p->_next;
        }
        // set head.prev = NULL
        _head->_next->_prev = NULL;
    }
    // from list
    List(std::initializer_list<T> l): _head{new Node()}, _size{l.size()} {
        Node *p = _head;
        for(auto it = l.begin(); it != l.end(); ++it) {
            _head->_next = new Node(*it, p);
            p = p->_next;
        }
        _head->_next->_prev = NULL;
    }
    // copy
    List(const List& l): _head{new Node()}, _size{l._size} {
        Node *p = _head, *q = l._head;
        while(q != NULL) {
            p->_next = new Node(q->_data, p);
            p = p->_next;
            q = q->_next;
        }
        _head->_next->_prev = NULL;
    }

    ~List() {
        if(_head != NULL) {
            while(_head->_next != NULL) {
                delete _head->_prev;
                _head = _head->_next;
            }
            delete _head;
        }
        _size = 0;
    }

    // size
    size_t size() const { return _size; }

    // head
    const Node* head() const { return _head->_next; }
};
