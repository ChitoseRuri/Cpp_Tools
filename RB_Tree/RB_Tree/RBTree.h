#pragma once
#if defined(_DEBUG)|defined(DEBUG)
#else
#define NDEBUG
#endif // defined(_Debug)|defined(Debug)
#include <assert.h>

enum  class RB_Tree_Color
{
	red = 0,
	black
};

template<typename T>
struct RBNode
{
	RB_Tree_Color color;
	RBNode<T> *left, *right, *parent;
	T data;
};

template<typename T>
class RBTree
{
private:
	RBNode<T> * m_Root;
	struct k
	{

	};

private:
	void rotateLeft(RBNode<T> * pNode);
	void rotateRight(RBNode<T> * pNode);
	void check();
	void clear(RBNode<T> * p);

public:
	RBTree();
	RBTree(const RBTree<T> & lhs);
	RBTree(RBTree<T> && rhs);
	~RBTree();
	bool insert(T && data);
	void clear();
	bool isLeaf(const RBNode<T>* pNode) const;

};

template<typename T>
inline void RBTree<T>::clear(RBNode<T>* p)
{
	if (p)
	{
		clear(p->left);
		clear(p->right);
		delete p;
	}
}

template<typename T>
inline RBTree<T>::RBTree() :
	m_Root(new RBNode<T>({ RBNode<T>::Color::black,nullptr,nullptr, nullptr }))
{
}

template<typename T>
inline RBTree<T>::RBTree(const RBTree<T> & lhs)
{

}

template<typename T>
inline RBTree<T>::RBTree(RBTree<T> && rhs):
	m_Root(rhs.m_Root)
{
	rhs.m_Root = nullptr;
}

template<typename T>
inline RBTree<T>::~RBTree()
{
}

template<typename T>
inline bool RBTree<T>::insert(T && data)
{
	return false;
}

template<typename T>
inline void RBTree<T>::clear()
{
	clear(m_Root);
}

template<typename T>
inline bool RBTree<T>::isLeaf(const RBNode<T>* pNode) const
{
	assert(pNode);
	return !(pNode->left || pNode->right);
}

