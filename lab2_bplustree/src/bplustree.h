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

// Define Clock and Key types
typedef std::chrono::high_resolution_clock Clock;
typedef uint64_t Key;

// Compare function for keys
int compare_(const Key& a, const Key& b) {
    if (a < b) {
        return -1;
    } else if (a > b) {
        return +1;
    } else {
        return 0;
    }
}

// B+ Tree class template definition
template<typename Key>
class Bplustree {
   private:
    // Forward declaration of node structures
    struct Node;
    struct InternalNode;
    struct LeafNode;

   public:
    // Constructor: Initializes a B+ Tree with the specified degree (maximum number of children per internal node)
    Bplustree(int degree = 4);

    // Insert function:
    // Inserts a key into the B+ Tree.
    // TODO: Implement insertion, handling leaf node insertion and splitting if necessary.
    void Insert(const Key& key);

    // Contains function:
    // Returns true if the key exists in the tree; otherwise, returns false.
    // TODO: Implement key lookup starting from the root and traversing to the appropriate leaf.
    bool Contains(const Key& key) const;

    // Scan function:
    // Performs a range query starting from the specified key and returns up to 'scan_num' keys.
    // TODO: Traverse leaf nodes using the next pointer and collect keys.
    std::vector<Key> Scan(const Key& key, const int scan_num);

    // Delete function:
    // Removes the specified key from the tree.
    // TODO: Implement deletion, handling key removal, merging, or rebalancing nodes if required.
    bool Delete(const Key& key);

    // Print function:
    // Traverses and prints the internal structure of the B+ Tree.
    // This function is helpful for debugging and verifying that the tree is constructed correctly.
    void Print() const;

   private:
    // Base Node structure. All nodes (internal and leaf) derive from this.
    struct Node {
        bool is_leaf; // Indicates whether the node is a leaf
        // Helper functions to cast a Node pointer to InternalNode or LeafNode pointers.
        InternalNode* as_internal() { return static_cast<InternalNode*>(this); }
        LeafNode* as_leaf() { return static_cast<LeafNode*>(this); }
        virtual ~Node() = default;
    };

    // Internal node structure for the B+ Tree.
    // Stores keys and child pointers.
    struct InternalNode : public Node {
        std::vector<Key> keys;         // Keys used to direct search to the correct child
        std::vector<Node*> children;   // Pointers to child nodes
        InternalNode() { this->is_leaf = false; }
    };

    // Leaf node structure for the B+ Tree.
    // Stores actual keys and a pointer to the next leaf for efficient range queries.
    struct LeafNode : public Node {
        std::vector<Key> keys; // Keys stored in the leaf node
        LeafNode* next;        // Pointer to the next leaf node for range scanning
        LeafNode() : next(nullptr) { this->is_leaf = true; }
    };

    // Helper function to insert a key into an internal node.
    // 'new_child' and 'new_key' are output parameters if the node splits.
    // TODO: Implement insertion into an internal node and handle splitting of nodes.
    void InsertInternal(Node* current, const Key& key, Node*& new_child, Key& new_key);

    // Helper function to delete a key from the tree recursively.
    // TODO: Implement deletion from internal nodes with proper merging or rebalancing.
    bool DeleteInternal(Node* current, const Key& key);

    // Helper function to find the leaf node where the key should reside.
    // TODO: Implement traversal from the root to the appropriate leaf node.
    LeafNode* FindLeaf(const Key& key) const;

    // Helper function to recursively print the tree structure.
    void PrintRecursive(const Node* node, int level) const;

    Node* root;   // Root node of the B+ Tree
    int degree;   // Maximum number of children per internal node
};

// Constructor implementation
// Initializes the tree by creating an empty leaf node as the root.
template<typename Key>
Bplustree<Key>::Bplustree(int degree) : degree(degree) {
    root = new LeafNode();
    // To be implemented by students
}

// Insert function: Inserts a key into the B+ Tree.
template<typename Key>
void Bplustree<Key>::Insert(const Key& key) {
    // TODO: Implement insertion logic here.
    // To be implemented by students
}


// Contains function: Checks if a key exists in the B+ Tree.
template<typename Key>
bool Bplustree<Key>::Contains(const Key& key) const {
    // TODO: Implement lookup logic here.
    // To be implemented by students
    return false;
}


// Scan function: Performs a range query starting from a given key.
template<typename Key>
std::vector<Key> Bplustree<Key>::Scan(const Key& key, const int scan_num) {
    // TODO: Implement range query logic here.
    // To be implemented by students
    return {};
}


// Delete function: Removes a key from the B+ Tree.
template<typename Key>
bool Bplustree<Key>::Delete(const Key& key) {
    // TODO: Implement deletion logic here.
    // To be implemented by students
    return false;
}


// InsertInternal function: Helper function to insert a key into an internal node.
template<typename Key>
void Bplustree<Key>::InsertInternal(Node* current, const Key& key, Node*& new_child, Key& new_key) {
    // TODO: Implement internal node insertion logic here.
    // To be implemented by students
}


// DeleteInternal function: Helper function to delete a key from an internal node.
template<typename Key>
bool Bplustree<Key>::DeleteInternal(Node* current, const Key& key) {
    // TODO: Implement internal node deletion logic here.
    // To be implemented by students
    return false;
}


// FindLeaf function: Traverses the B+ Tree from the root to find the leaf node that should contain the given key.
template<typename Key>
typename Bplustree<Key>::LeafNode* Bplustree<Key>::FindLeaf(const Key& key) const {
    // TODO: Implement the traversal logic to locate the correct leaf node.
    // To be implemented by students
    return nullptr;
}

// Print function: Public interface to print the B+ Tree structure.
template<typename Key>
void Bplustree<Key>::Print() const {
    PrintRecursive(root, 0);
}

// Helper function: Recursively prints the tree structure with indentation based on tree level.
template<typename Key>
void Bplustree<Key>::PrintRecursive(const Node* node, int level) const {
    if (node == nullptr) return;
    // Indent based on the level in the tree.
    for (int i = 0; i < level; ++i)
        std::cout << "  ";
    if (node->is_leaf) {
        // Print leaf node keys.
        const LeafNode* leaf = node->as_leaf();
        std::cout << "[Leaf] ";
        for (const Key& key : leaf->keys)
            std::cout << key << " ";
        std::cout << std::endl;
    } else {
        // Print internal node keys and recursively print children.
        const InternalNode* internal = node->as_internal();
        std::cout << "[Internal] ";
        for (const Key& key : internal->keys)
            std::cout << key << " ";
        std::cout << std::endl;
        for (const Node* child : internal->children)
            PrintRecursive(child, level + 1);
    }
}