#pragma once
#if defined(_DEBUG)|defined(DEBUG)
#else
#define NDEBUG
#endif // defined(_Debug)|defined(Debug)
#include <assert.h>

template<typename T>
struct RBNode
{
	enum Color
	{
		red = 1,
		black
	};
	Color color;
	RBNode<T> *left, *right, *parent;
	T data;
};

template<typename T>
class RBTree
{
private:
	RBNode<T> * m_Root;

private:
	void rotateLeft(RBNode<T> *&& pNode);
	void rotateRight(RBNode<T> *&& pNode);
	void check();
	void clear(RBNode<T> * p);

public:
	RBTree();
	~RBTree();
	bool insert(T && data);
	const RBNode<T> * find(T && data);
	void clear();
	bool isLeaf(const RBNode<T>* && pNode) const;
};

template<typename T>
inline void RBTree<T>::rotateLeft(RBNode<T>*&& pNode)
{
}

template<typename T>
inline void RBTree<T>::rotateRight(RBNode<T>*&& pNode)
{
}

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
inline RBTree<T>::~RBTree()
{
}

template<typename T>
inline bool RBTree<T>::insert(T && data)
{
	return false;
}

template<typename T>
inline const RBNode<T> * RBTree<T>::find(T && data)
{
	RBNode<T> * p = m_Root;
	while (!isLeaf(p))
	{
		if (p->data == data)
		{
			return p;
		}
		else if (p->data > data)
		{
			p = p->right;
		}
		else
		{
			p = p->left;
		}
	}
	return nullptr;
}

template<typename T>
inline void RBTree<T>::clear()
{
	clear(m_Root);
}

template<typename T>
inline bool RBTree<T>::isLeaf(const RBNode<T>*&& pNode) const
{
	assert(pNode);
	return !(pNode->left || pNode->right);
}

