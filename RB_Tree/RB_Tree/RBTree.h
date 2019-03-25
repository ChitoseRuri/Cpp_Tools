#pragma once
#if defined(_DEBUG)|defined(DEBUG)
#else
#define NDEBUG
#endif // defined(_Debug)|defined(Debug)
#include <assert.h>
#include <vector>

enum class RB_Tree_Color
{
	red = 0,
	black
};

template<typename T>
struct Smaller
{
	bool operator()(T rhsl, T rhsr)
	{
		return rhsl < rhsr;
	}
};

template<typename T>
class LazyDelete
{
private:
	std::vector<T*> m_Data;
	std::vector<T*> m_Rub;


public:
	LazyDelete();
	LazyDelete(LazyDelete && rhs);
	~LazyDelete();

	T * allocate() noexcept;
	void deallocate(T *& p);
	size_t size() const noexcept;
	size_t max_size() const noexcept;
	void clear() noexcept;
	void dispose() noexcept;
};

template<typename _Key, typename _Value>
struct Pair
{
	_Key key;
	_Value value;
};

template<typename _Key, typename _Value>
struct RB_Tree_Node
{
	RB_Tree_Color color = RB_Tree_Color::black;
	Pair<_Key, _Value> data;
	RB_Tree_Node *parent, *left, *right;
};	

template<typename _Key,
	typename _Value,
	typename _Compare = Smaller<_Key>,
	typename _Allocate = LazyDelete<RB_Tree_Node<_Key, _Value>>>
	class RBTree
{
private:
	template<typename _Key,typename _Value>
	using Node = RB_Tree_Node<_Key, _Value>;

public:
	class iterator
	{
	private:
		Node<_Key, _Value> *m_p;

	public:
		iterator();
		iterator(Node<_Key, _Value> *p);
		~iterator();

		void operator++();
		void operator--();
		auto operator->();
		auto & operator*();
		bool operator == (const iterator & lhs) const noexcept;
		bool operator == (const iterator && rhs) const noexcept;
		bool operator != (const iterator & lhs) const noexcept;
		bool operator != (const iterator && rhs) const noexcept;
		bool operator < (const iterator & lhs) const noexcept;
		bool operator < (const iterator && rhs) const noexcept;
		bool operator > (const iterator & lhs) const noexcept;
		bool operator > (const iterator && rhs) const noexcept;
	};

private:
	_Allocate m_Allocator;
	_Compare m_Comparator;
	Node<_Key, _Value> * m_pRoot;

private:
	void rotateLeft(Node<_Key,_Value> * const p) noexcept;//counter clockwise
	void rotateRight(Node<_Key, _Value> * const p) noexcept;//clockwise
	void clone(Node<_Key, _Value> *const pDestination, Node<_Key, _Value> *const pSource) noexcept;
	auto & find_try(const _Key & key) noexcept;
	auto & find_try(const _Key && key) noexcept;
	void fix(Node<_Key, _Value> * p);

public:
	RBTree();
	RBTree(const RBTree & lhs);
	RBTree(RBTree && rhs);

	void clone(const RBTree & lhs) noexcept;
	void clear() noexcept;
	void dispose() noexcept;
	auto find(const _Key & key) noexcept;
	auto find(const _Key && key) noexcept;
	bool isLeaf(const Node<_Key, _Value> *const p)const noexcept;
	bool insert(Pair<_Key, _Value> & lhs) noexcept;
	bool insert(Pair<_Key, _Value>&& rhs) noexcept;
};

template<typename T>
inline LazyDelete<T>::LazyDelete()
{
}

template<typename T>
inline LazyDelete<T>::LazyDelete(LazyDelete && rhs) :
	m_Data(std::move(rhs.m_Data)),
	m_Rub(std::move(rhs.m_Rub))
{
}

template<typename T>
inline LazyDelete<T>::~LazyDelete()
{
	dispose();
}

template<typename T>
inline T * LazyDelete<T>::allocate() noexcept
{
	if (!m_Rub.empty())
	{
		m_Data.push_back(m_Rub.back());
		m_Rub.pop_back();
		return new(&m_Data.back()) T();
	}
	else
	{
		m_Data.emplace_back(new T());
		return m_Data.back();
	}
}

template<typename T>
inline void LazyDelete<T>::deallocate(T *& p)
{
	for (size_t i = 0, size = m_Data.size(); i != size; i++)
	{
		if (m_Data[i] == p)
		{
			m_Data[i] = m_Data.back();
			m_Data.pop_back();
			m_Rub.push_back(p);
			p = nullptr;
		}
	}
	assert(false);//Each p should be found in m_Data, never will the program go to this line.
}

template<typename T>
inline size_t LazyDelete<T>::size() const noexcept
{
	return m_Data.size();
}

template<typename T>
inline size_t LazyDelete<T>::max_size() const noexcept
{
	return m_Data.size() + m_Rub.size();
}

template<typename T>
inline void LazyDelete<T>::clear() noexcept
{
	auto sizeD = m_Data.size();
	auto sizeR = m_Rub.size();
	m_Rub.resize(sizeD + sizeR, nullptr);
	memmove_s(m_Rub.data + sizeR, sizeD * sizeof(T*), m_Data.data(), sizeD * sizeof(T*));
	m_Data.clear();
}

template<typename T>
inline void LazyDelete<T>::dispose() noexcept
{
	for (auto & var : m_Data)
	{
		delete var;
	}
	for (auto & var : m_Rub)
	{
		delete var;
	}
	m_Data.clear();
	m_Rub.clear();
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline RBTree<_Key, _Value, _Compare, _Allocate>::iterator::iterator() :
	m_p(nullptr)
{
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline RBTree<_Key, _Value, _Compare, _Allocate>::iterator::iterator(Node<_Key, _Value>* p) :
	m_p(p)
{
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline RBTree<_Key, _Value, _Compare, _Allocate>::iterator::~iterator()
{
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline auto RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator->()
{
	return &(m_p->data);
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline auto & RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator*()
{
	return m_p->data;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator==(const iterator & lhs) const noexcept
{
	return m_p == lhs.m_p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator==(const iterator && rhs) const noexcept
{
	return m_p == rhs.m_p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator!=(const iterator & lhs) const noexcept
{
	return m_p != lhs.m_p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator!=(const iterator && rhs) const noexcept
{
	return m_p != rhs.m_p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator<(const iterator & lhs) const noexcept
{
	return m_p < lhs.m_p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator<(const iterator && rhs) const noexcept
{
	return m_p < rhs.m_p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator>(const iterator & lhs) const noexcept
{
	return m_p > lhs.m_p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::iterator::operator>(const iterator && rhs) const noexcept
{
	return m_p > rhs.m_p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline void RBTree<_Key, _Value, _Compare, _Allocate>::rotateLeft(Node<_Key, _Value>* const p) noexcept
{
	auto parent = p->parent;
	auto rightChild = p->right;
	if (parent)//if p's parent is not nullptr
	{
		(p == parent->left) ? (parent->left = rightChild) : (parent->right = rightChild);
	}
	rightChild->parent = parent;
	p->right = rightChild->left;
	p->right->parent = p;
	rightChild->left = p;
	p->parent = rightChild;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline void RBTree<_Key, _Value, _Compare, _Allocate>::rotateRight(Node<_Key, _Value>* const p) noexcept
{
	auto parent = p->parent;
	auto leftChild = p->left;
	if (parent)//if p's parent is not nullptr
	{
		(p == parent->left) ? (parent->left = leftChild) : (parent->right = leftChild);
	}
	leftChild->parent = parent;
	p->left = leftChild->right;
	p->left->parent = p;
	leftChild->right = p;
	p->parent = leftChild;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline void RBTree<_Key, _Value, _Compare, _Allocate>::clone(Node<_Key, _Value>* const pDestination, Node<_Key, _Value>* const pSource) noexcept
{
	assert(pDestination && pSource);//both ptrs will not be nullptr
	if (isLeaf(pSource))			//return when pSource is leaf node
	{
		return;
	}
	pDestination->data = pSource->data;
	pDestination->color = pSource->color;
	pDestination->left = m_Allocator.allocate();
	pDestination->left->parent = pDestination;
	pDestination->right = m_Allocator.allocate();
	pDestination->right->parent = pDestination;
	clone(pDestination->left, pSource->left);
	return clone(pDestination->right, pSource->right);//tail return
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline auto & RBTree<_Key, _Value, _Compare, _Allocate>::find_try(const _Key & key) noexcept
{
	auto p = m_pRoot;
	if (!p)
	{
		return m_pRoot;
	}
	while (true)
	{
		if (key == p->data.key)
		{
			return p;
		}
		else if (m_Comparator(key, p->data.key))
		{
			if (p->left)
			{
				p = p->left;
			}
			else
			{
				return p->left;
			}
		}
		else
		{
			if (p->right)
			{
				p = p->right;
			}
			else
			{
				return p->right;
			}
		}
	}
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline auto & RBTree<_Key, _Value, _Compare, _Allocate>::find_try(const _Key && key) noexcept
{
	auto p = m_pRoot;
	if (!p)
	{
		return m_pRoot;
	}
	while (true)
	{
		if (key == p->data.key)
		{
			return p;
		}
		else if (m_Comparator(key, p->data.key))
		{
			if (p->left)
			{
				p = p->left;
			}
			else
			{
				return p->left;
			}
		}
		else
		{
			if (p->right)
			{
				p = p->right;
			}
			else
			{
				return p->right;
			}
		}
	}
	assert(false);
	return nullptr;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline void RBTree<_Key, _Value, _Compare, _Allocate>::fix(Node<_Key, _Value>* p)
{
	auto parent = p->parent;
	if (!parent)
	{
		p->color = RB_Tree_Color::black;
		return;
	}
	if (parent->color == RB_Tree_Color::black)
	{
		return;
	}
	else//If it's parent's color is red, it must has grandparent.
	{
		auto grap = parent->parent;
		auto uncle = (parent == grap->left) ? (grap->right) : (grap->left);
		if (uncle->color == RB_Tree_Color::red)
		{
			parent->color = RB_Tree_Color::red;
			uncle->color = RB_Tree_Color::black;
			grap->color = RB_Tree_Color::red;
			return fix(grap);
		}
		if (uncle->color == RB_Tree_Color::black)
		{
			if (p == parent->left)
			{
				return rotateLeft(parent);
			}
			else// p == parent->right
			{
				parent->color = RB_Tree_Color::black;
				grap->color = RB_Tree_Color::red;
				return rotateRight(grap);
			}
		}
	}
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline RBTree<_Key, _Value, _Compare, _Allocate>::RBTree():
	m_pRoot(nullptr)
{	
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline RBTree<_Key, _Value, _Compare, _Allocate>::RBTree(const RBTree & lhs):
	m_pRoot(m_Allocator.allocate())//Initial root before clone
{
	clone(lhs);
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline RBTree<_Key, _Value, _Compare, _Allocate>::RBTree(RBTree && rhs) :
	m_pRoot(rhs.m_pRoot),
	m_Allocator(std::move(rhs.m_Allocator))
{
	rhs.m_pRoot = nullptr;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline void RBTree<_Key, _Value, _Compare, _Allocate>::clone(const RBTree & lhs) noexcept
{
	if (!isLeaf(m_pRoot))
	{
		clear();
	}
	clone(m_pRoot, lhs.m_pRoot);
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline void RBTree<_Key, _Value, _Compare, _Allocate>::clear() noexcept
{
	m_pRoot = nullptr;
	m_Allocator.clear();
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline void RBTree<_Key, _Value, _Compare, _Allocate>::dispose() noexcept
{
	m_pRoot = nullptr;
	m_Allocator.dispose();
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline auto RBTree<_Key, _Value, _Compare, _Allocate>::find(const _Key & key) noexcept
{
	auto p = find_try(key);
	return iterator(isLeaf(p) ? nullptr : p);
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline auto RBTree<_Key, _Value, _Compare, _Allocate>::find(const _Key && key) noexcept
{
	auto p = find_try(key);
	return iterator(isLeaf(p) ? nullptr : p);
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::isLeaf(const Node<_Key, _Value>* const p) const noexcept
{
	return !p;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::insert(Pair<_Key, _Value>& lhs) noexcept
{
	auto & p = find_try(lhs.key);
	if (!isLeaf(p))	// That means this key exists.
	{
		return false;
	}
	// If p is leaf node, it is the node we want to insert.
	p = m_Allocator.allocate();
	p->data = lhs;
	p->color = RB_Tree_Color::red;
	if (p->left)
	{
		p->left->parent = p;
	}
	if (p->right)
	{
		p->right->parent = p;
	}
	fix(p);
	return true;
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline bool RBTree<_Key, _Value, _Compare, _Allocate>::insert(Pair<_Key, _Value>&& rhs) noexcept
{
	auto & p = find_try(rhs.key);
	if (!isLeaf(p))	// That means this key exists.
	{
		return false;
	}
	p = m_Allocator.allocate();
	p->data = std::move(rhs);
	p->color = RB_Tree_Color::red;
	if (p->left)
	{
		p->left->parent = p;
	}
	if (p->right)
	{
		p->right->parent = p;
	}
	fix(p);
	return true;
}