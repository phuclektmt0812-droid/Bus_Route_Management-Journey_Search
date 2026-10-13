#ifndef SLINKEDLIST_H
#define SLINKEDLIST_H

#include "IList.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

template<class T>
class SLinkedList : public IList<T> {
public:
    class Iterator;
    class Node;

protected:
    Node* head;
    Node* tail;
    int count;
    bool (*itemEqual)(T& lhs, T& rhs);
    void (*deleteUserData)(SLinkedList<T>*);

public:
    SLinkedList(
        void (*deleteUserData)(SLinkedList<T>*) = 0,
        bool (*itemEqual)(T&, T&) = 0
    ) {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->next = head;
        count = 0;
        this->itemEqual = itemEqual;
        this->deleteUserData = deleteUserData;
    }

    SLinkedList(const SLinkedList<T>& list) {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->next = head;
        count = 0;
        itemEqual = list.itemEqual;
        deleteUserData = list.deleteUserData;
        copyFrom(list);
    }

    SLinkedList<T>& operator=(const SLinkedList<T>& list) {
        if (this == &list) return *this;
        removeInternalData();
        head->next = tail;
        tail->next = head;
        count = 0;
        itemEqual = list.itemEqual;
        deleteUserData = list.deleteUserData;
        copyFrom(list);
        return *this;
    }

    ~SLinkedList() {
        removeInternalData();
        delete head;
        delete tail;
    }

    void add(T e) override {
        // TODO Q1
        Node* newNode = new Node(e, tail);
        if (count == 0) {
            head->next = newNode;
        } else {
            Node* tmp = head;
            while (tmp->next != tail) {
                tmp = tmp->next;
            }
            tmp->next = newNode;
        }
        newNode->next = tail;
        count++;
    }

    void add(int index, T e) override {
        // TODO Q1
        if (index < 0 || index > count) {
            throw out_of_range("SLinkedList index is out of range");
        }
        Node* newNode = new Node(e, tail);
        Node* tmp = head;
        for (int i = 0; i < index; ++i) {
            tmp = tmp->next;
        }
        newNode->next = tmp->next;
        tmp->next = newNode;
        count++;
        return;
    }

    T removeAt(int index) override {
        // TODO Q1
        if (index < 0 || index >= count) {
            throw out_of_range("SLinkedList index is out of range");
        }
        Node* tmp = head;
        for (int i = 0; i < index; i++) tmp = tmp->next;
        Node* nodeDelete = tmp->next;
        T removeData = nodeDelete->data;
        tmp->next = nodeDelete->next;
        delete nodeDelete;
        count--;
        return removeData;
    }

    bool removeItem(T item, void (*removeItemData)(T) = 0) override {
        // TODO Q1
        Node* tmp = head->next;
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
        // TODO Q1
        removeInternalData();
        head->next = tail;
        tail->next = head;
        count = 0;
    }

    T& get(int index) override {
        // TODO Q1
        if (index < 0 || index >= count) {
            throw out_of_range("SLinkedList index is out of range");
        }
        Node* tmp = head->next;
        for (int i = 0; i < index; ++i) tmp = tmp->next;
        return tmp->data;
    }

    int indexOf(T item) override {
        // TODO Q1
        Node* tmp = head->next;
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

    bool contains(T item) override {
        return indexOf(item) >= 0;
    }

    string toString(string (*item2str)(T&) = 0) override {
        stringstream ss;
        ss << "[";
        Node* cur = head->next;
        bool first = true;
        while (cur != tail) {
            if (!first) ss << ", ";
            if (item2str) ss << item2str(cur->data);
            else ss << cur->data;
            first = false;
            cur = cur->next;
        }
        ss << "]";
        return ss.str();
    }

    void println(string (*item2str)(T&) = 0) {
        cout << toString(item2str) << endl;
    }

    void setDeleteUserDataPtr(void (*deleteUserData)(SLinkedList<T>*) = 0) {
        this->deleteUserData = deleteUserData;
    }

    Iterator begin() { return Iterator(this, true); }
    Iterator end() { return Iterator(this, false); }

    static void free(SLinkedList<T>* list) {
        Iterator it = list->begin();
        while (it != list->end()) {
            delete *it;
            it++;
        }
    }

protected:
    static bool equals(T& lhs, T& rhs, bool (*itemEqual)(T&, T&)) {
        return itemEqual ? itemEqual(lhs, rhs) : (lhs == rhs);
    }

    void copyFrom(const SLinkedList<T>& list) {
        Node* src = list.head->next;
        while (src != list.tail) {
            add(src->data);
            src = src->next;
        }
    }

    void removeInternalData() {
        if (deleteUserData != 0) deleteUserData(this);
        Node* cur = head->next;
        while (cur != tail) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
    }

public:
    class Node {
    public:
        T data;
        Node* next;

        Node(Node* next = 0) : next(next) {}
        Node(T data, Node* next = 0) : data(data), next(next) {}
    };

    class Iterator {
    private:
        SLinkedList<T>* pList;
        Node* pNode;

    public:
        Iterator(SLinkedList<T>* pList = 0, bool begin = true) {
            this->pList = pList;
            if (pList == 0) pNode = 0;
            else pNode = begin ? pList->head->next : pList->tail;
        }

        Iterator& operator=(const Iterator& iterator) {
            pNode = iterator.pNode;
            pList = iterator.pList;
            return *this;
        }

        void remove(void (*removeItemData)(T) = 0) {
            if (pList == 0 || pNode == pList->tail) return;

            Node* prev = pList->head;
            int index = 0;
            while (prev->next != pList->tail && prev->next != pNode) {
                prev = prev->next;
                ++index;
            }
            if (prev->next != pNode) return;

            T removed = pList->removeAt(index);
            if (removeItemData != 0) removeItemData(removed);
            pNode = prev;
        }

        T& operator*() { return pNode->data; }
        bool operator!=(const Iterator& iterator) { return pNode != iterator.pNode; }
        Iterator& operator++() { pNode = pNode->next; return *this; }
        Iterator operator++(int) { Iterator old = *this; ++(*this); return old; }
    };
};

#endif
