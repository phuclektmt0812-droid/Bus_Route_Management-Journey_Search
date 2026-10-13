#include "SLinkedList.h"
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
static void observeClear(SLinkedList<int>*) {
    ++clearCallbackCalls;
}

int main(int argc, char* argv[]) {
    PublicTestSuite suite("Q1 - SLinkedList");

    suite.add("Empty list and single-item boundary", []() {
        SLinkedList<int> list;
        requireTrue(list.empty(), "new list is empty");
        requireEqual(list.indexOf(99), -1, "indexOf on empty list returns -1");
        requireTrue(!list.contains(99), "contains on empty list is false");

        list.add(0, 1);
        requireEqual(list.get(0), 1, "add at index 0 works on empty list");
        requireEqual(list.removeAt(0), 1, "removeAt removes the only item");
        requireTrue(list.empty(), "list is empty after removing its only item");
    });

    suite.add("Insertion and indexed access", []() {
        SLinkedList<int> list;
        list.add(10);
        list.add(30);
        list.add(1, 20);
        list.add(list.size(), 40);

        requireEqual(list.size(), 4, "size after insertion");
        requireEqual(list.get(0), 10, "get first");
        requireEqual(list.get(1), 20, "get middle");
        requireEqual(list.get(3), 40, "add at index == size");
        requireEqual(list.indexOf(20), 1, "indexOf returns existing index");
        requireEqual(list.indexOf(999), -1, "indexOf missing item returns -1");
    });

    suite.add("Invalid indices", []() {
        SLinkedList<int> list;
        list.add(10);
        list.add(20);

        requireOutOfRange([&]() { list.get(-1); }, "get negative index");
        requireOutOfRange([&]() { list.get(list.size()); }, "get index == size");
        requireOutOfRange([&]() { list.add(-1, 99); }, "add negative index");
        requireOutOfRange([&]() { list.add(list.size() + 1, 99); }, "add index > size");
        requireOutOfRange([&]() { list.removeAt(-1); }, "removeAt negative index");
        requireOutOfRange([&]() { list.removeAt(list.size()); }, "removeAt index == size");
    });

    suite.add("Removal at head and tail", []() {
        SLinkedList<int> list;
        list.add(10);
        list.add(20);
        list.add(30);
        list.add(40);

        requireEqual(list.removeAt(0), 10, "remove head");
        requireEqual(list.removeAt(list.size() - 1), 40, "remove tail");
        requireEqual(list.size(), 2, "size after removals");
        requireEqual(list.get(0), 20, "first remaining item");
        requireEqual(list.get(1), 30, "second remaining item");
    });

    suite.add("Custom equality, duplicates, and remove callback", []() {
        SLinkedList<Token> tokens(nullptr, sameKey);
        tokens.add(Token(1, "first"));
        tokens.add(Token(2, "middle"));
        tokens.add(Token(1, "second"));

        requireEqual(tokens.indexOf(Token(1, "ignored")), 0, "indexOf uses custom equality and first match");
        requireTrue(tokens.contains(Token(2, "ignored")), "contains uses custom equality");

        removedCount = 0;
        removedTag.clear();
        requireTrue(tokens.removeItem(Token(1, "ignored"), recordRemoved), "removeItem finds custom-equal item");
        requireEqual(removedCount, 1, "remove callback called once");
        requireEqual(removedTag, string("first"), "removeItem removes first match");
        requireEqual(tokens.get(1).tag, string("second"), "later duplicate remains");
        requireTrue(!tokens.removeItem(Token(9, "none"), recordRemoved), "removeItem missing item returns false");
        requireEqual(removedCount, 1, "remove callback not called for missing item");
    });

    suite.add("Copy constructor and assignment", []() {
        SLinkedList<int> source;
        source.add(7);
        source.add(8);

        SLinkedList<int> copied(source);
        SLinkedList<int> assigned;
        assigned = source;
        source.removeAt(0);

        requireEqual(copied.get(0), 7, "copy constructor has independent nodes");
        requireEqual(copied.get(1), 8, "copy constructor preserves order");
        requireEqual(assigned.get(0), 7, "assignment has independent nodes");
    });

    suite.add("Clear, callback, and reuse", []() {
        SLinkedList<int> list;
        list.add(1);
        list.add(2);
        list.clear();
        requireTrue(list.empty(), "clear makes list empty");
        list.clear();
        list.add(42);
        requireEqual(list.get(0), 42, "reuse after clear");

        clearCallbackCalls = 0;
        {
            SLinkedList<int> owned(observeClear);
            owned.add(1);
            owned.add(2);
            owned.clear();
            requireEqual(clearCallbackCalls, 1, "clear invokes deleteUserData once");
            requireTrue(owned.empty(), "clear leaves the list empty");
        }
    });

    return suite.run(argc, argv);
}
