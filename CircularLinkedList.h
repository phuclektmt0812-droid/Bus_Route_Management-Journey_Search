#ifndef CIRCULARLINKEDLIST_H
#define CIRCULARLINKEDLIST_H

#include "IList.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

template<class T>
class CircularLinkedList : public IList<T> {
public:
    class Node;

protected:
    Node* head;
    Node* tail;
    int count;
    bool (*itemEqual)(T& lhs, T& rhs);
    void (*deleteUserData)(CircularLinkedList<T>*);

public:
    CircularLinkedList(
        void (*deleteUserData)(CircularLinkedList<T>*) = 0,
        bool (*itemEqual)(T&, T&) = 0
    )
        : head(nullptr), tail(nullptr), count(0),
          itemEqual(itemEqual), deleteUserData(deleteUserData) {}

    CircularLinkedList(const CircularLinkedList<T>& list)
        : head(nullptr), tail(nullptr), count(0),
          itemEqual(list.itemEqual), deleteUserData(list.deleteUserData) {
        copyFrom(list);
    }

    CircularLinkedList<T>& operator=(const CircularLinkedList<T>& list) {
        if (this == &list) return *this;
        removeInternalData();
        itemEqual = list.itemEqual;
        deleteUserData = list.deleteUserData;
        copyFrom(list);
        return *this;
    }

    ~CircularLinkedList() {
        removeInternalData();
    }

    void add(T e) override {
        // TODO Q2
        Node* newNode = new Node(e);
        if (head == nullptr) {
            head = tail = newNode;
            newNode->next = newNode;
        } else {
            newNode->next = head;
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    void add(int index, T e) override {
        // TODO Q2
        if (index < 0 || index > count) {
            throw out_of_range("CircularLinkedList index is out of range");
        }
        Node* newNode = new Node(e);
        if (index == 0) {
            if (head == nullptr) {
                head = tail = newNode;
                newNode->next = newNode;
            } else {
                newNode->next = head;
                tail->next = newNode;
                head = newNode;
            }
        } else {
            Node* tmp = head;
            for (int i = 1; i < index; ++i) {
                tmp = tmp->next;
            }
            newNode->next = tmp->next;
            tmp->next = newNode;
            if (index == count) {
                tail = newNode;
            }
        }
        count++;
    }

    T removeAt(int index) override {
        // TODO Q2
        if (index < 0 || index >= count) {
            throw out_of_range("CircularLinkedList index is out of range");
        }
        Node* tmp = head;
        T removedData;
        if (index == 0) {
            removedData = head->data;
            if (count == 1) {
                delete head;
                head = tail = nullptr;
            } else {
                head = head->next;
                tail->next = head;
                delete tmp;
            }
        } else {
            for (int i = 0; i < index-1; i++) tmp = tmp->next;
            Node* nodeDel = tmp->next;
            removedData = nodeDel->data;
            tmp->next = nodeDel->next;
            if (nodeDel == tail) {
                tail = tmp;
            }
            delete nodeDel;
        }
        count--;
        return removedData;
    }

    bool removeItem(T item, void (*removeItemData)(T) = 0) override {
        // TODO Q2
        Node* tmp = head;
        for (int i = 0; i < count; ++i) {
            if (equals(tmp->data, item, itemEqual)) {
                T removedData = removeAt(i);
                if (removeItemData != nullptr) {
                    removeItemData(removedData);
                }
                return true;
            }
            tmp = tmp->next;
        }
        return false;
    }

    void clear() override {
        // TODO Q2
        removeInternalData();
    }

    T& get(int index) override {
        // TODO Q2
        if (index < 0 || index >= count) {
            throw out_of_range("CircularLinkedList index is out of range");
        }
        Node* tmp = head;
        for (int i = 0; i < index; ++i) {
            tmp = tmp->next;
        }
        return tmp->data;
    }

    int indexOf(T item) override {
        // TODO Q2
        Node* tmp = head;
        for (int i = 0; i < count; ++i) {
            if (equals(tmp->data, item, itemEqual)) {
                return i;
            }
            tmp = tmp->next;
        }
        return -1;
    }

    bool empty() override { return count == 0; }
    int size() override { return count; }
    bool contains(T item) override { return indexOf(item) >= 0; }

    string toString(string (*item2str)(T&) = 0) override {
        stringstream ss;
        ss << "[";
        Node* cur = head;
        for (int i = 0; i < count; ++i) {
            if (i > 0) ss << ", ";
            if (item2str) ss << item2str(cur->data);
            else ss << cur->data;
            cur = cur->next;
        }
        ss << "]";
        return ss.str();
    }

    void println(string (*item2str)(T&) = 0) {
        cout << toString(item2str) << endl;
    }

protected:
    static bool equals(T& lhs, T& rhs, bool (*itemEqual)(T&, T&)) {
        return itemEqual ? itemEqual(lhs, rhs) : (lhs == rhs);
    }

    void copyFrom(const CircularLinkedList<T>& list) {
        Node* cur = list.head;
        for (int i = 0; i < list.count; ++i) {
            add(cur->data);
            cur = cur->next;
        }
    }

    void removeInternalData() {
        if (deleteUserData != 0) deleteUserData(this);
        Node* cur = head;
        for (int i = 0; i < count; ++i) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        head = tail = nullptr;
        count = 0;
    }

public:
    class Node {
    public:
        T data;
        Node* next;
        Node(T data, Node* next = nullptr) : data(data), next(next) {}
    };
};

#endif
