#Binary search Tree#

The main class implementing the binary search tree is named **Bst**, templated as
requested by the assignment. 

Each node in the tree is represented by an object of the class **Node**, which
we now describe.

The root of the tree is contained in a std::unique\_ptr<Node> called root.

We first describe how the class Node is implemented. Then, we pass to a
description of the iterator interface to the tree. Finally, we describe all the
other functions of the class Bst, that extensively use the Node and Iterator
implementations.

##Node##

Each node is made up of:
- **item**: an std::pair of key and value (keys are unique inside the tree),
  renamed pair\_type;
- **left**, **right**: std::unique\_ptr<Node> to the left and right branch below the current
  node (if any, otherwise nullptr). We decided to use unique pointers to represent the fact that the Node
  is the owner of its left and right branches;
- up: a pointer Node\* to the node above the current Node (if any, otherwise nullptr).
  We need to be careful since the node pointed by this pointer is owned by
  another std::unique\_ptr<Node> (some left or right). We decided to use up since it
  made some of the operations easier.

The class Node contains:
- a copy constructor that takes a const reference to pair\_type
- a move constructor that takes an r-value reference to pair\_type
- a copy constructor that takes a unique\_ptr<Node> and builds a
  tree starting from that Node, performing a deep copy of all the branches
  (recursively).

The first two constructors are used to build a Node from scratch, while the
last one is used to copy a sub-tree (or a full tree). 
To perform this deep copy, we proceed
recursively. In particular, if the node to be copied from has left, we reset the
left of the tree to be copied to by using the same Node constructor (which in
turn will call other Node constructors to build the branches, and so on), also
remembering to update the up values. The same is done for the right branch,
if any.

Moreover, we define the == operator that confronts the keys of the nodes
(defined as friend inside the class).

##Iterator##

Another item contained in the private implementation of Bst is the iterator
class. This is contained in \_iterator, which is used to perform forwarding
reference, i.e. this class is used to build both constant and non-constant
iterators (later on in the public interface of Bst).

The definition of \_iterator is quite standard. The iterator points to a Node,
while the value\_type (represented by O in the forwarding reference) is either a
pair\_type or a const pair\_type. As a clarification, value\_type inside the class
\_iterator does NOT correspond to the value of the key-value pair (which
instead is templated as V), but corresponds to the whole pair.

We then define a constructor that takes a pointer to node and we define 
the get\_current() function that returns the Node pointed to (in case we not 
only want to access the pair but also left, right and up).

The pre-increment is then defined. The algorithm used to perform this is the
following:
- if the current node is the nullptr, return the nullptr
- if the current node has right, simply return the minimum on the right
  subtree, computed by iteratively going to the left
- if it does not have right subtree, it is a bit more complex. We need first to
  go up iteratively until we are going up from a left branch. When in fact we
  are going up from a left branch, that up is the next we are searching for.

The post-increment is simply derived by the pre-increment, and the operators ==
and != rely on the operator == defined for the class Node (which confronts
keys).

## Public interface of Bst ##

We now pass to a description of other functions of the class Bst.

### returnMin ###

We define a function **returnMin** that returns the minimum key for a subtree
starting from p, passed as a pointer to Node. This is easily done by proceeding
on the left iteratively. This function is used later by other algorithms.

### insert ###

We then define the **insert** operator, as required by the assignment. This is done
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

### constructors and destructors ###

We then define the default ctors and dtors of Bst, the copy and move
constructor and assignment by using standard implementations.

### emplace ###

We define emplace, which performs a construction of the pair kay+value and
inserts it in the tree (by using insert).

### clear ###

clear() simply reset the root node.

### begin and end ###

We define begin() and end() in the various implementations. The first item
is returned by using returnMin(), as defined above, while end() simply returns
nullptr.

### find ###

find is then implemented by iteratively doing a comparison between the key to
be found and the key of the current node, descending on the right or on the
left if the key is not equal, returning the correct iterator if the key is
found (or not found, in which case we return the iterator{nullptr}, the same
returned by end()). 

We also implement a const version of find, by using the above implementation
and a const\_cast. This will be used in cases for which a const implementation
is needed (find is never modifying the tree).

We note that we could have used the implementation contained inside \_insert to
also perform the find operation, to avoid some code duplication.

### subscripting operator ###

The subscript operator is then implemented. This is implemented by using a
forwarding reference on \_subscript, in order to consider both the const and
non-const case. Inside \_subsript, we use find to find the element (if any).

### erase ###

We now pass to erase. The implementation of the function erase is based on the
function eraseFromSubtree, which erases a node (identified with a key) from a
subtree starting from root\_, operating recursively. In the end, erase will
just call eraseFromSubtree on root.

eraseFromSubtree works using the following algorithm:
- if the subtree given is empty, do nothing;
- if the key to ke deleted is less than the key corresponding to the root\_
  node (the comparison is made using the templated comparison operator), then
  call eraseFromSubtree on the left subtree;
- the same if the key is bigger, passing the right subtree
- if we found the key, we proceed as follows. We first find the min node in the
  right subtree of the current node, which corresponds to the successive node
  with respect to the current node. What we wish to do is to take this node,
  and reset the current node with it. To do so, we need to be careful with
  left, right and up nodes. In particular, we first save in three temporary
  pointers the up, left and right of the current node (right and left are
  released). We then create a new node containing the pair of the minimum node
  found before, and reset the current node with this. Then, we update left,
  right and up with the temporary pointers saved before. Then, we also upfate
  values of up of the newly saved left and right. Finally, we erase the minimum
  which we substituted.

### balance ###

As for the balancing of the tree, we decided to proceed as follows. Inside the
function balance, we build a vector of pairs storing the values contained in
the tree. After that, we pass this array to the function buildBalancedTree
which builds a balanced tree starting from this vector, and reset the root with
this newly built tree.

The function buildBalancedTree proceeds by recursion. In particular, the
function accepts 3 arguments: the vector, and two numbers a and b that express
the fact that we want to build the subtree for elements in the vector going
from index a to index b. The, inside the function, we compute h as the mean
point between a and b, and recursively build the subtrees going from a to h-1
and from h+1 to b. Finally, we also need to update the values of the up
pointers.

### printing ###

Finally, we define the streaming operator << used to print the tree. This
operator uses the iterators previously defined, and simply does a for loop on
the tree streaming the value to os. 

print and print_depth are functions used during the construction of the various
algorithms in order to test the code.
