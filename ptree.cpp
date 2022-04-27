/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages
#include <iostream>

using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  //add your implementation below
  if(root != nullptr) {
    Clear(root);
    root = NULL;
  }
}

void PTree::Clear(Node *root) {

  if(root != nullptr) {
    Clear(root->A);
    Clear(root->B);

    delete root;
    root = NULL;

  }
  
}


/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  // add your implementation below
  int width = other.MyGetRoot()->width;
  int height = other.MyGetRoot()->height;
  PNG source;
  source.resize(width, height);
  root = BuildNodeCopy(other.MyGetRoot()); 
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {
  // replace the line below with your implementation

  Node *node = new Node();
  node->width = w;
  node->height = h;
  node->upperleft = ul;

  if(w == 1 && h == 1) {
    
    HSLAPixel *pixel = im.getPixel(ul.first, ul.second);
    node->avg.s = pixel->s;
    node->avg.l = pixel->l;
    node->avg.h = pixel->h;
    return node;
  }
  

  int pixelCount = 0;
  double sumS = 0;
  double sumL = 0;
  double sumH = 0;

  double hueX = 0;
  double hueY = 0;
  

  int x = (unsigned int)ul.first;
  int y = (unsigned int)ul.second;

  for(int i = 0; i < w; i++) {
    for(int j = 0; j < h; j++) {
      HSLAPixel *pixel = im.getPixel(x, y);
      pixelCount++;
      sumS += pixel->s;
      sumL += pixel->l;
      hueX += Deg2X(pixel->h);
      hueY += Deg2Y(pixel->h);
      y++;
    }
    
    x++;
    y = ul.second;
  }

  node->avg.s = sumS / pixelCount;
  node->avg.l = sumL / pixelCount;
  node->avg.h = XY2Deg(hueX, hueY);

  if (h > w) { 
    node->A = BuildNode(im, ul, w, h/2);
    node->B = BuildNode(im, make_pair(ul.first, ul.second+(h/2)), w, h/2+h%2);
  } else {
    node->A = BuildNode(im, ul, w/2, h);
    node->B = BuildNode(im, make_pair(ul.first+(w/2), ul.second), w/2+w%2, h);
  }

  return node;
}

////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  // add your implementation below
  int width = im.width();
  int height = im.height();

  root = BuildNode(im, {0,0} , width , height);
  
} 

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  // add your implementation below
  int width = other.MyGetRoot()->width;
  int height = other.MyGetRoot()->height;
  PNG source;
  source.resize(width, height);
  root = BuildNodeCopy(other.MyGetRoot());
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  // add your implementation below

   if(this != &other) {
    Clear();
    Copy(other);
  } 
  return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  // add your implementation below
  Clear();
}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {
  // replace the line below with your implementation
  PNG ans;
  ans.resize(root->width, root->height);

  Render(ans, root);
  return ans;
}

void PTree::Render(PNG &img, Node *root) const {
  if(root == NULL) {
    return;
  } else if(root->A == nullptr && root->B == nullptr){
    for(int i = root->upperleft.first; i < (root->width + root->upperleft.first); i++) {
      for(int j = root->upperleft.second; j < (root->height + root->upperleft.second); j++) {
        HSLAPixel *pixel = img.getPixel(i,j);
        *pixel = root->avg;
      }
    }
  } else {
    Render(img, root->A);
    Render(img, root->B);
  }
}

/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
  // add your implementation below
  Prune(root, root, tolerance);
}



void PTree::Prune(Node *og, Node*root,  double tolerance) {

    bool ans = Prunable(root, root, tolerance);

    if(ans) {
      if(root != NULL) {

        Clear(root->A);
        Clear(root->B);
        root->A = nullptr;
        root->B = nullptr;

      }
    } else {  
      Prune(root->A, root->A, tolerance);
      Prune(root->B, root->B, tolerance);
    }

}

bool PTree::Prunable(Node* og, Node *root, double tolerance) {


  bool result = true;

  if(root->A == nullptr && root->B == nullptr) {
    if(og->avg.dist(root->avg) <= tolerance) {
      return true;
    } else {
      return false;
    }
  }

  if(root->A != nullptr) {
    result = Prunable(og, root->A, tolerance);
    if(result == false) {
      return result;
    }
  }

  if(root->B != nullptr) {
    result = Prunable(og, root->B, tolerance);
    if(result == false) {
      return result;
    }
  }

  return result;

}


/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {
  // replace the line below with your implementation
  return SizeLocal(root);
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  // replace the line below with your implementation
  return Leafs(root, 0);
}

/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  // add your implementation below
  PNG *ans = new PNG(Render());

  for(int i = 0; i < ans->width() / 2; i++) {
    for(int j = 0; j < ans->height(); j++) {
      HSLAPixel *pixel1 = ans->getPixel(i, j);
      HSLAPixel pixel1Copy = *(ans->getPixel(i, j));

      HSLAPixel *pixel2 = ans->getPixel(ans->width() - i - 1, j);

      *(ans->getPixel(i,j)) = *pixel2;
      *(ans->getPixel(ans->width() - i - 1,j)) = pixel1Copy;

    }
  }

  Clear();
  int width = ans->width();
  int height = ans->height();
  root = BuildNode(*ans, {0,0} , width , height);
  delete ans;
  
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  // add your implementation below
    PNG ans = Render();

  for(int i = 0; i < ans.height() / 2; i++) {
    for(int j = 0; j < ans.width(); j++) {
      HSLAPixel *pixel1 = ans.getPixel(j, i);
      HSLAPixel pixel1Copy = *(ans.getPixel(j, i));

      HSLAPixel *pixel2 = ans.getPixel(j, ans.height() - i - 1);

      *(ans.getPixel(j,i)) = *pixel2;
      *(ans.getPixel(j,ans.height() - i - 1)) = pixel1Copy;

    }
  }
  
  Clear();
  int width = ans.width();
  int height = ans.height();
  root = BuildNode(ans, {0,0} , width , height);

}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////

Node* PTree::MyGetRoot() const{
  return root;
}


int PTree::Leafs(Node * root, int leafs) const {
  
  int result = leafs;

  if(root->A == nullptr && root-> B == nullptr) {
    return ++result;
  }

  if(root->A != nullptr) {
    result += Leafs(root->A, leafs);
  }

  if(root->B != nullptr) {
    result += Leafs(root->B, leafs);
  }

  return result;
}

int PTree::SizeLocal(Node * root) const {

  int result = 0;

  if(root->A != nullptr) {
    result += SizeLocal(root->A);
  }

  if(root->B != nullptr) {
    result += SizeLocal(root->B);
  }

  return ++result;
}


Node* PTree::BuildNodeCopy(Node* copy) {
  // replace the line below with your implementation

  Node *n = new Node();
  n->width = copy->width;
  n->height = copy->height;
  n->upperleft = copy->upperleft;
  n->avg = copy->avg;

  if(copy->A != NULL) {
    n->A = BuildNodeCopy(copy->A);
    n->B = BuildNodeCopy(copy->B);
  }

  return n;
}
