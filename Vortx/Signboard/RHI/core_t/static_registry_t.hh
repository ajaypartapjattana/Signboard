#pragma once

template <typename Traits>
class static_registry {
private:

	using _Ty = typename Traits::type;
	using _rootTy = typename Traits::root;
	using _resultTy = typename Traits::result;

	_Ty m_resource{};
	_rootTy r_root{};

public:
	static_registry() noexcept = default;

	explicit static_registry(_rootTy root) noexcept
		:
		r_root(root) 
	{

	}

	static_registry(const static_registry&) = delete;
	static_registry& operator=(const static_registry&) = delete;

	static_registry(static_registry&& other) noexcept
		:
		m_resource(other.m_resource),
		r_root(other.r_root)
	{
		other.m_resource = _Ty{};
	}

	static_registry& operator=(static_registry&& other) noexcept {
		if (this == &other)
			return *this;

		reset();

		m_resource = other.m_resource;
		r_root = other.r_root;

		other.m_resource = _Ty{};

		return *this;
	}

	~static_registry() noexcept {
		reset();
	}

	operator _Ty() const noexcept {
		return m_resource;
	}

	explicit operator bool() const noexcept {
		return m_resource != _Ty{};
	}

	_Ty* data() noexcept {
		return &m_resource;
	}

	const _Ty* data() const noexcept {
		return &m_resource;
	}

	_resultTy create(typename Traits::createInfo* pInfo) noexcept {
		_Ty resource{};
		_resultTy result = Traits::create(r_root, pInfo, &resource);
		if (result != _resultTy{})
			return result;

		reset();

		m_resource = resource;

		return _resultTy{};
	}

	void root(_rootTy root) noexcept {
		reset();
		r_root = root;
	}

	_Ty release() noexcept {
		_Ty resource = m_resource;
		m_resource = _Ty{};
		return resource;
	}

	void reset() noexcept {
		if (m_resource != _Ty{})
			Traits::destroy(r_root, m_resource);

		m_resource = _Ty{};
	}

};