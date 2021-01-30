The BST is templated on:
- K, type of the key
- V, type of the value
- COMP, 


The generic node of the Binary Search Tree is defined as instance of the class 
*Node*. A node is defined by:
- item: a pair of key-value
- left: a *unique_ptr<Node>* pointing to the left node below (if any)
- right: a *unique_ptr<Node>* pointing to the right node below (if any)
- up: a pointer to Node, pointing to the node above (if any)

root is a *unique_ptr<Node>* that owns the Node corresponding to the root of 
the tree. All the others Nodes are owned by the left or right item of some 
other nodes. Instead, the up pointer is a simple pointer, hence caution must 
be made in working with it (the Node it points to-ò+
is owned by some unique_ptr). 

For the class node, we define a constructor from pair_type, in two versions
(copy and move), and a constructor from unique_ptr to node that performs a 
deep copy of a subtree starting from the node passed (recursively).
We also define equality between Nodes: two nodes are equal if the key is equal.


