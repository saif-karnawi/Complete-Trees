/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////

Node* MyGetRoot() const;

void Clear(Node *root);

void Prune(Node* og, Node *root, double tolerance);

void Render(PNG &img, Node *root) const;


int Leafs(Node * root, int leafs) const;

int SizeLocal(Node * root) const;

Node* BuildNodeCopy(Node* copy);



bool Prunable(Node* og, Node *root, double tolerance);

#endif