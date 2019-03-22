#pragma once
#if defined(_DEBUG)|defined(DEBUG)
#else
#define NDEBUG
#endif // defined(_Debug)|defined(Debug)
#include <assert.h>

enum class RB_Tree_Color
{
	red = 0,
	black
};

template<typename T>
struct Compare
{
	bool operator()(T && lhs, T && rhs)
	{
		return lhs < rhs;
	}
};

template<typename T>
class Allocator
{
private:

public:
	T& operator ()
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
	typename _Compare = Compare<_Key>,
	typename _Allocate = Allocator<RB_Tree_Node<_Key, _Value>>>
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