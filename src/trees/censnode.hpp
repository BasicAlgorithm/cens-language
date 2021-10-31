#ifndef CENS_CENSNODE_HPP_
#define CENS_CENSNODE_HPP_

#include <iostream>
#include <list>

class CensNode {
 public:
  CensNode();
  CensNode(std::string data);

  virtual ~CensNode();

  void addChildren(CensNode *nd);
  void setData(std::string data);

  std::list<CensNode *> getChildren();
  std::string getData();

  CensNode *parent;
  std::list<CensNode *> children;

  std::string data;

 protected:
  void setParent(CensNode *nd);
};

// =========================================================

CensNode::CensNode() { parent = nullptr; }

CensNode::CensNode(std::string n_data) {
  this->parent = nullptr;
  this->data = n_data;
}

CensNode::~CensNode() {
  for (auto it = children.begin(); it != children.end(); ++it) {
    delete (*it);
  }
}

void CensNode::addChildren(CensNode *new_n) {
  children.push_front(new_n);
  new_n->setParent(this);
}

std::list<CensNode *> CensNode::getChildren() { return children; }

std::string CensNode::getData() { return data; }

void CensNode::setData(std::string data) { this->data = data; }

void CensNode::setParent(CensNode *nd) { parent = nd; }

#endif  // CENS_CENSNODE_HPP_
