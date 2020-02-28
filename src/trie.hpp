#ifndef TRIE_HPP
#define TRIE_HPP

#include <cassert>
#include <stdlib.h>

using namespace std;

const int RANGE = 52;

class TrieNode {
    /*
     * Each trie node attempts to store the values of the transition edges
     * The transition "c" from a->b is represented by storing "c" on the node b,
     * such that node b is at the index c[0]
     */

    int len;  // length of the string "data" contained in this node
    char *data;
    char *value;   // value associated with the key ending at this trie node
    TrieNode **p;  // array of TrieNode pointers

   private:
    int getIndex(char c) {
        if (c < 'a')
            return c - 'A';
        return (c - 'a') + (RANGE / 2);
    }

    char getChar(int idx) {
        if (RANGE / 2 <= idx)
            return (char)((idx - RANGE / 2) + 'a');

        return (char)(idx + 'A');
    }

    TrieNode **getNewTransitionsArray() {
        return (TrieNode **)calloc(sizeof(TrieNode *) * RANGE, 1);
    }

    // need not be a leaf node
    // index will be a part of the newly created trie node
    void split(int index) {
        assert(index < this->len && index >= 0);

        // -1 since one character gets used in the edge transition
        int splittedOutLen = len - index - 1;
        char transition = *(data + index);

        TrieNode *splittedOut = new TrieNode();
        splittedOut->data = data + index + 1;
        splittedOut->len = splittedOutLen;
        // transfer value to the splitted out node
        splittedOut->value = value;
        value = nullptr;

        // this node's remaining length
        len -= (splittedOutLen + 1);  // +1 for the transition letter

        //  pointer remapping
        int newTransitionIndex = getIndex(transition);
        TrieNode **previousTransitions = this->p;
        this->p = getNewTransitionsArray();
        this->p[newTransitionIndex] = splittedOut;
        splittedOut->p = previousTransitions;
    }

    /* I DON'T THINK IT IS NECESSARY THOUGH */
    // @description
    // if this trie node has only one outward
    // edge at the "index", then merge it back inside
    void merge() {
    }

   public:
    TrieNode()
        : len(0), data(nullptr), value(nullptr), p(getNewTransitionsArray()) {
    }

    TrieNode(char *str, int sLen, char *valueAssign)
        : len(sLen),
          data(str),
          value(valueAssign),
          p(getNewTransitionsArray()) {
    }

    ~TrieNode() {
        for (int i = 0; i < RANGE; i++) {
            delete p[i];
        }

        free(p);
    }

    // finds the key in the trie, sets value and len accordingly
    // returns the trienode pointer in which the key was found, or nullptr
    // otherwise
    TrieNode *lookup(char *key, int keyLen, char *&valueToInsert) {
        TrieNode *curr = this;
        int currIndex = 0;
        char *dataStr = curr->data;

        for (int i = 0; i < keyLen;) {
            while (i < keyLen && currIndex < curr->len && *dataStr == *key) {
                dataStr++;
                key++;
                currIndex++;
                i++;
            }

            if (i == keyLen) {
                // stopped middway in a key's data
                if (currIndex < curr->len) {
                    return nullptr;
                }

                valueToInsert = curr->value;
                return curr;
            }

            int transition = getIndex(*key);

            if (!curr->p[transition]) {
                return nullptr;
            }

            curr = curr->p[transition];
            currIndex = 0;
            dataStr = curr->data;
            key++;
            i++;
        }

        if (curr->len == 0)
            valueToInsert = curr->value;
        else
            return nullptr;
        return curr;
    }

    // return whether value was overwritten or not
    bool insert(char *s, int sLen, char *valueAssign) {
        int i = 0;
        char *pointer = this->data;

        while (i < sLen && i < len && *pointer == *s) {
            s++;
            i++;
            pointer++;
            sLen--;
        }

        // The existing transition completely encloses my transition
        // so I need to split this node
        if (sLen == 0) {
            // both ending at same point, overwrite!
            if (i == len) {
                bool overwrote = this->value != nullptr;
                this->value = valueAssign;
                return overwrote;
            }

            this->split(i);
            this->value = valueAssign;
            return false;
        } else if (i != 0 && i < this->len)
            this->split(i);

        int idx = getIndex(*s);
        if (!this->p[idx]) {
            this->p[idx] = new TrieNode(s + 1, sLen - 1, valueAssign);
            return false;
        }

        return this->p[idx]->insert(s + 1, sLen - 1, valueAssign);
    }

    // technically it is supposed to backtrack and merge nodes
    // but premature optimization is the root of all evil - Donald Knuth
    void erase(char *s) {
        char *val;
        TrieNode *node = lookup(s, (uint8_t)strlen(s), val);

        // Precondition: s exists in Trie before erasure
        assert(node && node->value != nullptr);
        node->value = nullptr;
    }
};

#endif