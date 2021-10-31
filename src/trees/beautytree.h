#ifndef CENS_BEAUTYNODE_H_
#define CENS_BEAUTYNODE_H_

#include <list>
#include <memory>
#include <string>
#include <vector>

template <class T>
class BeautyNode {
 public:
  typedef std::list<T*> (T::*childrenGetterFcn)(void);
  typedef std::string (T::*dataGetterFcn)(void);
  static void initializeClass(childrenGetterFcn f1, dataGetterFcn f2);

 private:
  static childrenGetterFcn childrenGetter;
  static dataGetterFcn dataGetter;

  std::list<T*> getChildren();
  std::string getData();

 public:
  BeautyNode(T* node);

  virtual ~BeautyNode();

  void printTree(std::shared_ptr<BeautyNode<T>> thisHead);

 private:
  T* nd;
  std::list<std::shared_ptr<BeautyNode>> children;
  std::size_t depth;

  std::size_t wself;
  std::size_t wchild;
  std::size_t wblock;

  std::size_t fcp;  // First  Character Position
  std::size_t lcp;  // Last   Character Position
  std::size_t mcp;  // Middle Character Position
  std::size_t fbp;  // First  Block     Position
  std::size_t lbp;  // Last   Block     Position

  void calcDepth(std::size_t initialDepth);
  std::size_t calcWidth();

  void groupNodesByDepth(
      std::vector<std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>>&
          levels,
      std::shared_ptr<BeautyNode<T>> thisHead);
  std::size_t assignPositionsToNodes(std::size_t blockStart);

  void printLine(
      std::size_t ln,
      std::vector<std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>>&
          levels);
  void printPreline(
      std::size_t ln,
      std::vector<std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>>&
          levels);

  std::size_t printNodeStartingAt(
      std::size_t c,
      std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& level);
  std::size_t printNodeData();

  bool CisParentFirstCharacter(
      std::size_t c,
      std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& level);
  bool CisBetweenChildrenAndParent(
      std::size_t c,
      std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& parent);
  bool CisCenterChildren(
      std::size_t c,
      std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& children);
  int parentPositionRelToChildren(
      std::size_t c,
      std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& parent,
      std::unique_ptr<std::list<std::shared_ptr<BeautyNode>>>& children);

  void printSpace();
  void printUnderscore();
  void printLeftChildren();
  void printRightChildren();
  void printVerticalChildren();
};

// Tree Class
template <class T>
class BeautyTree {
 public:
  typedef std::list<T*> (T::*childrenGetterFcn)(void);
  typedef std::string (T::*dataGetterFcn)(void);
  BeautyTree(T* head, childrenGetterFcn f1, dataGetterFcn f2);
  void print();

 private:
  std::shared_ptr<BeautyNode<T>> head_tree;
};

#include "beautytree.tpp"
#endif  // CENS_BEAUTYNODE_H_