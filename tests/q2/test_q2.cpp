#include "CircularLinkedList.h"
#include "tests/TestUtils.h"

struct Token {
    int key;
    string tag;

    Token(int key = 0, string tag = "") : key(key), tag(tag) {}
    bool operator==(const Token& other) const {
        return key == other.key && tag == other.tag;
    }
};

static ostream& operator<<(ostream& os, const Token& t) {
    return os << t.key << ":" << t.tag;
}

static bool sameKey(Token& a, Token& b) {
    return a.key == b.key;
}

static int removedCount = 0;
static string removedTag;
static void recordRemoved(Token t) {
    ++removedCount;
    removedTag = t.tag;
}

static int clearCallbackCalls = 0;
static void observeClear(CircularLinkedList<int>*) {
    ++clearCallbackCalls;
}

int main(int argc, char* argv[]) {
    PublicTestSuite suite("Q2 - CircularLinkedList");

    suite.add("Empty list and single-node boundary", []() {
        CircularLinkedList<int> list;
        requireTrue(list.empty(), "new circular list is empty");
        requireEqual(list.indexOf(99), -1, "indexOf on empty circular list returns -1");
        requireTrue(!list.contains(99), "contains on empty circular list is false");

        list.add(0, 99);
        requireEqual(list.get(0), 99, "add at index 0 works on empty circular list");
        requireEqual(list.removeAt(0), 99, "removeAt removes only circular node");
        requireTrue(list.empty(), "empty after removing only circular node");
    });

    suite.add("Insertion and indexed access", []() {
        CircularLinkedList<int> list;
        list.add(10);
        list.add(30);
        list.add(1, 20);
        list.add(0, 5);
        list.add(list.size(), 40);

        requireEqual(list.size(), 5, "size after insertion");
        requireEqual(list.get(0), 5, "head item");
        requireEqual(list.get(2), 20, "middle item");
        requireEqual(list.get(4), 40, "add at index == size");
        requireEqual(list.indexOf(20), 2, "indexOf returns existing circular index");
        requireEqual(list.indexOf(999), -1, "indexOf missing circular item returns -1");
    });

    suite.add("Invalid indices", []() {
        CircularLinkedList<int> list;
        list.add(10);
        list.add(20);

        requireOutOfRange([&]() { list.get(-1); }, "get negative index");
        requireOutOfRange([&]() { list.get(list.size()); }, "get index == size");
        requireOutOfRange([&]() { list.add(-1, 99); }, "add negative index");
        requireOutOfRange([&]() { list.add(list.size() + 1, 99); }, "add index > size");
        requireOutOfRange([&]() { list.removeAt(-1); }, "removeAt negative index");
        requireOutOfRange([&]() { list.removeAt(list.size()); }, "removeAt index == size");
    });

    suite.add("Head, tail, and middle removal", []() {
        CircularLinkedList<int> list;
        list.add(5);
        list.add(10);
        list.add(20);
        list.add(30);
        list.add(40);

        requireEqual(list.removeAt(0), 5, "remove head");
        requireEqual(list.removeAt(list.size() - 1), 40, "remove tail");
        requireTrue(list.removeItem(20), "remove middle item");
        requireEqual(list.size(), 2, "size after head, tail, and middle removals");
        requireEqual(list.get(0), 10, "remaining first item");
        requireEqual(list.get(1), 30, "remaining second item");
    });

    suite.add("Custom equality, duplicates, and remove callback", []() {
        CircularLinkedList<Token> tokens(nullptr, sameKey);
        tokens.add(Token(1, "first"));
        tokens.add(Token(2, "middle"));
        tokens.add(Token(1, "second"));

        requireEqual(tokens.indexOf(Token(1, "ignored")), 0, "indexOf uses custom equality and first match");
        requireTrue(tokens.contains(Token(2, "ignored")), "contains uses custom equality");

        removedCount = 0;
        removedTag.clear();
        requireTrue(tokens.removeItem(Token(1, "ignored"), recordRemoved), "removeItem custom-equal item");
        requireEqual(removedCount, 1, "remove callback called once");
        requireEqual(removedTag, string("first"), "removeItem removes first match");
        requireEqual(tokens.get(1).tag, string("second"), "later duplicate remains");
        requireTrue(!tokens.removeItem(Token(9, "none"), recordRemoved), "removeItem missing item returns false");
        requireEqual(removedCount, 1, "remove callback not called for missing circular item");
    });

    suite.add("Copy constructor and assignment", []() {
        CircularLinkedList<int> source;
        source.add(11);
        source.add(12);

        CircularLinkedList<int> copied(source);
        CircularLinkedList<int> assigned;
        assigned = source;
        source.removeAt(0);

        requireEqual(copied.get(0), 11, "copy constructor has independent nodes");
        requireEqual(copied.get(1), 12, "copy constructor preserves order");
        requireEqual(assigned.get(0), 11, "assignment has independent nodes");
    });

    suite.add("Clear, callback, and reuse", []() {
        CircularLinkedList<int> list;
        list.add(1);
        list.add(2);
        list.clear();
        requireTrue(list.empty(), "clear makes circular list empty");
        list.clear();
        list.add(7);
        requireEqual(list.get(0), 7, "reuse after clear");

        clearCallbackCalls = 0;
        {
            CircularLinkedList<int> owned(observeClear);
            owned.add(1);
            owned.add(2);
            owned.clear();
            requireEqual(clearCallbackCalls, 1, "clear invokes circular deleteUserData once");
            requireTrue(owned.empty(), "circular clear leaves the list empty");
        }
    });

    return suite.run(argc, argv);
}
