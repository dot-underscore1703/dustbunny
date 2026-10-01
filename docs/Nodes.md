# Nodes Documentation
## Node structure
```c
typedef struct Node {
	NodeType type;
	
	struct Node *parent;
	size_t parent_idx;

	struct Node **children;
	size_t children_amt;
	
	char *value;
} Node;
```
A Node in libdustbunny is made up of the 6 fields defined in the Node type declaration shown above.
The code shown above is exactly how Nodes are declared in libdustbunny. (src/nodes.c)
- type
	The type of the Node, it tells the interpreter what it is and how it should be handled.
	For more info on the NodeType enum, see the NodeType section in this file.

- parent
	A pointer to another Node. The parent node should contain a pointer to the current node in its children field.
	The Root node should have its Node field set to NULL, this is the only Node in the AST node tree that should have this trait.

- parent_idx
	The index of the parents children field that points to the current node.
	For example, `node->parent->children[node->parent_idx]` should point to the current node.
	The Root node should have this field set to -1, this is the only Node in the AST node tree that should have this trait.
	
- children
	Pointers to pointers to Node(s). Nodes, excluding the Root node, should have a maximum of two children.
	Text nodes should have their children field set to NULL, this is the only Node in the AST node tree that should have this trait.

- children_amt
	The amount of children the node has. It will increment when a node is adopted to its children, and decrease when one of its children nodes is disowned or destroyed.
	Nodes that cannot have children, such as Text nodes should simply have this value set to 0, same as Nodes that can have children, but don't.

- value
	A string value used in File() and Arg() nodes. In File() nodes, this field is used to store a path to the file requested. This value will be used to get a handle of the file.
	In Arg() nodes, this field is used like an argv array. The first Arg() node is the program name, etc.
	
## Node operation
### node_new
```c
Node *node_new(NodeType type, char *value)
```
node_new: Create a new node with the node type and its value, if applicable.
Takes a NodeType and char*, in that order.
The type parameter specifies the type of the node you wish to create, e.g Text, Pipe, RedirectTo, etc
The value parameter takes a string value the node may have. Only Text nodes should have this value. All other nodes should have this set to NULL.

node_new will allocate new memory for the new node, set the node fields to the values provided, and allocate space for children Node pointers.
Note that node_new will not set the parent or any children, you should use node_adopt.

Returns a Node pointer on success.
Returns NULL on failure.

Will fail if a malloc() call is unsuccessful. In which case you are probably screwed anyways.

### node_destroy
```c
Node *node_destroy(Node *node)
```
node_destroy: Destroy the node at the provided pointer and free its fields.
Takes a pointer to a Node.
The node parameter points to the Node you wish to destroy.

node_destroy will call free on the nodes value and type, then iteratively call node_destroy on the pointers to the nodes children.
It will then nullify the parents pointer to the node, decrement the parents children amount.
To finish its operation it will nullify the pointer to the parent and then call free() on the node.

Returns 0 on success.
Returns 1 if the node is null.

### node_adopt
```c
void node_adopt(Node *parent, Node *child)
```
node_adopt: Add the child node to the provided parents children.
Takes 2 pointers to a Node.
The first Node is the parent that will adopt the child.
The second Node is the child that will be adopted.

node_adopt will add the child Node to the parents children field.
Then it will set the childs parent field to the parent pointer, set the childs parent_idx field and then increment the parents children_amt field.
Returns 0 on success.
Returns 1 if the parent is null.
Returns 2 if the child is null.

### node_disown
```c
void node_disown(Node *child)
```
node_disown: Disown the child provided from the parent node. The parent node will be found automatically, so no need to provide a pointer to one.
Takes 1 pointer to a Node.
The Node is the child that will get disowned.

Because nodes have a field where a pointer to the node parent is stored, you do not need to provide a pointer to a parent yourself.
node_disown will nullify the parents pointer to the child, decrease the parents children_amt field and nullify the childs pointer to the parent.
Returns 0 on success.

## Node types
### Root
The Root node is, unsurprisingly, the root of the node tree. It can carry all other node types except for itself.
The root nodes parent pointer should be NULL, and the parent_idx should be -1.
The root nodes value should be NULL.

### Command
Command nodes store commands (who could've known?). The commands they contain are made up of Arg() nodes (see the Arg() node documentation below). When parsed, each Arg() child is put into an argv array with the first child (index 0) being argv[0] and so on.
Command nodes should not contain values, and instead have the commands be in the children Arg() nodes.
Command nodes can appear anywhere. As a child to the Root node, or as children for nodes like Pipe(), etc.

### Arg
Arg nodes contain text values that correspond to an argv array. They are used as children to Command() nodes, where the order of the children is the order of the argv array.
For example, the first Arg() node under a Command() node will be argv[0], the program name.
Arg() nodes should not have children, as they are meant to only contain text values and any children will not be used.

### File
File nodes contain text values that correspond to a file path on the system. The parser/executor of the AST will use File() nodes to get a handle to the file at the path specified.
File() nodes should not contain any children, as they are not needed.

### Pipe
Pipe nodes are used for directing one commands standard output to another commands standard input.
Pipe nodes should have two children, the first being the command to get standard output from, and the second child is the command that we are putting it into.
Should a command like `c1 | c2 | c3` appear, the first child of Pipe() should be Pipe(c1,c2), and the second c3. 
So `c1 | c2 | c3` becomes `Pipe(Pipe(c1,c2),c3)`

### RedirectTo
RedirectTo nodes are similar to Pipe nodes, except rather than piping one processes standard output into anothers standard input, we redirect a programs standard output into a file.
A command like 'c1 > file1' will set c1's stdout to a file stream to file1.

### RedirectFrom
RedirectFrom nodes work the same as RedirectTo nodes, but in reverse.
Instead of putting a processes standard output into a file, RedirectFrom puts a files contents into a processes standard input.

### SendToBackground
SendToBackground nodes are incredibly simple. A normal process will not return control of the shell to the user until the process has completed its execution.
A program run with a '&' at the end will send the process to the background and immediately return control of the shell to the user.
The processes standard output will still be directed to the terminal that started the process, unless redirected.

### ExecuteOnSuccess
ExecuteOnSuccess nodes are also incredibly simple. A program ran with '&&' and another command at the end will execute the first command, and only execute the second command if the first succeeded.
A successful program run is where the program returns 0 on exit. Any other integer value is interpreted as failure.
It's direct counterpart is ExecuteOnFailure

### ExecuteOnFailure
ExecuteOnFailure nodes are exactly the same as ExecuteOnSuccess nodes (see above), the only difference is instead of executing the second program on success, it executes the second program on the first programs failure (any return value that is not 0).

## Macros
No public-facing macros are defined.
