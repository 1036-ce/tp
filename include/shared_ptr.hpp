#pragma once

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>     // placement new
#include <utility> // std::is_same_v

class deleter_base {
};

struct ctl_block {
	using size_type = std::size_t;

	explicit ctl_block(size_type _ref_cnt) : ref_cnt(_ref_cnt) {}

	size_type ref_cnt{0};
	deleter_base *deleter;
};

template <typename T> class shared_ptr {
public:
	using element_type = T;

	shared_ptr() = default;

	shared_ptr(T *ptr) : elem(ptr), ctlb(new ctl_block(1)) {}

	template <typename Y> shared_ptr(Y *ptr);

	template <typename Y, typename Deleter> shared_ptr(Y *ptr, Deleter d);

	shared_ptr(const shared_ptr &other) {
		elem = other.elem;
		ctlb = other.ctlb;
		if (ctlb)
			++(ctlb->ref_cnt);
	}

	~shared_ptr() {
		if (ctlb && --(ctlb->ref_cnt) == 0) {
			delete elem;
			delete ctlb;
		}
	}
	shared_ptr &operator=(const shared_ptr &other) {
		elem = other.elem;
		ctlb = other.ctlb;
		if (ctlb)
			++(ctlb->ref_cnt);
		return *this;
	}

	void reset(T *_elem) {
		elem = _elem;
		if (ctlb && --(ctlb->ref_cnt) == 0) {
			delete elem;
			ctlb->ref_cnt = 1;
		}
		ctlb = new ctl_block(1);
	}

	void swap(shared_ptr &lhs);

	element_type *get() const { return elem; }

	element_type &operator*() const { return *elem; }

	element_type *operator->() const { return elem; }

	long use_count() const { return ctlb == nullptr ? 0 : ctlb->ref_cnt; }

	// checks if the stored pointer is not null
	explicit operator bool() const { return elem != nullptr; }

private:
	element_type *elem{nullptr};
	ctl_block *ctlb{nullptr};
};
