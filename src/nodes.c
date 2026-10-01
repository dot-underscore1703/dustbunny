// =====================
// NODES.C - NO D'E TREE
// =====================

// Coming from Github? The documentation for the functions and types declared in this file is in docs/Nodes.md

#include <stdlib.h>
#include <stdio.h>
#include "libdustbunny/debug.h"

#define MAX_CHILDREN 128 // Will decrease when reallocation is implemented for nodes

/// Types for nodes. Each node has a type, which tells the interpreter how to use it and its children.
typedef enum NodeType {
	Root,	
	Command, 
	Arg,				// echo
	File,
	Pipe,				// |
	RedirectTo,			// >
	RedirectFrom,		// <

	SendToBackground,	// &
	ExecuteOnSuccess,	// &&
	ExecuteOnFailure	// ||
} NodeType;

/// The node type. The AST node tree is comprised of these types. Every node should have a parent except for the Root node, and every node should have a child except for the Text node.
typedef struct Node {
	NodeType type;
	
	struct Node *parent;
	size_t parent_idx;

	struct Node **children;
	size_t children_amt;
	
	char *value;
} Node;

/// Create a new node with the node type and its value, if applicable.
Node *node_new(NodeType type, char *value) {
	size_t node_size = sizeof(Node);
	
	dustbunny_debug("creating new node with size of %zu",node_size);
	Node *node = malloc(node_size);

	if(!node){
		// no memory left, you're probably fucked
		dustbunny_debug("failed to allocate space for node");
		return NULL;
	}

	node->type 	= type;
	node->value = value;
	
	// to be assigned to node->children
	Node **tmp_children = malloc(sizeof(Node*) * MAX_CHILDREN);

	// We made a temporary so we can do this check before assigning to the node
	if(!tmp_children){
		dustbunny_debug("failed to allocate space for node children");
		return NULL;
	}

	// We dont need the temporary after the check, since it succeeded we can give it to the node and get rid of the pointer.
	node->children = tmp_children;
	tmp_children = NULL;

	return node;
}

/// Destroy the node at the provided pointer and free its fields.
int node_destroy(Node *node) {
	dustbunny_debug("destroying node");

	// hopefully the node we a are destroying actually exists.
	if(!node){
		dustbunny_debug("...but nobody came");
		return 1;
	}

	// === I haven't checked that this works yet, its a bit confusing ;-; but it'll be fixed ===
	
	// Free the fields first so we dont get dangly variables with no pointer causing a memory leak
	free(&node->type);
	free(node->value);

	// there is probably a better way to do this but iteratively should work for now.
	for(size_t i = 0; i < node->children_amt; ++i) {
		if(node_destroy(node->children[i]) == 1){
			dustbunny_debug("failed to destroy child, %i",i);
		}
	}

	// The root node shouldn't be destroyed so hopefully this works.

	node->parent->children[
		node->parent_idx
	] = NULL; // parent_idx should hold where the pointer is in the parents children field

	--node->parent->children_amt;

	node->parent = NULL;

	// we are done
	free(node);
	
	return 0;
}

/// Add the child node provided to the parent nodes children.
int node_adopt(Node *parent, Node *child) {
	dustbunny_debug("adopting node");

	// We want to know both the child and parent actually exist.
	if(!parent) {
		dustbunny_debug("the parent is null");
		return 1;
	}
	if(!child) {
		dustbunny_debug("the child is null");
		return 2;
	}

	// This code is really funny, and probably not that good.

	// most of this is smelly pointer work (EWWW) and just counters.
	// bare with me here, cant wait to have to find the segfault that only happens once in a blue moon
	parent->children[
		parent->children_amt
	] = child; // give the parent a pointer to the child so the parent can identify its children in parsing
		
	child->parent = parent; // give the child a pointer to the parent so the child can modify the parent if needed (like when disowning)
	
	
	child->parent_idx = parent->children_amt++; // the child has to store WHERE it is. This will also increment the parents children_amt field for the next child that will be adopted.

	return 0;  
}

/// Disown the child provided from the parent node. The parent node will be found automatically, so no need to provide a pointer to one.
int node_disown(Node *child) {
	dustbunny_debug("disowning node");

	if(!child){
		dustbunny_debug("the child does not exist");
		return 1;
	}
 
	child->parent->children[
		child->parent->children_amt
	] = NULL;

	--child->parent->children_amt;

	child->parent = NULL;
	return 0;
}

