#pragma once

#include <memory>
#include <utility>

class sp_counted_base {
public:
	sp_counted_base() noexcept : use_cnt(1), weak_cnt(1) {}

	virtual ~sp_counted_base() noexcept {}

	// Called when use_count drop to zero, to release the resources
	// managed by *this
	virtual void dispose() noexcept = 0;

	virtual void destroy() noexcept { delete this; }

	// Increment the use count (used when the count is greater than zero).
	void add_ref_copy() { ++use_cnt; }

	void add_ref_lock() {
		if (use_cnt > 0)
			++use_cnt;
	}

	// Decrement the use count
	void release() noexcept {
		if (--use_cnt == 0) {
			dispose();
			if (--weak_cnt == 0) {
				destroy();
			}
		}
	}

	// Called by release when the use count reaches zero
	void release_last_use() noexcept { dispose(); }

	// Increment weak count
	void add_weak_ref_copy() { ++weak_cnt; }

	// Decrement weak count
	void weak_release() {
		if (--weak_cnt == 0) {
			destroy();
		}
	}

	long get_use_count() noexcept { return use_cnt; }

private:
	sp_counted_base(const sp_counted_base &)            = delete;
	sp_counted_base &operator=(const sp_counted_base &) = delete;

	long use_cnt;
	long weak_cnt;
};

// forward declcarations.

template <typename T> class shared_ptr_base;

template <typename T> class weak_ptr_base;

class shared_count;

class weak_count;

// Define operator* and operator-> for shared_ptr<T>
template <typename T> class shared_ptr_access;

template <typename T> class weak_ptr;

template <typename T>
class enable_shared_from_this;

template <typename Ptr> class sp_counted_ptr final : public sp_counted_base {
public:
	explicit sp_counted_ptr(Ptr p) noexcept : ptr(p) {}

	virtual void dispose() noexcept override { delete ptr; }

	virtual void destroy() noexcept { delete this; }

	sp_counted_ptr(const sp_counted_ptr &)            = delete;
	sp_counted_ptr &operator=(const sp_counted_ptr &) = delete;

private:
	Ptr ptr;
};

template <typename Ptr, typename Deleter, typename Alloc>
class sp_counted_deleter : public sp_counted_base {
public:
	class Impl {
	public:
		Impl(Ptr p, Deleter d, const Alloc &a) noexcept
		    : ptr(p), del(std::move(d)), alloc(a) {}

		Ptr ptr;
		Deleter del;
		Alloc alloc;
	};

	using allocator_type = typename std::allocator_traits<
	    Alloc>::template rebind_alloc<sp_counted_deleter>;

	sp_counted_deleter(Ptr p, Deleter d) noexcept
	    : impl(p, std::move(d), Alloc()) {}

	sp_counted_deleter(Ptr p, Deleter d, const Alloc &a) noexcept
	    : impl(p, std::move(d), a) {}

	~sp_counted_deleter() noexcept {}

	virtual void dispose() noexcept override { impl.del(impl.ptr); }

	virtual void destroy() noexcept override {
		this->~sp_counted_deleter(); // call destruction function
		::operator delete(this);     // release resources
	}

private:
	Impl impl;
};

template <typename Tp, typename Alloc>
class sp_counted_inplace : public sp_counted_base {
public:
	class Impl {
	public:
		Impl(Alloc a) noexcept : alloc(a) {}

		Alloc alloc;
		Tp val;
	};

	using allocator_type = typename std::allocator_traits<
	    Alloc>::template rebind_alloc<sp_counted_inplace>;

	template <typename... Args>
	sp_counted_inplace(Alloc a, Args &&...args) : impl(a) {
		std::allocator_traits<Alloc>::construct(a, ptr(),
		                                        std::forward<Args>(args)...);
	}

	virtual void dispose() noexcept override {
		std::allocator_traits<Alloc>::destroy(impl.alloc, ptr());
	}

	virtual void destroy() noexcept override {
		this->~sp_counted_inplace();

		// placement delete;
		::operator delete(this);
	}

	~sp_counted_inplace() noexcept {}

private:
	friend class shared_count;

	Tp *ptr() { return &(impl.val); }

	Impl impl;
};

template <typename Alloc> struct sp_alloc_shared_tag { const Alloc &alloc; };

class shared_count {
public:
	// SFINAE used
	template <typename T> struct not_alloc_shared_tag { using type = void; };

	template <typename T>
	struct not_alloc_shared_tag<sp_alloc_shared_tag<T>> {};

	shared_count() : pi(nullptr) {}

	// sp_counted_ptr
	template <typename Ptr> shared_count(Ptr p) {
		pi = new sp_counted_ptr<Ptr>(p);
	}

	// sp_counted_deleter
	template <typename Ptr, typename Deleter,
	          typename = typename not_alloc_shared_tag<Deleter>::type>
	shared_count(Ptr p, Deleter d)
	    : shared_count(p, d, std::allocator<void>()) {}

	template <typename Ptr, typename Deleter, typename Alloc,
	          typename = typename not_alloc_shared_tag<Deleter>::type>
	shared_count(Ptr p, Deleter d, Alloc a) {
		using sp_cd_type     = sp_counted_deleter<Ptr, Deleter, Alloc>;
		using allocator_type = typename sp_cd_type::allocator_type;

		allocator_type a2(a);
		pi = std::allocator_traits<allocator_type>::allocate(a2, 1);
		pi = ::new (pi) sp_cd_type(p, d, a);
		// std::allocator_traits<allocator_type>::construct(a2, pi, p, d, a);
	}

	// sp_counted_inplace
	template <typename Tp, typename Alloc, typename... Args>
	shared_count(Tp *&p, sp_alloc_shared_tag<Alloc> a, Args &&...args) {
		using sp_ci_type     = sp_counted_inplace<Tp, Alloc>;
		using allocator_type = typename sp_ci_type::allocator_type;

		allocator_type a2(a.alloc);
		sp_ci_type *p1 = std::allocator_traits<allocator_type>::allocate(a2, 1);

		/* std::allocator_traits<Alloc>::construct(
		 *     a.alloc, p1, std::forward<Args>(args)...);  */
		p1 = ::new (p1) sp_ci_type(a.alloc, std::forward<Args>(args)...);
		p  = p1->ptr();
		pi = p1;
	}

	shared_count(const shared_count &other) : pi(other.pi) {
		if (pi != nullptr) {
			pi->add_ref_copy();
		}
	}

	explicit shared_count(const weak_count &other);

	~shared_count() noexcept {
		if (pi != nullptr) {
			pi->release();
		}
	}

	shared_count &operator=(const shared_count &other) {
		sp_counted_base *tmp = other.pi;
		if (tmp != pi) {
			if (tmp != nullptr) {
				tmp->add_ref_copy();
			}
			if (pi != nullptr) {
				pi->release();
			}
			pi = tmp;
		}
		return *this;
	}

	void swap(shared_count& other) {
		sp_counted_base *tmp = pi;
		pi = other.pi;
		other.pi = tmp;
	}

	long get_use_count() { return pi ? pi->get_use_count() : 0; }

private:
	friend class weak_count;
	sp_counted_base *pi;
};

class weak_count {
public:
	weak_count() : pi(nullptr) {}

	weak_count(const shared_count &r) : pi(r.pi) {
		if (pi != nullptr) {
			pi->add_weak_ref_copy();
		}
	}

	weak_count(const weak_count &r) : pi(r.pi) {
		if (pi != nullptr) {
			pi->add_weak_ref_copy();
		}
	}

	weak_count(weak_count &&r) : pi(r.pi) { r.pi = nullptr; }

	~weak_count() {
		if (pi != nullptr)
			pi->weak_release();
	}

	weak_count &operator=(const shared_count &r) {
		sp_counted_base *tmp = r.pi;
		if (tmp != nullptr) {
			tmp->add_weak_ref_copy();
		}
		if (pi != nullptr) {
			pi->weak_release();
		}
		pi = tmp;
		return *this;
	}

	weak_count &operator=(const weak_count &r) {
		sp_counted_base *tmp = r.pi;
		if (tmp != nullptr) {
			tmp->add_weak_ref_copy();
		}
		if (pi != nullptr) {
			pi->weak_release();
		}
		pi = tmp;
		return *this;
	}

	void swap(weak_count& other) {
		sp_counted_base *tmp = pi;
		pi = other.pi;
		other.pi = tmp;
	}

	long get_use_count() const { return pi == nullptr ? 0 : pi->get_use_count(); }

private:
	friend class shared_count;
	sp_counted_base *pi;
};

inline shared_count::shared_count(const weak_count &other) : pi(other.pi) {
	pi->add_ref_lock();
}

template <typename T> class shared_ptr_base : public shared_ptr_access<T> {
public:
	using element_type = T;

	shared_ptr_base() : ptr(nullptr), refcount() {}

	// shared_ptr_base(T *p) : ptr(p), refcount(p) {}

	template <typename Y>
	requires std::convertible_to<Y *, T *>
	explicit shared_ptr_base(Y *p) : ptr(p), refcount(p) {
		static_assert(!std::is_void_v<Y>, "incomplete type");
		static_assert(sizeof(Y) > 0, "incomplete type");
		enable_shared_from_this_with(p);
	}

	template <typename Y, typename Deleter>
	requires std::convertible_to<Y *, T *> shared_ptr_base(Y *p, Deleter d)
	    : ptr(p), refcount(p, std::move(d)) {
			enable_shared_from_this_with(p);
		}

	template <typename Y, typename Deleter, typename Alloc>
	requires std::convertible_to<Y *, T *> shared_ptr_base(Y *p, Deleter d,
	                                                       Alloc a)
	    : ptr(p), refcount(p, std::move(d), std::move(a)) {
			enable_shared_from_this_with(p);
		}

	template <typename Y>
	requires std::convertible_to<Y *, T *>
	shared_ptr_base(const shared_ptr_base<Y> &other)
	    : ptr(other.ptr), refcount(other.refcount) {}

	// TODO : maybe wrong
	template <typename Y>
	requires std::convertible_to<Y *, T *>
	explicit shared_ptr_base(const weak_ptr_base<Y> &other)
	    : refcount(other.refcount) {
		ptr = other.ptr;
	}

	shared_ptr_base &operator=(const shared_ptr_base &other) {
		ptr      = other.ptr;
		refcount = other.refcount;
		return *this;
	}

	void swap(shared_ptr_base& other) {
		std::swap(ptr, other.ptr);
		refcount.swap(other.refcount);
	}

	void reset() {
		shared_ptr_base().swap(*this);
	}

	template <typename Y>
		requires std::convertible_to<Y*, T*>
		void reset(Y* p) {
			shared_ptr_base(p).swap(*this);
		}

	template <typename Y, typename Deleter>
		requires std::convertible_to<Y*, T*>
		void reset(Y* p, Deleter d) {
			shared_ptr_base(p, std::move(d)).swap(*this);
		}

	template <typename Y, typename Deleter, typename Alloc>
		requires std::convertible_to<Y*, T*>
		void reset(Y* p, Deleter d, Alloc a) {
			shared_ptr_base(p, std::move(d), std::move(a)).swap(*this);
		}

	explicit operator bool() const {
		return ptr != nullptr;
	}

	long use_count() { return refcount.get_use_count(); }

	element_type *get() const noexcept { return ptr; }

protected:
	template <typename Alloc, typename... Args>
	shared_ptr_base(sp_alloc_shared_tag<Alloc> tag, Args &&...args)
	    : ptr(nullptr), refcount(ptr, tag, std::forward<Args>(args)...) {
			enable_shared_from_this_with(ptr);
		}

	friend class weak_ptr_base<T>;

private:

	template <typename Y>
		requires std::derived_from<Y, enable_shared_from_this<Y>>
	void enable_shared_from_this_with(Y* p) {
		if (auto base = enable_shared_from_this_base(p)) {
			base->weak_assign(p, refcount);
		}
	  /* if (auto __base = __enable_shared_from_this_base(_M_refcount, __p))
	   *   __base->_M_weak_assign(const_cast<_Yp2*>(__p), _M_refcount); */
	}
	
	template <typename Y>
		requires (!std::derived_from<Y, enable_shared_from_this<Y>>)
	void enable_shared_from_this_with(Y* p) {
	}

	element_type *ptr;
	shared_count refcount;
};

// Define operator* and operator-> for shared_ptr<T>
template <typename T> class shared_ptr_access {
public:
	using element_type = T;

	element_type &operator*() const noexcept { return *get(); }

	element_type *operator->() const noexcept { return get(); }

private:
	element_type *get() const noexcept {
		return static_cast<const shared_ptr_base<T> *>(this)->get();
	}
};

template <typename T> class weak_ptr_base {
public:
	using element_type = std::remove_extent_t<T>;

	weak_ptr_base() : ptr(nullptr), refcount() {}

	// copy ctor
	weak_ptr_base(const weak_ptr_base &) = default;

	template <typename Y>
	weak_ptr_base(const shared_ptr_base<Y> &sp)
	    : ptr(sp.ptr), refcount(sp.refcount) {}

	~weak_ptr_base() = default;

	shared_ptr_base<T> lock() const {
		return shared_ptr_base<element_type>(*this);
	}

	long use_count() const {
		return refcount.get_use_count();
	}

	void swap(weak_ptr_base& other) {
		std::swap(ptr, other.ptr);
		refcount.swap(other.refcount);
	}

	void reset() {
		weak_ptr_base().swap(*this);
	}

	bool expired() const {
		return refcount.get_use_count() == 0;
	}

private:
	void assign(T* p, const shared_count& cnt) {
		if (use_count() == 0) {
			ptr = p;
			refcount = cnt;
		}
	}

	template <typename T1> friend class enable_shared_from_this;

	template <typename T1> friend class shared_ptr_base;

	template <typename T1> friend class weak_ptr_base;

	element_type *ptr;
	weak_count refcount;
};
