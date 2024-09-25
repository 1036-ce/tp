#pragma once

#include <utility>

template <typename T> struct default_delete {
	void operator()(T *ptr) noexcept { delete ptr; }
};

template <typename T, typename Deleter = default_delete<T>> class unique_ptr {
public:
	using pointer      = T *;
	using element_type = T;
	using deleter_type = Deleter;

	unique_ptr() : ptr(nullptr), deleter(Deleter{}) {}

	explicit unique_ptr(pointer ptr) : ptr(ptr), deleter(Deleter{}) {}

	unique_ptr(const unique_ptr &) = delete;

	unique_ptr(unique_ptr &&other)
	    : ptr(other.ptr), deleter(std::move(other.deleter)) {
		other.ptr = nullptr;
	}

	~unique_ptr() { deleter(ptr); }

	unique_ptr &operator=(const unique_ptr &) = delete;

	unique_ptr &operator=(std::nullptr_t) {
		if (ptr)
			deleter(ptr);
		ptr = nullptr;
		return *this;
	}

	unique_ptr &operator=(unique_ptr &&other) {
		ptr       = other.ptr;
		deleter   = std::move(other.deleter);
		other.ptr = nullptr;
		return *this;
	}

	pointer release() {
		pointer ret = ptr;
		ptr         = nullptr;
		return ret;
	}

	void reset(pointer p = pointer()) {
		pointer old_ptr = ptr;
		ptr             = p;
		if (old_ptr) {
			deleter(old_ptr);
		}
	}

	void swap(unique_ptr &other) {
		pointer tmp = ptr;
		ptr         = other.ptr;
		other.ptr   = tmp;
	}

	pointer get() const { return ptr; }

	Deleter get_deleter() const { return deleter; }

	explicit operator bool() const { return ptr != nullptr; }

	std::add_lvalue_reference_t<T> operator*() const { return *ptr; }

	pointer operator->() const { return ptr; }

private:
	pointer ptr;
	Deleter deleter;
};

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args &&...args) {
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename Deleter>
void swap(unique_ptr<T, Deleter> &lhs, unique_ptr<T, Deleter> &rhs) {
	lhs.swap(rhs);
}
