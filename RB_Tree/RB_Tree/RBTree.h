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
struct Small
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
	T * allocate(...);
	void deallocate(T *);
};

template<typename _Key, typename _Value>
struct RB_Tree_Node
{
	RB_Tree_Color color;
	_Key key;
	_Value value;
	RB_Tree_Node *parent, *left, *right;
};

template<typename _Key,
	typename _Value,
	typename _Compare = Small<_Key>,
	typename _Allocate = LazyDelete<RB_Tree_Node<_Key, _Value>>>
	class RBTree
{
private:
	using Node = RB_Tree_Node;

public:
	class iterator
	{
	public:
		iterator();
		~iterator();


	};

private:
	Node * m_pRoot;

private:
	void rotateLeft() noexcept;
	void rotateRight() noexcept;
	

public:
	RBTree();
	RBTree(const RBTree & lhs);
	RBTree(RBTree && rhs);

	clone(const RBTree & lhs) noexcept;
	clear() noexcept;
};

template<typename T>
inline LazyDelete<T>::LazyDelete()
{
}

template<typename T>
inline LazyDelete<T>::~LazyDelete()
{
}

template<typename T>
inline T * LazyDelete<T>::allocate(...)
{
	if (!m_Use.empty())
	{
		size_t index = 0 ,size = m_Use.size();
		for (; index < size; index++)
		{
			if (!m_Use[index])
			{
				m_Use[index] = true;
				return new(m_Data.begin() + index) T(va_list);
			}
		}
	}
	return new T;
}

template<typename T>
inline void LazyDelete<T>::deallocate(T *)
{

}
