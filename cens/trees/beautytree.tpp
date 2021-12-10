#include <climits>

#include "beautytree.h"

#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))

template <class T>
typename BeautyNode<T>::childrenGetterFcn BeautyNode<T>::childrenGetter =
    nullptr;

template <class T>
typename BeautyNode<T>::dataGetterFcn BeautyNode<T>::dataGetter = nullptr;

// static adapter
template <class T>
void BeautyNode<T>::initializeClass(childrenGetterFcn f1, dataGetterFcn f2) {
  childrenGetter = f1;
  dataGetter = f2;
}

template <class T>
std::list<T*> BeautyNode<T>::getChildren() {
  return CALL_MEMBER_FN(*nd, childrenGetter)();
}

template <class T>
std::string BeautyNode<T>::getData() {
  return CALL_MEMBER_FN(*nd, dataGetter)();
}

template <class T>
BeautyNode<T>::BeautyNode(T* node) {
  nd = node;
  depth = 0;

  fcp = 0;
  lcp = 0;
  mcp = 0;

  fbp = 0;
  lbp = 0;

  wself = getData().length();
  wchild = 0;
  wblock = 0;

  std::list<T*> ls = getChildren();
  for (auto& child : ls) {
    std::shared_ptr<BeautyNode> newChild(new BeautyNode(child));
    children.push_back(std::move(newChild));
  }
}

template <class T>
BeautyNode<T>::~BeautyNode() {}

template <class T>
void BeautyNode<T>::printTree(std::shared_ptr<BeautyNode<T>> thisHead) {
  calcDepth(0);

  calcWidth();

  std::vector<std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>> levels;
  groupNodesByDepth(levels, thisHead);

  assignPositionsToNodes(0);

  printLine(0, levels);

  for (std::size_t i = 1; i < levels.size(); i++) {
    printPreline(i, levels);
    printLine(i, levels);
  }
}

template <class T>
void BeautyNode<T>::calcDepth(std::size_t initialDepth) {
  depth = initialDepth;
  for (auto& child : children) {
    child->calcDepth(initialDepth + 1);
  }
}

template <class T>
std::size_t BeautyNode<T>::calcWidth() {
  if (children.empty())  // If there are no children
  {
    wchild = 0;  // Width of the children is 0
  } else {
    wchild += children.front()->calcWidth();

    auto it = ++children.begin();
    for (; it != children.end(); ++it) {
      wchild += 1;
      wchild += (*it)->calcWidth();
    }
  }
  wblock = std::max(wself, wchild);
  return wblock;
}

template <class T>
void BeautyNode<T>::groupNodesByDepth(
    std::vector<std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>>&
        levels,
    std::shared_ptr<BeautyNode<T>> thisHead) {
  std::list<std::shared_ptr<BeautyNode>> queue;
  std::shared_ptr<BeautyNode> nd;

  queue.push_back(thisHead);

  while (!queue.empty()) {
    // Traverse
    nd = queue.front();
    queue.pop_front();
    for (auto& child : nd->children) {
      queue.push_back(child);
    }
    // Add layers to the levels if it is not deep enough
    while (levels.size() <= nd->depth) {
      // Create unique pointer to a new layer (that is: a new List of BeautyNode
      std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>> layer(
          new std::list<std::shared_ptr<BeautyNode>>());
      levels.push_back(std::move(layer));
    }
    levels[nd->depth]->push_back(nd);
  }
}

template <class T>
std::size_t BeautyNode<T>::assignPositionsToNodes(std::size_t blockStart) {
  // Block Data
  fbp = blockStart;
  lbp = blockStart + wblock - 1;

  if (wself >= wchild)  // if this node is longer than the children
  {
    // Do this node first
    fcp = blockStart + (wblock - wself) / 2;
    lcp = fcp + wself - 1;
    mcp = (fcp + lcp) / 2;  // left aligning;

    // Then, do the children
    std::size_t childrenBlockStart = blockStart + (wblock - wchild) / 2;
    for (std::shared_ptr<BeautyNode>& nd : children) {
      childrenBlockStart = nd->assignPositionsToNodes(childrenBlockStart);
    }
  } else  // if the combination of children is longer than this node
  {
    // Do the children first
    std::size_t childrenBlockStart = blockStart + (wblock - wchild) / 2;
    for (std::shared_ptr<BeautyNode>& nd : children) {
      childrenBlockStart = nd->assignPositionsToNodes(childrenBlockStart);
    }

    // Then, do this node
    mcp = (children.front()->mcp + children.back()->mcp) / 2;
    fcp = mcp - (wself - 1) / 2;  // left aligning;
    lcp = fcp + wself - 1;
  }

  return blockStart + wblock + 1;
}

template <class T>
void BeautyNode<T>::printLine(
    std::size_t ln,
    std::vector<std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>>&
        levels) {
  for (std::size_t c = 0; c < wblock; c++) {
    if (CisParentFirstCharacter(c, levels[ln])) {
      c += printNodeStartingAt(c, levels[ln]) - 1;
    } else if (CisBetweenChildrenAndParent(c, levels[ln])) {
      printUnderscore();
    } else {
      printSpace();
    }
  }
  std::cout << std::endl;
}

template <class T>
void BeautyNode<T>::printPreline(
    std::size_t ln,
    std::vector<std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>>&
        levels) {
  for (std::size_t c = 0; c < wblock; c++) {
    if (CisCenterChildren(c, levels[ln])) {
      switch (parentPositionRelToChildren(c, levels[ln - 1], levels[ln])) {
        case -1:
          printLeftChildren();
          break;
        case 0:
          printVerticalChildren();
          break;
        case 1:
          printRightChildren();
          break;
      }
    } else {
      printSpace();
    }
  }
  std::cout << std::endl;
}

// TODO: add in-out (by ref) parameter to function for the parent node (to avoid
template <class T>
bool BeautyNode<T>::CisParentFirstCharacter(
    std::size_t c,
    std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& level) {
  for (std::shared_ptr<BeautyNode>& nd : *level) {
    if (nd->fcp == c) return true;
  }
  return false;
}

template <class T>
std::size_t BeautyNode<T>::printNodeStartingAt(
    std::size_t c,
    std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& level) {
  // Find corresponding node
  for (std::shared_ptr<BeautyNode>& nd : *level) {
    if (nd->fcp == c) {
      return nd->printNodeData();
    }
  }
  return 0;  // TODO: this should be an error
}

template <class T>
bool BeautyNode<T>::CisBetweenChildrenAndParent(
    std::size_t c,
    std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& parent) {
  for (std::shared_ptr<BeautyNode>& nd : *parent) {
    if (nd->children.empty()) {
      continue;
    }
    std::size_t minCp = nd->children.front()->mcp;
    std::size_t maxCp = nd->children.back()->mcp;
    if (c > minCp && c < maxCp) return true;
  }
  return false;
}

// TODO: add in-out (by ref) children
template <class T>
bool BeautyNode<T>::CisCenterChildren(
    std::size_t c,
    std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& children) {
  for (std::shared_ptr<BeautyNode>& nd : *children) {
    if (nd->mcp == c) return true;
  }
  return false;
}

template <class T>
int BeautyNode<T>::parentPositionRelToChildren(
    std::size_t c,
    std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& parent,
    std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& children) {
  std::shared_ptr<BeautyNode> dad = nullptr;
  std::shared_ptr<BeautyNode> kid = nullptr;
  for (std::shared_ptr<BeautyNode>& nd : *parent)  // Find Parent
  {
    if ((c >= nd->fbp) && (c <= nd->lbp)) {
      dad = nd;
    }
  }
  for (auto& nd : *children)  // Find children
  {
    if ((c >= nd->fbp) && (c <= nd->lbp)) {
      kid = nd;
    }
  }
  if (kid->mcp < dad->mcp)
    return -1;
  else if (kid->mcp == dad->mcp)
    return 0;
  else
    return 1;
}

template <class T>
std::size_t BeautyNode<T>::printNodeData() {
  std::cout << getData();
  return wself;
}

template <class T>
void BeautyNode<T>::printSpace() {
  std::cout << " ";
}

template <class T>
void BeautyNode<T>::printUnderscore() {
  std::cout << "_";
}

template <class T>
void BeautyNode<T>::printLeftChildren() {
  std::cout << "/";
}

template <class T>
void BeautyNode<T>::printRightChildren() {
  std::cout << "\\";
}

template <class T>
void BeautyNode<T>::printVerticalChildren() {
  std::cout << "|";
}

// Beauty Tree Class
template <class T>
BeautyTree<T>::BeautyTree(T* n_head, childrenGetterFcn f1, dataGetterFcn f2) {
  BeautyNode<T>::initializeClass(f1, f2);
  head_tree = std::shared_ptr<BeautyNode<T>>(new BeautyNode<T>(n_head));
}

template <class T>
void BeautyTree<T>::print() {
  head_tree->printTree(head_tree);
}