#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <type_traits>

#ifdef _TYPE_TRAITS_
#define C_TYPE_ASSERT(T) \
    static_assert(std::is_trivially_copyable_v<T>); \
    static_assert(std::is_trivially_default_constructible_v<T>); \
    static_assert(std::is_trivially_destructible_v<T>); \
    static_assert(std::is_standard_layout_v<T>)
#endif

namespace type {
	template <typename T>
	class clist {
	private:
		C_TYPE_ASSERT(T);
		struct node {
			node* prev;
			node* next;
			T val;
		};
		node* m_head = nullptr;
		node* m_tail = nullptr;
		std::size_t m_size = 0;
		static node* create_node() noexcept {
			void* const storage = std::malloc(sizeof(node));
			if (!storage)
				return nullptr;
			node* const n = ::new(storage) node;
			n->next = nullptr;
			n->prev = nullptr;
			return n;
		}
		static void destroy_node(node* n) noexcept {
			std::free(n);
		}
		static node* retrieve_node(T* val_ptr) noexcept {
			return reinterpret_cast<node*>(reinterpret_cast<unsigned char*>(val_ptr) - offsetof(node, val));
		}
		node* search_node(const T* val_ptr) const noexcept {
			for (node* n{ m_head }; n; n = n->next)
				if (&n->val == val_ptr)
					return n;
			return nullptr;
		}
		void link_front(node* n) noexcept {
			n->next = m_head;
			if (m_head)
				m_head->prev = n;
			else
				m_tail = n;
			m_head = n;
			++m_size;
		}
		void link_back(node* n) noexcept {
			n->prev = m_tail;
			if (m_tail)
				m_tail->next = n;
			else
				m_head = n;
			m_tail = n;
			++m_size;
		}
		void link_before(node* pos, node* n) noexcept {
			n->prev = pos->prev;
			n->next = pos;
			if (pos->prev)
				pos->prev->next = n;
			else
				m_head = n;
			pos->prev = n;
			++m_size;
		}
		void link_after(node* pos, node* n) noexcept {
			n->next = pos->next;
			n->prev = pos;
			if (pos->next)
				pos->next->prev = n;
			else
				m_tail = n;
			pos->next = n;
			++m_size;
		}
		void unlink_node(node* n) noexcept {
			if (n->prev)
				n->prev->next = n->next;
			else
				m_head = n->next;
			if (n->next)
				n->next->prev = n->prev;
			else
				m_tail = n->prev;
			--m_size;
			destroy_node(n);
		}
	public:
		clist() noexcept = default;
		clist(const clist&) = delete;
		clist& operator=(const clist&) = delete;
		clist(clist&& other) noexcept
			: m_head(other.m_head)
			, m_tail(other.m_tail)
			, m_size(other.m_size)
		{
			other.m_head = nullptr;
			other.m_tail = nullptr;
			other.m_size = nullptr;
		}
		clist& operator=(clist&& other) noexcept {
			if (this == &other)
				return *this;
			clear();
			m_head = other.m_head;
			m_tail = other.m_tail;
			m_size = other.m_size;
			other.m_head = nullptr;
			other.m_tail = nullptr;
			other.m_size = 0;
			return *this;
		}
		~clist() noexcept {
			clear();
		}
		class clist_iterator {
			friend class clist;
			const clist* m_list = nullptr;
			node* m_node = nullptr;
			clist_iterator(clist* pList, node* n) noexcept
				: m_list(pList)
				, m_node(n)
			{}
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;
			clist_iterator() noexcept = default;
			clist_iterator(const clist_iterator& other) noexcept
				: m_list(other.m_list)
				, m_node(other.m_node)
			{}
			reference operator*() const noexcept {
				return m_node->val;
			}
			pointer operator->() const noexcept {
				return &m_node->val;
			}
			clist_iterator& operator++() noexcept {
				if (m_node)
					m_node = m_node->next;
				return *this;
			}
			clist_iterator operator++(int) noexcept {
				clist_iterator old = *this;
				++(*this);
				return old;
			}
			clist_iterator& operator++() noexcept {
				if (m_node)
					m_node = m_node->prev;
				else
					m_node = m_list->m_tail;
				return *this;
			}
			clist_iterator operator++(int) noexcept {
				clist_iterator old = *this;
				--(*this);
				return old;
			}
			bool operator==(const clist_iterator& other) const noexcept {
				return m_node == other.m_node;
			}
			bool operator!=(const clist_iterator& other) const noexcept {
				return !(*this == other);
			}
		};
		using iterator = clist_iterator;
		iterator begin() noexcept {
			return iterator(this, m_head);
		}
		iterator end() noexcept {
			return iterator(this, nullptr);
		}
		using reverse_iterator = std::reverse_iterator<iterator>;
		reverse_iterator rbegin() noexcept {
			return reverse_iterator(end());
		}
		reverse_iterator rend() noexcept {
			return reverse_iterator(begin());
		}
		class clist_const_iterator {
			friend class clist;
			const clist* m_list = nullptr;
			node* m_node = nullptr;
			clist_const_iterator(clist* pList, node* n) noexcept
				: m_list(pList)
				, m_node(n)
			{
			}
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = const T*;
			using reference = const T&;
			clist_const_iterator() noexcept = default;
			clist_const_iterator(const clist_const_iterator& other) noexcept
				: m_list(other.m_list)
				, m_node(other.m_node)
			{
			}
			reference operator*() const noexcept {
				return m_node->val;
			}
			pointer operator->() const noexcept {
				return &m_node->val;
			}
			clist_const_iterator& operator++() noexcept {
				if (m_node)
					m_node = m_node->next;
				return *this;
			}
			clist_const_iterator operator++(int) noexcept {
				clist_const_iterator old = *this;
				++(*this);
				return old;
			}
			clist_const_iterator& operator++() noexcept {
				if (m_node)
					m_node = m_node->prev;
				else
					m_node = m_list->m_tail;
				return *this;
			}
			clist_const_iterator operator++(int) noexcept {
				clist_const_iterator old = *this;
				--(*this);
				return old;
			}
			bool operator==(const clist_const_iterator& other) const noexcept {
				return m_node == other.m_node;
			}
			bool operator!=(const clist_const_iterator& other) const noexcept {
				return !(*this == other);
			}
		};
		using const_iterator = clist_const_iterator;
		const_iterator begin() const noexcept {
			return const_iterator(this, m_head);
		}
		const_iterator end() const noexcept {
			return const_iterator(this, nullptr);
		}
		const_iterator cbegin() const noexcept {
			return begin();
		}
		const_iterator cend() const noexcept {
			return end();
		}
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		const_reverse_iterator rbegin() const noexcept {
			return const_reverse_iterator(end());
		}
		const_reverse_iterator rend() const noexcept {
			return const_reverse_iterator(begin());
		}
		const_reverse_iterator crbegin() const noexcept {
			return const_reverse_iterator(cend());
		}
		const_reverse_iterator crend() const noexcept {
			return const_reverse_iterator(cbegin());
		}
		[[nodiscard]] std::size_t size() const noexcept {
			return m_size;
		}
		[[nodiscard]] bool empty() const noexcept {
			return m_size == 0;
		}
		T* front() noexcept {
			return &m_head->val;
		}
		T* back() noexcept {
			return &m_tail->val;
		}
		const T* front() const noexcept {
			return &m_head->val;
		}
		const T* back() const noexcept {
			return &m_tail->val;
		}
		T* push_front() noexcept {
			node* const n = create_node();
			if (!n)
				return nullptr;
			link_front(n);
			return &n->val;
		}
		T* push_back() noexcept {
			node* const n = create_node();
			if (!n)
				return nullptr;
			link_back(n);
			return &n->val;
		}
		T* insert_before(T* val_ptr) noexcept {
			if (!val_ptr)
				return push_front();
			node* const p = retrieve_node(val_ptr);
			node* const n = create_node();
			if (!n)
				return nullptr;
			link_before(p, n);
			return &n->val;
		}
		T* insert_after(T* val_ptr) noexcept {
			if (!val_ptr)
				return push_back();
			node* const p = retrieve_node(val_ptr);
			node* const n = create_node();
			if (!n)
				return nullptr;
			link_after(p, n);
			return &n->val;
		}
		T* searched_insert_before(T* val_ptr) noexcept {
			if (!val_ptr)
				return push_front();
			node* const p = search_node(val_ptr);
			if (!p)
				return nullptr;
			node* const n = create_node();
			if (!n)
				return nullptr;
			link_before(p, n);
			return &n->val;
		}
		T* searched_insert_after(T* val_ptr) noexcept {
			if (!val_ptr)
				return push_back();
			node* const p = search_node(val_ptr);
			if (!p)
				return nullptr;
			node* const n = create_node();
			if (!n)
				return nullptr;
			link_after(p, n);
			return &n->val;
		}
		void pop_front() noexcept {
			if (m_head)
				unlink_node(m_head);
		}
		void pop_back() noexcept {
			if (m_tail)
				unlink_node(m_tail);
		}
		bool remove(T* val_ptr) noexcept {
			if (!val_ptr)
				return false;
			node* const n = retrieve_node(val_ptr);
			unlink_node(n);
			return true;
		}
		bool searched_remove(T* val_ptr) noexcept {
			if (!val_ptr)
				return false;
			node* const n = search_node(val_ptr);
			if (!n)
				return false;
			unlink_node(n);
			return true;
		}
		void clear() noexcept {
			while (m_head) {
				node* const next = m_head->next;
				destroy_node(m_head);
				m_head = next;
			}
			m_tail = nullptr;
			m_size = 0;
		}
	};
}
