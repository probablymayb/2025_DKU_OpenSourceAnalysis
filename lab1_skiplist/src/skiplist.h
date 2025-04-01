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

// Generate a random level for new nodes
template<typename Key>
int SkipList<Key>::RandomLevel() {
    // To be implemented by students
    return 1; // Default return value (students should modify this)
}

// Constructor for SkipList
template<typename Key>
SkipList<Key>::SkipList(int max_level, float probability)
    : max_level(max_level), probability(probability) {
    // To be implemented by students
}

// Insert function (inserts a key into SkipList)
template<typename Key>
void SkipList<Key>::Insert(const Key& key) {
    // To be implemented by students
}

// Delete function (removes a key from SkipList)
template<typename Key>
bool SkipList<Key>::Delete(const Key& key) const {
    // To be implemented by students
    return false;
}

// Lookup function (checks if a key exists in SkipList)
template<typename Key>
bool SkipList<Key>::Contains(const Key& key) const {
    // To be implemented by students
    return false;
}

// Range query function (retrieves scan_num keys starting from key)
template<typename Key>
std::vector<Key> SkipList<Key>::Scan(const Key& key, const int scan_num) {
    // To be implemented by students
    return {};
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
