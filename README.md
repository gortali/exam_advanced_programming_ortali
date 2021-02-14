# Python - Dictionary reverse #

Regarding the python part, we provide a jupyter notebook, named `my_exam.ipynb`, where I did my experimentations with 
computational time, and `test_exam.py`, that can be run in order to test the implementations.

Inside `my_exam.ipynb` we first define some dictionaries to test.
Then, we define:
- `rev_dict`, which reverses the dictionary using for loops;
- `rev_dict_compr`, which reverses the dictionary using comprehensions.

We then test the speeds using `%%timeit`. 

`test_exam.py` can be easily run by running `pytest` on the directory.

# C++ - Binary search Tree #

The main class implementing the binary search tree is named **Bst**, templated as
requested in the assignment. 

Each node in the tree is represented by an object of the class **Node**.
The root of the tree is contained in a `std::unique_ptr<Node>` named `root`.

We first illustrate how to run the code. Then, we describe how the class `Node` is implemented. After that, 
we pass to a description of the iterator interface to the `Bst`. Finally, we describe all the
other functions of the class `Bst`, that extensively use the `Node` and iterator
implementations. Finally, we briefly comment the benchmarking done inside the `main()` function in
order to test the functions created

## Running the code ##

In order to compile and run the code, it's sufficient to run `make`, which will compile and 
generate the executable, and then run `./bst`. The compilation is done using the flags
required in the assignment. If you want to check the memory leaks, simply run `valgrind ./bst`.

## Node ##

A `Node` is made up of:
- **item**: an `std::pair<K,V>` of key and value (keys are unique inside the tree),
  renamed `pair_type`;
- **left**, **right**: `std::unique_ptr<Node>` to the left and right branch below the current
  node (if any, otherwise the unique pointer points to `nullptr`). We decided to use unique pointers 
  to represent the fact that the current node is the owner of its left and right branches;
- up: a pointer `Node*` to the node above the current node (if any, otherwise `nullptr`).
  We need to be careful since the node pointed by this pointer is owned by
  another `std::unique_ptr<Node>` (some left or right). We decided to use up since it
  made some of the operations easier (in particular tree traversing).

The class `Node` contains:
- a copy constructor, that takes a const reference to `pair_type`
- a move constructor, that takes an r-value reference to `pair_type`
- a copy constructor, that takes a `unique_ptr<Node>` and builds a
  tree starting from that node, performing a deep copy of all the branches
  (recursively).

The first two constructors are used to build a node from scratch, while the
last one is used to copy a sub-tree (or a full tree). 
To perform this deep copy, we proceed
recursively. In particular, if the node to be copied from has `left`, we reset the
`left` of the tree by using the same Node constructor (which in
turn will call other node constructors to build the branches, and so on), also
remembering to update the `up` values. The same is done for the `right` branch,
if any.

Moreover, we define the `==` operator that confronts the keys of the nodes
(defined as `friend` inside the class).

## Iterator ##

Another item contained in the private implementation of Bst is the iterator
class. This is contained in `_iterator`, which is used to build both constant and non-constant
iterators (later on in the public interface of Bst) by using templates.

The definition of `_iterator` is quite standard. The iterator points to a `Node`,
while the `value_type` (templated by `O` in `_iterator`) is either a
`pair_type` or a `const pair_type`. As a clarification, `value_type` inside the class
`_iterator` does __NOT__ correspond to the value of the key-value pair (which
instead is templated as `V`), but corresponds to the whole pair of key+value.

We then define a constructor that takes a pointer to `Node`, and we define 
the `get_current()` function that returns the node pointed to (in case we not 
only want to access the pair but also `left`, `right` and `up` of the node).

The pre-increment is then defined. The algorithm used to perform this is the
following:
- if the current node is the `nullptr`, return the `nullptr`;
- else if the current node has `right`, simply return the minimum on the right
  subtree, computed by iteratively going to the left;
- else if the current does not have right subtree, it is a bit more complex. We need first to
  go up iteratively until we are going up from a left branch. When in fact we
  are going up from a left branch, that up we are going to is the next node we are searching for.

The post-increment is simply derived by the pre-increment, and the operators `==`
and `!=` rely on the operator `==` defined for the class Node (which confronts
keys).

## Interface of Bst ##

We now pass to a description of other functions of the class Bst. 

### returnMin ###

We define a function `returnMin` that returns the minimum key for a subtree
starting from `p`, passed as a `Node*`. This is easily done by proceeding
on the left iteratively. This function is used later by other algorithms.

### insert ###

We then define the `insert` operator, as required by the assignment. This is done
by performing a forwarding reference. That is, we define `_insert` templated on
`O`, which will contain either const l-value or r-value reference to a `pair_type`.

`_insert` is implemented recursively. This function takes as argument x,
the const l-value or r-value reference to a `pair_type` object to be inserted,
and a `std::unique_ptr<Node>& y`, which is the subtree on which we wish to insert
x. `_insert` then returns a pair of: iterator, representing the newly inserted
node, and a bool representing the success of the insertion.

The functions insert, in the two different implementations, will then call `_insert` 
on x and root (i.e. we insert on a subtree represented by the whole tree). 

The `_insert` function performs:
- if the subtree y is empty, we fill it with the new node (remembering to fill
  also the up Node) and return the iterator;
- else if the current key to be inserted is less than the one of y (the
  comparison is made using the templated comparison operator), we descend left
  by iteratively calling the function `_insert` (remembering, also in this case,
  to fill the up Node);
- the same if the comparison is in the opposite sense, by descending on the
  right;
- finally, if the key is already present, we simply return false.

### constructors and destructors ###

We then define the default ctors and dtors of `Bst`, the copy and move
constructor and assignment by using standard implementations.

### emplace ###

We define `emplace`, which performs a construction of the pair kay+value and
inserts it in the tree (by using `insert`).

### clear ###

`clear()` simply reset the root node.

### begin and end ###

We define `begin()` and `end()` in the various implementations. The first item
is returned by using `returnMin()`, as defined above, while `end()` simply returns
`nullptr`.

### find ###

`find` is implemented by iteratively doing a comparison between the key to
be found and the key of the current node, descending on the right or on the
left if the key is not equal, returning the correct iterator if the key is
found (or not found, in which case we return the `iterator{nullptr}`, the same
returned by `end()`). 

We also implement a const version of `find`, by using the above implementation
and a `const_cast`. This will be used in cases for which a const implementation
is needed.

We note that we could have used the implementation contained inside `_insert` to
also perform the find operation, to avoid some code duplication.

### subscripting operator ###

The subscript operator is then implemented. This is done templating on `_subscript`, 
in order to consider both the const and
non-const case. Inside `_subscript`, we use find to find the element (if any).

### erase ###

We now pass to `erase`. The implementation of the function erase is based on the
function `eraseFromSubtree`, which erases a node (identified with a key) from a
subtree starting from `root_`, operating recursively. In the end, `erase` will
just call `eraseFromSubtree` on `root`.

`eraseFromSubtree` works using the following algorithm:
- if the subtree given is empty, do nothing;
- else if the key to ke deleted is less than the key corresponding to the `root_`
  node (the comparison is made using the templated comparison operator), then
  call `eraseFromSubtree` on the `left` subtree;
- the same if the key is bigger, passing the `right` subtree
- if we found the key, we proceed as follows. We first find the min node in the
  right subtree of the current node, which corresponds to the successive node
  with respect to the current node. What we wish to do is to take this node,
  and reset the current node with it. To do so, we need to be careful with
  `left`, `right` and `up` nodes. In particular, we first save inside three temporary
  pointers the `up`, `left` and `right` of the node to be deleted (right and left are
  released). We then create a new node containing the pair of the minimum node
  found before, and reset the current node with this. Then, we update `left`,
  `right` and `up` with the temporary pointers saved before. Then, we also update
  values of up of the newly saved `left` and `right`. Finally, we erase the minimum
  which we substituted.

### balance ###

As for the balancing of the tree, we decided to proceed as follows. Inside the
function `balance`, we build a `std::vector` of pairs storing the values contained in
the tree. After that, we pass this vector to the function `buildBalancedTree`
which builds a balanced tree starting from this vector, and reset the root with
this newly built tree.

The function `buildBalancedTree` proceeds by recursion. In particular, the
function accepts 3 arguments: the vector, and two numbers `a` and `b` that express
the fact that we want to build the subtree for elements in the vector going
from index a to index b. Then, inside the function, we compute `h` as the mean
point between `a` and `b`, and recursively build the subtrees going from `a` to `h-1`
and from `h+1` to `b`. Finally, we also need to update the values of the `up`
pointers.

### printing ###

Finally, we define the streaming operator `<<` used to print the tree. This
operator uses the iterators previously defined, and simply does a for loop on
the tree streaming the value to `os`. 

print and print_depth are functions used during the construction of the various
algorithms in order to test the code, which we avoid discussing here.

## Benchmamarking ##
Inside the main function, we perform some tests. These test are not designed to be comprehensive
of all the possible behaviour of the tree class.

We do the following:
- create a tree templated with int for keys and strings for values. Then we
and insert some nodes using insert and emplace, and print the tree.
- we then try balancing the tree and print it again.
- then, we erase some nodes
- we then search for two nodes, one present and one not
- finally, we test the various copy constructors, clearing and copy assignment operators


