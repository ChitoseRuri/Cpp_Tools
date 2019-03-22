#pragma once
#if defined(_DEBUG)|defined(DEBUG)
#else
#define NDEBUG
#endif // defined(_Debug)|defined(Debug)
#include <assert.h>
#include <vector>
#include <stdarg.h>

enum class RB_Tree_Color
{
	red = 0,
	black
};

template<typename T>
struct Smaller
{
	bool operator()(T && lhs, T && rhs)
	{
		return lhs < rhs;
	}
};

template<typename T>
class LazyDelete
{
private:
	std::vector<T*> m_Data;
	std::vector<bool> m_Use;


public:
	LazyDelete();
	~LazyDelete();
	T * allocate();
	void deallocate(T *& p);
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
	RB_Tree_Color color;
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
		~iterator();

		void operator++();
		void operator--();
		auto operator->();
		auto & operator*();
	};

private:
	Node<_Key, _Value> * m_pRoot;

private:
	void rotateLeft() noexcept;
	void rotateRight() noexcept;
	

public:
	RBTree();
	RBTree(const RBTree & lhs);
	RBTree(RBTree && rhs);

	void clone(const RBTree & lhs) noexcept;
	void clear() noexcept;
};

template<typename T>
inline LazyDelete<T>::LazyDelete()
{
}

template<typename T>
inline LazyDelete<T>::~LazyDelete()
{
	for (auto & var : m_Data)
	{
		delete var;
	}
}

template<typename T>
inline T * LazyDelete<T>::allocate()
{

	size_t index = 0, size = m_Use.size();
	for (; index < size; index++)
	{
		if (!m_Use[index])
		{
			m_Use[index] = true;
			return new(&m_Data[index]) T();
		}
	}
	m_Data.emplace_back(new T());
	return m_Data.back() - 1;
}

template<typename T>
inline void LazyDelete<T>::deallocate(T *& p)
{
	for (size_t size = m_Data.size(), i = 0; i != size; i++)
	{
		if (m_Data[i] == p)
		{
			m_Use[i] = false;
			p = nullptr;
		}
	}
	assert(false);//never goes to this
}

template<typename _Key, typename _Value, typename _Compare, typename _Allocate>
inline RBTree<_Key, _Value, _Compare, _Allocate>::iterator::iterator() :
	m_p(nullptr)
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
