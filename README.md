The main class implementing the binary search tree is named Bst, templated as
requested by the assignment. 
Each node in the tree is represented by an object of the class Node. Each node
is made up of:
- item: an std::pair of key and value (keys are unique inside the tree),
  renamed pair\_type;
- left, right: std::unique\_ptr<Node> to the left and right branch below the current
  node. We decided to use unique pointers to represent the fact that the Node
  is the owner of its left and right branches;
- up: a pointer Node\* to the node above the current Node (if any, otherwise nullptr).
  We need to be careful since the node pointed by this pointer is owned by
  another std::unique\_ptr<Node> (some left or right). We decided to use up since it
  made some of the operations easier.

The root of the tree is contained in a std::unique\_ptr<Node> called root.

The class Node contains:
- a copy constructor that takes a const reference to pair\_type
- a move constructor that takes an r-value reference to pair\_type
- a copy constructor that takes a unique\_ptr to Node and builds a
  tree starting from that Node, performing a deep copy of all the branches
  (recursively).

The first two constructors are used to build a Node from scratch, while the
last one is used to copy a sub-tree.

Moreover, we define the == operator that confronts the keys of the nodes
(defined as friend inside the class).

The last item contained in the private implementation of Bst is the iterator
class. This is contained in \_iterator, which is used to perform forwarding
reference, i.e. this class is used to build both constant and non-constant
iterators (later on in the public interface of Bst).

The definition of \_iterator is quite standard. The iterator points to a Node,
while the value\_type (represented by O in the forwarding reference) is either a
pair\_type or a const pair\_type. As a clarification, value\_type inside the class
\_iterator does NOT correspond to the value of the key-value pair, but
corresponds to the whole pair.

We then define a constructor that takes a pointer to node, the get\_current()
function that returns the Node pointed to (in case we not only want to access
the pair but also left, right and up).

The pre-increment is then defined. The algorithm used to perform this is the
following:

The post-increment is simply derived by the pre-increment, and the operators ==
and != rely on the operator == defined for the class Node (which confronts
keys).

We now pass to a description of the public interface of Bst.

We define a function returnMin that returns the minimum key for a subtree
starting from p, passed as a pointer to Node. This is easily done by proceeding
on the left iteratively. This function is used later by other algorithms.

We then define the insert operator, as required by the assignment. This is done
by performing a forwarding reference. That is, we define \_insert templated on
O, which will contain either const l-value or r-value reference to a pair\_type.

\_insert is then implemented recursively. This function takes as argument x,
the const l-value or r-value reference to a pair\_type object to be inserted,
and a std::unique_ptr<Node>& y, which is the subtree on which we wish to insert
x. \_insert then returns a pair of iterator, representing the newly inserted
node, and a bool representing the success of the insertion.

The function insert will then call \_insert on x and root (i.e. we insert on
a subtree represented by the whole tree). 

The \_insert function performs:
- if the subtree y is empty, we fill it with the new Node (remembering to fill
  also the up Node) and return the iterator;
- else if the current key to be inserted is less than the one of y (the
  comparison is made using the templated comparison operator), we descend left
  by iteratively calling the function \_insert (remembering, also in this case,
  to fill the up Node);
- the same if the comparison is in the opposite sense, by descending on the
  right;
- finally, if the key is already present, we simply return false.

We then define the default ctors and dtors of Bst, the copy and move
constructor and assignment by using standard implementations.

We define emplace, which performs a construction of the pair kay+value and
inserts it in the tree (by using insert).

clear() simply reset the root node.

We define begin() and end() in the various implementations. The first item
is returned by using returnMin(), as defined above, while end() simply returns
nullptr.

find is then implemented by iteratively doing a comparison between the key to
be found and the key of the current node, descending on the right or on the
left if the key is not equal, returning the correct iterator if the key is
found (or not found, in which case we return the iterator{nullptr}, the same
returned by end()). 

We also implement a const version of find, by using the above implementation
and a const\_cast. This will be used in cases for which a const implementation
is needed (find is never modifying the tree).

The subscript operator is then implemented. This is implemented by using a
forwarding reference on \_subscript, in order to consider both the const and
non-const case. Inside \_subsript, we use find to find the element (if any).

We now pass to erase. 

As for the balancing of the tree, we decided to proceed as follows. Inside the
function balance, we build a vector of pairs storing the values contained in
the tree. After that, we pass this array to the function buildBalancedTree
which builds a balanced tree starting from this vector, and reset the root with
this newly built tree.

The function buildBalancedTree proceeds by recursion. In particular, we 

We note that we could have used the implementation contained inside \_insert to
also perform the find operation, to avoid some code duplication.

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
is owned by some unique\_ptr). 

For the class node, we define a constructor from pair\_type, in two versions
(copy and move), and a constructor from unique\_ptr to node that performs a 
deep copy of a subtree starting from the node passed (recursively).
We also define equality between Nodes: two nodes are equal if the key is equal.


