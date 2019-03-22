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
<<<<<<< HEAD
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

=======
class RBTree
{
private:
	struct Node
	{
		RB_Tree_Color color;
		Node *left, *right, *parent;
		T data;
	};

public:
	class iterator
	{
	private:
	public:
		iterator();
		~iterator();

		void operator ++();
		void operator --();
		void operator *()const;
		void operator ->()const;
	};

private:
	Node * m_pRoot;				//root ptr
	Node * m_pRub;				//rubbish ptr

private:
	void rotateLeft(RBTree<T>::Node * pNode);			//counterclockwise
	void rotateRight(RBTree<T>::Node * pNode);			//clockwise
	void check();
	void fix();
	void clear(RBTree<T>::Node * p);
	bool isLeaf(const RBTree<T>::Node* pNode) const;

public:
	RBTree();
	RBTree(RBTree<T> & lhs);
	RBTree(RBTree<T> && rhs);
	~RBTree();
	bool insert(T && data);
	bool copy(RBTree<T> & lhs);
	const RBTree<T>::Node * find(T && data);
	void clear();
};

template<typename T>
inline void RBTree<T>::rotateLeft(RBTree<T>::Node * pNode)
{
	assert(pNode);//pNode must not be nullptr
	assert(!isLeaf(pNode));//pNode must not be left node
	auto pParent = pNode->parent;
	auto pRightChild = pNode->right;
	if (pParent)//if parent exists
	{
		//set the right child as pNode's parent's child
		(pParent->left == pNode) ? (pParent->left = pRightChild) : (pParent->right = pRightChild);
	}
	pRightChild->parent = pParent;
	//set the right child's left child as pNode's right child
	pNode->right = pRightChild->left;
	pRightChild->left->parent = pNode;
	//set pNode as right child's left child
	pRightChild->left = pNode;
	pNode->parent = pRightChild;
}

template<typename T>
inline void RBTree<T>::rotateRight(RBTree<T>::Node * pNode)
{
	assert(pNode);//pNode must not be nullptr
	assert(!isLeaf(pNode));//pNode must not be left node
	auto pParent = pNode->parent;
	auto pLeftChild = pNode->left;
	if (pParent)//if parent exists
	{
		//set the left child as pNode's parent' child
		(pParent->left == pNode) ? (pParent->left = pLeftChild) : (pParent->right = pLeftChild);
	}
	pLeftChild->parent = pParent;
	//set the left child's right child as pNode's left child
	pNode->left = pLeftChild->right;
	pLeftChild->right->parent = pNode;
	//set pNode as left child's right child
	pLeftChild->right = pNode;
	pNode->parent = pLeftChild;
}

template<typename T>
inline void RBTree<T>::clear(RBTree<T>::Node* p)
{
	if (p)
	{
		clear(p->left);
		clear(p->right);
		delete p;
	}
}

template<typename T>
inline bool RBTree<T>::isLeaf(const RBTree<T>::Node* pNode) const
{
	assert(pNode);
	return !(pNode->left || pNode->right);
}

template<typename T>
inline RBTree<T>::RBTree() :
	m_pRoot(new RBTree<T>::Node({ RB_Tree_Color::black,nullptr,nullptr, nullptr })),
	m_pRub(nullptr)
{
}

template<typename T>
inline RBTree<T>::RBTree(RBTree<T>& lhs)
{
	copy(lhs);
}

template<typename T>
inline RBTree<T>::RBTree(RBTree<T>&& rhs):
	m_pRoot(rhs.m_pRoot)
{
	rhs.m_pRoot = nullptr;
	
}


template<typename T>
inline RBTree<T>::~RBTree()
{
	clear(m_pRoot);
}

template<typename T>
inline bool RBTree<T>::insert(T && data)
{
	auto p = m_pRoot;
	while (!isLeaf(p))
	{
	}
}

template<typename T>
inline void RBTree<T>::clear()
{
	clear(m_pRoot);
	while (m_pRub)
	{
		auto p = m_pRub;
		m_pRub = m_pRub->right;
		delete p;
	}
}
>>>>>>> parent of 44b6463... Remaster
