#include <stdlib.h>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <xmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <bit>
#include <functional>
#include <mutex>
#include <vector>

#include <atomic>

typedef std::chrono::high_resolution_clock Clock;

// Key is an 8-byte integer
typedef uint64_t Key;

int compare_(const Key& a, const Key& b) {
    if (a < b) {
        return -1;
    } else if (a > b) {
        return +1;
    } else {
        return 0;
    }
}

template<typename Key>
class SkipList {
   private:
    struct Node;

   public:
    SkipList(int max_level = 16, float probability = 0.5);

    void Insert(const Key& key); // Insertion function (to be implemented by students)
    bool Contains(const Key& key) const; // Lookup function (to be implemented by students)
    std::vector<Key> Scan(const Key& key, const int scan_num); // Range query function (to be implemented by students)
    bool Delete(const Key& key) const; // Delete function (to be implemented by students)

    void Print() const;

   private:
    int RandomLevel(); // Generates a random level for new nodes (to be implemented by students)

    Node* head; // Head node (starting point of the SkipList)
    int max_level; // Maximum level in the SkipList
    float probability; // Probability factor for level increase
};

// SkipList Node structure
template<typename Key>
struct SkipList<Key>::Node {
    Key key;
    std::vector<Node*> next; // Pointer array for multiple levels

    // Constructor for Node
    Node(Key key, int level);
};

template<typename Key>
SkipList<Key>::Node::Node(Key key, int level) : key(key) {
    // Initialize the next pointers for each level
    next.resize(level, nullptr);
}

// Generate a random level for new nodes
template<typename Key>
int SkipList<Key>::RandomLevel() {
    int level = 1;
    // Increase level with probability 'probability'
    while ((static_cast<float>(rand()) / RAND_MAX) < probability && level < max_level) {
        level++;
    }
    return level;
}

// Constructor for SkipList
template<typename Key>
SkipList<Key>::SkipList(int max_level, float probability)
    : max_level(max_level), probability(probability) {
    // Initialize random seed
    srand(time(nullptr));

    // Create head node with maximum level
    head = new Node(Key(), max_level);
}

// Insert function (inserts a key into SkipList)
template<typename Key>
void SkipList<Key>::Insert(const Key& key) {
    // Array to store the update positions at each level
    std::vector<Node*> update(max_level, nullptr);
    Node* current = head;

    // Find the position to insert at each level
    for (int i = max_level - 1; i >= 0; i--) {
        while (current->next[i] != nullptr && compare_(current->next[i]->key, key) < 0) {
            current = current->next[i];
        }
        update[i] = current;
    }

    // Generate a random level for the new node
    int new_level = RandomLevel();

    // Create new node
    Node* new_node = new Node(key, new_level);

    // Update the next pointers for each level
    for (int i = 0; i < new_level; i++) {
        new_node->next[i] = update[i]->next[i];
        update[i]->next[i] = new_node;
    }
}

// Delete function (removes a key from SkipList)
template<typename Key>
bool SkipList<Key>::Delete(const Key& key) const {
    // Array to store the update positions at each level
    std::vector<Node*> update(max_level, nullptr);
    Node* current = head;

    // Find the position to delete at each level
    for (int i = max_level - 1; i >= 0; i--) {
        while (current->next[i] != nullptr && compare_(current->next[i]->key, key) < 0) {
            current = current->next[i];
        }
        update[i] = current;
    }

    // Move to the node to be deleted
    current = current->next[0];

    // If the key is not found, return false
    if (current == nullptr || compare_(current->key, key) != 0) {
        return false;
    }

    // Update the next pointers to bypass the deleted node
    for (int i = 0; i < max_level; i++) {
        if (update[i]->next[i] != current) {
            break;
        }
        update[i]->next[i] = current->next[i];
    }

    // Delete the node
    delete current;

    return true;
}

// Lookup function (checks if a key exists in SkipList)
template<typename Key>
bool SkipList<Key>::Contains(const Key& key) const {
    Node* current = head;

    // Start from the highest level and move down
    for (int i = max_level - 1; i >= 0; i--) {
        while (current->next[i] != nullptr && compare_(current->next[i]->key, key) < 0) {
            current = current->next[i];
        }
    }

    // Move to the lowest level
    current = current->next[0];

    // Check if the key is found
    return (current != nullptr && compare_(current->key, key) == 0);
}

// Range query function (retrieves scan_num keys starting from key)
template<typename Key>
std::vector<Key> SkipList<Key>::Scan(const Key& key, const int scan_num) {
    std::vector<Key> result;
    Node* current = head;

    // Find the starting position
    for (int i = max_level - 1; i >= 0; i--) {
        while (current->next[i] != nullptr && compare_(current->next[i]->key, key) < 0) {
            current = current->next[i];
        }
    }

    // Move to the first node that is >= key
    current = current->next[0];

    // Collect scan_num keys
    for (int i = 0; i < scan_num && current != nullptr; i++) {
        result.push_back(current->key);
        current = current->next[0];
    }

    return result;
}

template<typename Key>
void SkipList<Key>::Print() const {

    std::cout << "SkipList Structure:\n";

    for (int level = max_level - 1; level >= 0; --level) {

        Node* node = head->next[level];

        std::cout << "Level " << level << ": ";

        while (node != nullptr) {

            std::cout << node->key << " ";

            node = node->next[level];

        }

        std::cout << "\n";

    }

}
