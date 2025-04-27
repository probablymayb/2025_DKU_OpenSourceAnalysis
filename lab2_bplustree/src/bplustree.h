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
#include <iostream>
#include <cassert>
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

    // Destructor: Cleans up allocated memory
    ~Bplustree();

    // Insert function:
    // Inserts a key into the B+ Tree.
    void Insert(const Key& key);

    // Contains function:
    // Returns true if the key exists in the tree; otherwise, returns false.
    bool Contains(const Key& key) const;

    // Scan function:
    // Performs a range query starting from the specified key and returns up to 'scan_num' keys.
    std::vector<Key> Scan(const Key& key, const int scan_num);

    // Delete function:
    // Removes the specified key from the tree.
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
        const InternalNode* as_internal() const { return static_cast<const InternalNode*>(this); }
        const LeafNode* as_leaf() const { return static_cast<const LeafNode*>(this); }
        virtual ~Node() = default;
    };

    // Internal node structure for the B+ Tree.
    // Stores keys and child pointers.
    struct InternalNode : public Node {
        std::vector<Key> keys;         // Keys used to direct search to the correct child
        std::vector<Node*> children;   // Pointers to child nodes
        InternalNode() { this->is_leaf = false; }
        ~InternalNode() {
            for (auto child : children) {
                delete child;
            }
        }
    };

    // Leaf node structure for the B+ Tree.
    // Stores actual keys and a pointer to the next leaf for efficient range queries.
    struct LeafNode : public Node {
        std::vector<Key> keys; // Keys stored in the leaf node
        LeafNode* next;        // Pointer to the next leaf node for range scanning
        LeafNode() : next(nullptr) { this->is_leaf = true; }
    };

    // Helper function to find the appropriate parent node given a child node.
    // This is used during deletion to help find the parent of a node that needs merging or redistribution.
    InternalNode* FindParent(Node* child, Node* root);

    // Helper function to insert a key into an internal node.
    // 'new_child' is the new child node that results from splitting a child node.
    void InsertInternal(InternalNode* parent, int child_idx, const Key& key, Node* new_child);

    // Helper functions for deletion
    
    // Finds the leaf where a key should be located and its parent path
    LeafNode* FindLeafForDelete(const Key& key, std::vector<InternalNode*>& parent_path, std::vector<int>& child_indices);
    
    // Merges a node with a sibling or redistributes keys when a node has too few keys
    void HandleUnderflow(InternalNode* parent, int child_idx);
    
    // Merges two leaf nodes
    void MergeLeaves(LeafNode* left, LeafNode* right, InternalNode* parent, int right_idx);
    
    // Redistributes keys between two leaf nodes
    void RedistributeLeaves(LeafNode* left, LeafNode* right, InternalNode* parent, int right_idx);
    
    // Merges two internal nodes
    void MergeInternalNodes(InternalNode* left, InternalNode* right, InternalNode* parent, int right_idx);
    
    // Redistributes keys between two internal nodes
    void RedistributeInternalNodes(InternalNode* left, InternalNode* right, InternalNode* parent, int right_idx);

    // Helper function to find the leaf node where the key should reside.
    LeafNode* FindLeaf(const Key& key) const;

    // Helper function to recursively print the tree structure.
    void PrintRecursive(const Node* node, int level) const;

    // Helper function to clean up the tree recursively
    void CleanUp(Node* node);

    Node* root;   // Root node of the B+ Tree
    int degree;   // Maximum number of children per internal node
    int min_keys; // Minimum number of keys per node (except root)
};

// Constructor implementation
// Initializes the tree by creating an empty leaf node as the root.
template<typename Key>
Bplustree<Key>::Bplustree(int degree) : degree(degree) {
    root = new LeafNode();
    min_keys = (degree - 1) / 2; // Minimum number of keys in a node (except root)
}

// Destructor implementation
// Cleans up all allocated nodes to prevent memory leaks
template<typename Key>
Bplustree<Key>::~Bplustree() {
    delete root;
}

// FindParent: Finds the parent node of a child node
template<typename Key>
typename Bplustree<Key>::InternalNode* Bplustree<Key>::FindParent(Node* child, Node* current) {
    if (current->is_leaf || static_cast<InternalNode*>(current)->children.empty()) {
        return nullptr;
    }

    InternalNode* internal = static_cast<InternalNode*>(current);
    
    // Check if child is a direct child of current
    for (size_t i = 0; i < internal->children.size(); ++i) {
        if (internal->children[i] == child) {
            return internal;
        }
    }
    
    // Recursively check each child
    for (size_t i = 0; i < internal->children.size(); ++i) {
        if (!internal->children[i]->is_leaf) {
            InternalNode* result = FindParent(child, internal->children[i]);
            if (result != nullptr) {
                return result;
            }
        }
    }
    
    return nullptr;
}

// Insert function: Inserts a key into the B+ Tree.
template<typename Key>
void Bplustree<Key>::Insert(const Key& key) {
    // Special case for empty tree
    if (root->is_leaf && static_cast<LeafNode*>(root)->keys.empty()) {
        static_cast<LeafNode*>(root)->keys.push_back(key);
        return;
    }

    // Find the leaf node where the key should be inserted
    LeafNode* leaf = FindLeaf(key);

    // Check if the key already exists
    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    if (it != leaf->keys.end() && *it == key) {
        // Key already exists, do nothing (no duplicates allowed)
        return;
    }

    // Insert the key at the correct position
    leaf->keys.insert(it, key);

    // Check if the leaf node needs to be split
    if (leaf->keys.size() > static_cast<size_t>(degree - 1)) {
        // Create a new leaf node
        LeafNode* new_leaf = new LeafNode();
        
        // Calculate the split point
        int split_point = leaf->keys.size() / 2;
        
        // Move half of the keys to the new leaf
        new_leaf->keys.assign(leaf->keys.begin() + split_point, leaf->keys.end());
        leaf->keys.resize(split_point);
        
        // Set up the next pointers for linked list traversal
        new_leaf->next = leaf->next;
        leaf->next = new_leaf;
        
        // Get the first key in the new leaf to use as a separator in the parent
        Key separator = new_leaf->keys.front();
        
        // If the leaf is the root, create a new root
        if (leaf == root) {
            InternalNode* new_root = new InternalNode();
            new_root->keys.push_back(separator);
            new_root->children.push_back(leaf);
            new_root->children.push_back(new_leaf);
            root = new_root;
        } else {
            // Find the parent of the leaf
            InternalNode* parent = FindParent(leaf, root);
            
            // Find the index of the leaf in the parent's children
            auto child_it = std::find(parent->children.begin(), parent->children.end(), leaf);
            int child_idx = std::distance(parent->children.begin(), child_it);
            
            // Insert the new leaf into the parent
            InsertInternal(parent, child_idx, separator, new_leaf);
        }
    }
}

// InsertInternal function: Helper function to insert a key and new child into an internal node.
template<typename Key>
void Bplustree<Key>::InsertInternal(InternalNode* parent, int child_idx, const Key& key, Node* new_child) {
    // Insert the key and new child into the parent
    parent->keys.insert(parent->keys.begin() + child_idx, key);
    parent->children.insert(parent->children.begin() + child_idx + 1, new_child);
    
    // Check if the internal node needs to be split
    if (parent->keys.size() > static_cast<size_t>(degree - 1)) {
        // Create a new internal node
        InternalNode* new_internal = new InternalNode();
        
        // Calculate the split point
        int split_point = parent->keys.size() / 2;
        
        // Get the separator key (the middle key that will move up to the parent)
        Key separator = parent->keys[split_point];
        
        // Move half of the keys and children to the new internal node
        new_internal->keys.assign(parent->keys.begin() + split_point + 1, parent->keys.end());
        new_internal->children.assign(parent->children.begin() + split_point + 1, parent->children.end());
        
        // Resize the original internal node
        parent->keys.resize(split_point);
        parent->children.resize(split_point + 1);
        
        // If the parent is the root, create a new root
        if (parent == root) {
            InternalNode* new_root = new InternalNode();
            new_root->keys.push_back(separator);
            new_root->children.push_back(parent);
            new_root->children.push_back(new_internal);
            root = new_root;
        } else {
            // Find the parent of the parent
            InternalNode* grandparent = FindParent(parent, root);
            
            // Find the index of the parent in the grandparent's children
            auto child_it = std::find(grandparent->children.begin(), grandparent->children.end(), parent);
            int parent_idx = std::distance(grandparent->children.begin(), child_it);
            
            // Insert the new internal node into the grandparent
            InsertInternal(grandparent, parent_idx, separator, new_internal);
        }
    }
}

// Contains function: Checks if a key exists in the B+ Tree.
template<typename Key>
bool Bplustree<Key>::Contains(const Key& key) const {
    // Find the leaf node that should contain the key
    LeafNode* leaf = FindLeaf(key);
    
    // Check if the key exists in the leaf
    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    return (it != leaf->keys.end() && *it == key);
}

// FindLeaf function: Traverses the B+ Tree from the root to find the leaf node that should contain the given key.
template<typename Key>
typename Bplustree<Key>::LeafNode* Bplustree<Key>::FindLeaf(const Key& key) const {
    Node* current = root;
    
    // Traverse down the tree until we reach a leaf
    while (!current->is_leaf) {
        InternalNode* internal = static_cast<InternalNode*>(current);
        
        // Find the appropriate child to follow using binary search
        auto it = std::upper_bound(internal->keys.begin(), internal->keys.end(), key);
        int idx = std::distance(internal->keys.begin(), it);
        
        // Move to the child
        current = internal->children[idx];
    }
    
    // Return the leaf node
    return static_cast<LeafNode*>(current);
}

// FindLeafForDelete: Similar to FindLeaf but also records the path to the leaf
template<typename Key>
typename Bplustree<Key>::LeafNode* Bplustree<Key>::FindLeafForDelete(const Key& key, std::vector<InternalNode*>& parent_path, std::vector<int>& child_indices) {
    Node* current = root;
    
    // Clear the path
    parent_path.clear();
    child_indices.clear();
    
    // Traverse down the tree until we reach a leaf
    while (!current->is_leaf) {
        InternalNode* internal = static_cast<InternalNode*>(current);
        
        // Find the appropriate child to follow using binary search
        auto it = std::upper_bound(internal->keys.begin(), internal->keys.end(), key);
        int idx = std::distance(internal->keys.begin(), it);
        
        // Record this node and child index in our path
        parent_path.push_back(internal);
        child_indices.push_back(idx);
        
        // Move to the child
        current = internal->children[idx];
    }
    
    // Return the leaf node
    return static_cast<LeafNode*>(current);
}

// Scan function: Performs a range query starting from a given key.
template<typename Key>
std::vector<Key> Bplustree<Key>::Scan(const Key& key, const int scan_num) {
    std::vector<Key> result;
    
    // Find the leaf node that should contain the starting key
    LeafNode* leaf = FindLeaf(key);
    
    // Find the position of the starting key
    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    
    // Collect keys
    int count = 0;
    
    // Process the first leaf
    while (it != leaf->keys.end() && count < scan_num) {
        result.push_back(*it);
        ++it;
        ++count;
    }
    
    // Process subsequent leaves if needed
    leaf = leaf->next;
    while (leaf != nullptr && count < scan_num) {
        for (const Key& k : leaf->keys) {
            if (count >= scan_num) break;
            result.push_back(k);
            ++count;
        }
        leaf = leaf->next;
    }
    
    return result;
}

// Delete function: Removes a key from the B+ Tree.
template<typename Key>
bool Bplustree<Key>::Delete(const Key& key) {
    // Handle empty tree case
    if (root->is_leaf && static_cast<LeafNode*>(root)->keys.empty()) {
        return false;
    }
    
    // Find the path to the leaf node containing the key
    std::vector<InternalNode*> parent_path;
    std::vector<int> child_indices;
    LeafNode* leaf = FindLeafForDelete(key, parent_path, child_indices);
    
    // Find the key in the leaf
    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    
    // If the key doesn't exist, return false
    if (it == leaf->keys.end() || *it != key) {
        return false;
    }
    
    // Remove the key from the leaf
    int key_idx = std::distance(leaf->keys.begin(), it);
    leaf->keys.erase(leaf->keys.begin() + key_idx);
    
    // If this is the root leaf or it has enough keys, we're done
    if (leaf == root || leaf->keys.size() >= static_cast<size_t>(min_keys)) {
        return true;
    }
    
    // Otherwise, we need to handle underflow
    // Get the parent of the leaf
    InternalNode* parent = parent_path.back();
    int leaf_idx = child_indices.back();
    
    // Handle underflow
    HandleUnderflow(parent, leaf_idx);
    
    // Check if the root is an internal node with only one child
    if (!root->is_leaf && static_cast<InternalNode*>(root)->keys.empty()) {
        Node* new_root = static_cast<InternalNode*>(root)->children[0];
        delete root;
        root = new_root;
    }
    
    return true;
}

// HandleUnderflow: Handles underflow by merging or redistributing
template<typename Key>
void Bplustree<Key>::HandleUnderflow(InternalNode* parent, int child_idx) {
    // Get the underflowing child
    Node* child = parent->children[child_idx];
    
    if (child->is_leaf) {
        LeafNode* leaf = static_cast<LeafNode*>(child);
        
        // Try to borrow from left sibling
        if (child_idx > 0) {
            LeafNode* left_sibling = static_cast<LeafNode*>(parent->children[child_idx - 1]);
            
            if (left_sibling->keys.size() > static_cast<size_t>(min_keys)) {
                // Redistribute (borrow from left sibling)
                RedistributeLeaves(left_sibling, leaf, parent, child_idx);
                return;
            }
        }
        
        // Try to borrow from right sibling
        if (child_idx < static_cast<int>(parent->children.size()) - 1) {
            LeafNode* right_sibling = static_cast<LeafNode*>(parent->children[child_idx + 1]);
            
            if (right_sibling->keys.size() > static_cast<size_t>(min_keys)) {
                // Redistribute (borrow from right sibling)
                RedistributeLeaves(leaf, right_sibling, parent, child_idx + 1);
                return;
            }
        }
        
        // If we can't borrow, merge with a sibling
        if (child_idx > 0) {
            // Merge with left sibling
            LeafNode* left_sibling = static_cast<LeafNode*>(parent->children[child_idx - 1]);
            MergeLeaves(left_sibling, leaf, parent, child_idx);
        } else {
            // Merge with right sibling
            LeafNode* right_sibling = static_cast<LeafNode*>(parent->children[child_idx + 1]);
            MergeLeaves(leaf, right_sibling, parent, child_idx + 1);
        }
    } else {
        // Handle internal node underflow
        InternalNode* internal = static_cast<InternalNode*>(child);
        
        // Try to borrow from left sibling
        if (child_idx > 0) {
            InternalNode* left_sibling = static_cast<InternalNode*>(parent->children[child_idx - 1]);
            
            if (left_sibling->keys.size() > static_cast<size_t>(min_keys)) {
                // Redistribute (borrow from left sibling)
                RedistributeInternalNodes(left_sibling, internal, parent, child_idx);
                return;
            }
        }
        
        // Try to borrow from right sibling
        if (child_idx < static_cast<int>(parent->children.size()) - 1) {
            InternalNode* right_sibling = static_cast<InternalNode*>(parent->children[child_idx + 1]);
            
            if (right_sibling->keys.size() > static_cast<size_t>(min_keys)) {
                // Redistribute (borrow from right sibling)
                RedistributeInternalNodes(internal, right_sibling, parent, child_idx + 1);
                return;
            }
        }
        
        // If we can't borrow, merge with a sibling
        if (child_idx > 0) {
            // Merge with left sibling
            InternalNode* left_sibling = static_cast<InternalNode*>(parent->children[child_idx - 1]);
            MergeInternalNodes(left_sibling, internal, parent, child_idx);
        } else {
            // Merge with right sibling
            InternalNode* right_sibling = static_cast<InternalNode*>(parent->children[child_idx + 1]);
            MergeInternalNodes(internal, right_sibling, parent, child_idx + 1);
        }
    }
    
    // Check if the parent now has too few keys
    if (parent != root && parent->keys.size() < static_cast<size_t>(min_keys)) {
        // Find the parent's parent and handle underflow recursively
        InternalNode* grandparent = FindParent(parent, root);
        if (grandparent != nullptr) {
            auto it = std::find(grandparent->children.begin(), grandparent->children.end(), parent);
            int parent_idx = std::distance(grandparent->children.begin(), it);
            HandleUnderflow(grandparent, parent_idx);
        }
    }
}

// MergeLeaves: Merges two leaf nodes
template<typename Key>
void Bplustree<Key>::MergeLeaves(LeafNode* left, LeafNode* right, InternalNode* parent, int right_idx) {
    // Move all keys from right to left
    left->keys.insert(left->keys.end(), right->keys.begin(), right->keys.end());
    
    // Update the next pointer
    left->next = right->next;
    
    // Remove the key and right child from the parent
    parent->keys.erase(parent->keys.begin() + right_idx - 1);
    parent->children.erase(parent->children.begin() + right_idx);
    
    // Delete the right node
    delete right;
}

// RedistributeLeaves: Redistributes keys between two leaf nodes
template<typename Key>
void Bplustree<Key>::RedistributeLeaves(LeafNode* left, LeafNode* right, InternalNode* parent, int right_idx) {
    if (left->keys.size() < right->keys.size()) {
        // Borrow from the right
        // Move the first key from right to left
        left->keys.push_back(right->keys.front());
        right->keys.erase(right->keys.begin());
        
        // Update the parent key
        parent->keys[right_idx - 1] = right->keys.front();
    } else {
        // Borrow from the left
        // Move the last key from left to right
        right->keys.insert(right->keys.begin(), left->keys.back());
        left->keys.pop_back();
        
        // Update the parent key
        parent->keys[right_idx - 1] = right->keys.front();
    }
}

// MergeInternalNodes: Merges two internal nodes
template<typename Key>
void Bplustree<Key>::MergeInternalNodes(InternalNode* left, InternalNode* right, InternalNode* parent, int right_idx) {
    // Move the separator key from the parent to the left node
    left->keys.push_back(parent->keys[right_idx - 1]);
    
    // Move all keys and children from right to left
    left->keys.insert(left->keys.end(), right->keys.begin(), right->keys.end());
    left->children.insert(left->children.end(), right->children.begin(), right->children.end());
    
    // Clear the right's children to prevent double deletion
    right->children.clear();
    
    // Remove the key and right child from the parent
    parent->keys.erase(parent->keys.begin() + right_idx - 1);
    parent->children.erase(parent->children.begin() + right_idx);
    
    // Delete the right node
    delete right;
}

// RedistributeInternalNodes: Redistributes keys between two internal nodes
template<typename Key>
void Bplustree<Key>::RedistributeInternalNodes(InternalNode* left, InternalNode* right, InternalNode* parent, int right_idx) {
    if (left->keys.size() < right->keys.size()) {
        // Borrow from the right
        // Move the separator key from parent to left
        left->keys.push_back(parent->keys[right_idx - 1]);
        
        // Move the first key from right to parent
        parent->keys[right_idx - 1] = right->keys.front();
        right->keys.erase(right->keys.begin());
        
        // Move the first child from right to left
        left->children.push_back(right->children.front());
        right->children.erase(right->children.begin());
    } else {
        // Borrow from the left
        // Move the separator key from parent to right
        right->keys.insert(right->keys.begin(), parent->keys[right_idx - 1]);
        
        // Move the last key from left to parent
        parent->keys[right_idx - 1] = left->keys.back();
        left->keys.pop_back();
        
        // Move the last child from left to right
        right->children.insert(right->children.begin(), left->children.back());
        left->children.pop_back();
    }
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
        const LeafNode* leaf = static_cast<const LeafNode*>(node);
        std::cout << "[Leaf] ";
        for (const Key& key : leaf->keys)
            std::cout << key << " ";
        std::cout << std::endl;
    } else {
        // Print internal node keys and recursively print children.
        const InternalNode* internal = static_cast<const InternalNode*>(node);
        std::cout << "[Internal] ";
        for (const Key& key : internal->keys)
            std::cout << key << " ";
        std::cout << std::endl;
        for (const Node* child : internal->children)
            PrintRecursive(child, level + 1);
    }
}