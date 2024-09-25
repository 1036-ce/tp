#pragma once

#include "shared_ptr_base.hpp"

template <typename T> class shared_ptr : public shared_ptr_base<T> {
public:
	shared_ptr() : shared_ptr_base<T>() {}

	// copy ctor
	shared_ptr(const shared_ptr &) = default;

	template <typename Y>
	requires std::constructible_from<shared_ptr_base<T>,
	                                 Y *> explicit shared_ptr(Y *ptr)
	    : shared_ptr_base<T>(ptr) {}

	template <typename Y, typename Deleter>
	requires std::constructible_from<shared_ptr_base<T>, Y *, Deleter>
	shared_ptr(Y *ptr, Deleter del) : shared_ptr_base<T>(ptr, del) {}

	template <typename Y, typename Deleter, typename Alloc>
	requires std::constructible_from<shared_ptr_base<T>, Y *, Deleter, Alloc>
	shared_ptr(Y *ptr, Deleter del, Alloc alloc)
	    : shared_ptr_base<T>(ptr, del, alloc) {}

	template <typename Y>
	requires std::constructible_from < shared_ptr_base<T>,
	const weak_ptr<Y> & > explicit shared_ptr(const weak_ptr<Y> &r)
	    : shared_ptr_base<T>(r) {}

	shared_ptr &operator=(const shared_ptr &other) {
		shared_ptr_base<T>::operator=(other);
		return *this;
	}

	template <typename U, typename... Args>
	friend shared_ptr<U> make_shared(Args &&...args);

private:
	template <typename Alloc, typename... Args>
	shared_ptr(sp_alloc_shared_tag<Alloc> tag, Args &&...args)
	    : shared_ptr_base<T>(tag, std::forward<Args>(args)...) {}
};

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args &&...args) {
	using Alloc = std::allocator<void>;
	Alloc alloc;
	return shared_ptr<T>(sp_alloc_shared_tag<Alloc>{alloc},
	                     std::forward<Args>(args)...);
}

template <typename T> class weak_ptr : public weak_ptr_base<T> {
public:
	weak_ptr() = default;

	// clang-format off
	template <typename Y>
	requires std::constructible_from <weak_ptr_base<T>, const shared_ptr<Y>&> 
	weak_ptr(const shared_ptr<Y> &r): weak_ptr_base<T>(r) {}
	// clang-format on

	weak_ptr(const weak_ptr &) = default;

	shared_ptr<T> lock() const { return shared_ptr<T>(*this); }

private:
};

template <typename T> class enable_shared_from_this {
protected:
	enable_shared_from_this() {}
	enable_shared_from_this(const enable_shared_from_this &) {}
	enable_shared_from_this &operator=(const enable_shared_from_this &) {
		return *this;
	}
	~enable_shared_from_this() {}

public:
	shared_ptr<T> shared_from_this() { return shared_ptr<T>(weak_this); }

	shared_ptr<const T> shared_from_this() const {
		return shared_ptr<const T>(weak_this);
	}

	weak_ptr<T> weak_from_this() { return weak_this; }

	weak_ptr<const T> weak_from_this() const { return weak_this; }

private:
	void weak_assign(T *p, const shared_count& cnt) const {
		weak_this.assign(p, cnt);
	}

	friend const enable_shared_from_this *
	enable_shared_from_this_base(const enable_shared_from_this *p) {
		return p;
	}

	friend class shared_ptr_base<T>;

	mutable weak_ptr<T> weak_this;
};
