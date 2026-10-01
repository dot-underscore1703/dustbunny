#ifndef NODES_H
#define NODES_H

#include <stdlib.h>

typedef enum NodeType {
	Text,
	Pipe,
	RedirectTo,
	RedirectFrom,
	SendToBackground,
	ExecuteOnSucces,
	ExecuteOnFailure
} NodeType;

typedef struct Node {
	NodeType type;
	
	struct Node *parent;
	size_t parent_idx;

	struct Node **children;
	size_t children_amt;
	
	char *value;
} Node;

Node *node_new(NodeType type, char *value);
void node_destroy(Node *node);
void node_adopt(Node *parent, Node *child);
void node_disown(Node *child);

#endif
