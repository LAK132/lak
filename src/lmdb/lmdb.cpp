#include "lak/lmdb/lmdb.hpp"

#include "lak/debug.hpp"

/* --- error --- */

lak::astring lak::lmdb::error::to_string() const
{
	return lak::to_astring(mdb_strerror(value));
}

std::ostream &operator<<(std::ostream &strm, const lak::lmdb::error &err)
{
	return strm << err.to_string();
}

/* --- environment_flags --- */

lak::lmdb::environment_flags lak::lmdb::operator|(
  const lak::lmdb::environment_flags &a, const lak::lmdb::environment_flags &b)
{
	return static_cast<lak::lmdb::environment_flags>(
	  static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

lak::lmdb::environment_flags lak::lmdb::operator&(
  const lak::lmdb::environment_flags &a, const lak::lmdb::environment_flags &b)
{
	return static_cast<lak::lmdb::environment_flags>(
	  static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

/* --- database_flags --- */

lak::lmdb::database_flags lak::lmdb::operator|(
  const lak::lmdb::database_flags &a, const lak::lmdb::database_flags &b)
{
	return static_cast<lak::lmdb::database_flags>(static_cast<unsigned int>(a) |
	                                              static_cast<unsigned int>(b));
}

lak::lmdb::database_flags lak::lmdb::operator&(
  const lak::lmdb::database_flags &a, const lak::lmdb::database_flags &b)
{
	return static_cast<lak::lmdb::database_flags>(static_cast<unsigned int>(a) &
	                                              static_cast<unsigned int>(b));
}

/* --- write_flags --- */

lak::lmdb::write_flags lak::lmdb::operator|(const lak::lmdb::write_flags &a,
                                            const lak::lmdb::write_flags &b)
{
	return static_cast<lak::lmdb::write_flags>(static_cast<unsigned int>(a) |
	                                           static_cast<unsigned int>(b));
}

lak::lmdb::write_flags lak::lmdb::operator&(const lak::lmdb::write_flags &a,
                                            const lak::lmdb::write_flags &b)
{
	return static_cast<lak::lmdb::write_flags>(static_cast<unsigned int>(a) &
	                                           static_cast<unsigned int>(b));
}

/* --- cursor --- */

template<bool READ_ONLY>
lak::lmdb::cursor<READ_ONLY>::cursor(MDB_cursor *csr) : _csr(csr)
{
}

template<bool READ_ONLY>
lak::lmdb::cursor<READ_ONLY>::cursor(cursor &&other)
: _csr(lak::exchange(other._csr, nullptr))
{
}

template<bool READ_ONLY>
lak::lmdb::cursor<READ_ONLY> &lak::lmdb::cursor<READ_ONLY>::operator=(
  cursor &&other)
{
	lak::swap(_csr, other._csr);
	return *this;
}

template<bool READ_ONLY>
lak::lmdb::cursor<READ_ONLY>::~cursor()
{
	if constexpr (READ_ONLY)
		if (_csr) close();
}

template<bool READ_ONLY>
void lak::lmdb::cursor<READ_ONLY>::close()
{
	ASSERT(_csr);
	mdb_cursor_close(_csr);
	_csr = nullptr;
}

template<bool READ_ONLY>
lak::lmdb::result<lak::lmdb::key_value>
lak::lmdb::cursor<READ_ONLY>::operator()(MDB_cursor_op op)
{
	ASSERT(_csr);

	if (op == MDB_SET || op == MDB_SET_KEY || op == MDB_SET_RANGE)
		// must use .at(key) instead
		return lak::err_t{lak::lmdb::error{.value = EINVAL}};

	MDB_val k;
	MDB_val v;

	if (int get_err = mdb_cursor_get(_csr, &k, &v, op); get_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = get_err}};

	return lak::ok_t{lak::lmdb::key_value{
	  .key   = lak::span<const void>(k.mv_data, k.mv_size),
	  .value = lak::span<const void>(v.mv_data, v.mv_size),
	}};
}

template<bool READ_ONLY>
lak::lmdb::result<lak::lmdb::key_value> lak::lmdb::cursor<READ_ONLY>::at(
  lak::span<const void> key)
{
	ASSERT(_csr);

	MDB_val k{.mv_size = key.size(), .mv_data = const_cast<void *>(key.data())};
	MDB_val v;

	if (int get_err = mdb_cursor_get(_csr, &k, &v, MDB_SET_KEY);
	    get_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = get_err}};

	return lak::ok_t{lak::lmdb::key_value{
	  .key   = lak::span<const void>(k.mv_data, k.mv_size),
	  .value = lak::span<const void>(v.mv_data, v.mv_size),
	}};
}

template<bool READ_ONLY>
lak::lmdb::result<lak::lmdb::key_value> lak::lmdb::cursor<READ_ONLY>::at_min(
  lak::span<const void> min_key)
{
	ASSERT(_csr);

	MDB_val k{.mv_size = min_key.size(),
	          .mv_data = const_cast<void *>(min_key.data())};
	MDB_val v;

	if (int get_err = mdb_cursor_get(_csr, &k, &v, MDB_SET_RANGE);
	    get_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = get_err}};

	return lak::ok_t{lak::lmdb::key_value{
	  .key   = lak::span<const void>(k.mv_data, k.mv_size),
	  .value = lak::span<const void>(v.mv_data, v.mv_size),
	}};
}

template struct lak::lmdb::cursor<true>;
template struct lak::lmdb::cursor<false>;

/* --- database --- */

template<bool READ_ONLY>
lak::lmdb::database<READ_ONLY>::database(MDB_txn *txn, MDB_dbi dbi)
: _txn(txn), _dbi(dbi)
{
}

template<bool READ_ONLY>
lak::lmdb::result<lak::span<const void>> lak::lmdb::database<READ_ONLY>::get(
  lak::span<const void> key)
{
	ASSERT(_txn);
	MDB_val k{.mv_size = key.size(), .mv_data = const_cast<void *>(key.data())};
	MDB_val v;
	if (int get_err = mdb_get(_txn, _dbi, &k, &v); get_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = get_err}};
	return lak::ok_t{lak::span<const void>(v.mv_data, v.mv_size)};
}

template<bool READ_ONLY>
lak::lmdb::result<> lak::lmdb::database<READ_ONLY>::put(
  lak::span<const void> key,
  lak::span<const void> value,
  lak::lmdb::write_flags flags)
{
	ASSERT(_txn);
	MDB_val k{.mv_size = key.size(), .mv_data = const_cast<void *>(key.data())};
	MDB_val v{.mv_size = value.size(),
	          .mv_data = const_cast<void *>(value.data())};
	if (int put_err =
	      mdb_put(_txn, _dbi, &k, &v, static_cast<unsigned int>(flags));
	    put_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = put_err}};
	return lak::ok_t{};
}

template<bool READ_ONLY>
lak::lmdb::result<> lak::lmdb::database<READ_ONLY>::del(
  lak::span<const void> key, lak::span<const void> value)
{
	ASSERT(_txn);
	if (value.data() == nullptr)
	{
		using dbf_t   = lak::lmdb::database_flags;
		const dbf_t f = flags().UNWRAP();
		ASSERT_EQUAL((f & dbf_t::dup_sort), dbf_t::dup_sort)
	}
	MDB_val k{.mv_size = key.size(), .mv_data = const_cast<void *>(key.data())};
	MDB_val v{.mv_size = value.size(),
	          .mv_data = const_cast<void *>(value.data())};
	if (int put_err = mdb_del(_txn, _dbi, &k, &v); put_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = put_err}};
	return lak::ok_t{};
}

template<bool READ_ONLY>
lak::lmdb::result<lak::lmdb::database_flags>
lak::lmdb::database<READ_ONLY>::flags()
{
	ASSERT(_txn);
	unsigned int flags = 0;
	if (int flag_err = mdb_dbi_flags(_txn, _dbi, &flags);
	    flag_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = flag_err}};
	return lak::ok_t{static_cast<lak::lmdb::database_flags>(flags)};
}

template<bool READ_ONLY>
lak::lmdb::result<lak::lmdb::cursor<READ_ONLY>>
lak::lmdb::database<READ_ONLY>::open_cursor()
{
	MDB_cursor *csr;
	if (int open_err = mdb_cursor_open(_txn, _dbi, &csr);
	    open_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = open_err}};
	return lak::ok_t{lak::lmdb::cursor<READ_ONLY>(csr)};
}

template struct lak::lmdb::database<true>;
template struct lak::lmdb::database<false>;

/* --- transaction --- */

template<bool READ_ONLY>
lak::lmdb::transaction<READ_ONLY>::transaction(MDB_txn *txn) : _txn(txn)
{
}

template<bool READ_ONLY>
lak::lmdb::transaction<READ_ONLY>::transaction(transaction &&other)
: _txn(lak::exchange(other._txn, nullptr))
{
}

template<bool READ_ONLY>
lak::lmdb::transaction<READ_ONLY>
  &lak::lmdb::transaction<READ_ONLY>::operator=(transaction &&other)
{
	lak::swap(_txn, other._txn);
	return *this;
}

template<bool READ_ONLY>
lak::lmdb::transaction<READ_ONLY>::~transaction()
{
	if (_txn) abort();
}

template<bool READ_ONLY>
lak::lmdb::result<> lak::lmdb::transaction<READ_ONLY>::commit()
{
	ASSERT(_txn);
	if (int commit_err = mdb_txn_commit(_txn); commit_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = commit_err}};
	_txn = nullptr;
	return lak::ok_t{};
}

template<bool READ_ONLY>
void lak::lmdb::transaction<READ_ONLY>::abort()
{
	ASSERT(_txn);
	mdb_txn_abort(_txn);
	_txn = nullptr;
}

template<bool READ_ONLY>
lak::lmdb::result<lak::lmdb::database<READ_ONLY>> lak::lmdb::transaction<
  READ_ONLY>::open_database(const char *name, lak::lmdb::database_flags flags)
{
	ASSERT(_txn);
	MDB_dbi dbi;
	if (int open_err =
	      mdb_dbi_open(_txn, name, static_cast<unsigned int>(flags), &dbi);
	    open_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = open_err}};
	return lak::ok_t{lak::lmdb::database<READ_ONLY>(_txn, dbi)};
}

template<bool READ_ONLY>
lak::lmdb::transaction<READ_ONLY>::operator bool() const
{
	return _txn != nullptr;
}

template struct lak::lmdb::transaction<true>;
template struct lak::lmdb::transaction<false>;

/* --- environment --- */

lak::lmdb::environment::environment(MDB_env *env) : _env(env) {}

lak::lmdb::environment::environment(environment &&other)
: _env(lak::exchange(other._env, nullptr))
{
}

lak::lmdb::environment &lak::lmdb::environment::operator=(environment &&other)
{
	lak::swap(_env, other._env);
	return *this;
}

lak::lmdb::environment::~environment()
{
	if (_env) close();
}

lak::lmdb::result<lak::lmdb::environment> lak::lmdb::environment::open(
  const std::filesystem::path &path,
  MDB_dbi max_dbs,
  environment_flags flags,
  mdb_mode_t mode)
{
	MDB_env *env;

	if (int create_err = mdb_env_create(&env); create_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = create_err}};

	if (int maxdbs_err = mdb_env_set_maxdbs(env, max_dbs);
	    maxdbs_err != MDB_SUCCESS)
	{
		mdb_env_close(env);
		return lak::err_t{lak::lmdb::error{.value = maxdbs_err}};
	}

	if (int open_err = mdb_env_open(
	      env, path.string().c_str(), static_cast<unsigned int>(flags), mode);
	    open_err != MDB_SUCCESS)
	{
		mdb_env_close(env);
		return lak::err_t{lak::lmdb::error{.value = open_err}};
	}

	return lak::ok_t{environment(env)};
}

void lak::lmdb::environment::close()
{
	ASSERT(_env);
	mdb_env_close(_env);
	_env = nullptr;
}

size_t lak::lmdb::environment::max_key_size()
{
	return static_cast<size_t>(mdb_env_get_maxkeysize(_env));
}

lak::lmdb::result<lak::lmdb::rwtransaction>
lak::lmdb::environment::begin_transaction(rwtransaction *parent)
{
	ASSERT(_env);
	MDB_txn *txn;
	if (int begin_err =
	      mdb_txn_begin(_env, parent ? parent->_txn : nullptr, 0, &txn);
	    begin_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = begin_err}};
	return lak::ok_t{lak::lmdb::rwtransaction(txn)};
}

lak::lmdb::result<lak::lmdb::rtransaction>
lak::lmdb::environment::begin_read_only_transaction(rwtransaction *parent)
{
	ASSERT(_env);
	MDB_txn *txn;
	if (int begin_err =
	      mdb_txn_begin(_env, parent ? parent->_txn : nullptr, MDB_RDONLY, &txn);
	    begin_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = begin_err}};
	return lak::ok_t{lak::lmdb::rtransaction(txn)};
}

lak::lmdb::result<lak::lmdb::rtransaction>
lak::lmdb::environment::begin_read_only_transaction(rtransaction *parent)
{
	ASSERT(_env);
	MDB_txn *txn;
	if (int begin_err =
	      mdb_txn_begin(_env, parent ? parent->_txn : nullptr, MDB_RDONLY, &txn);
	    begin_err != MDB_SUCCESS)
		return lak::err_t{lak::lmdb::error{.value = begin_err}};
	return lak::ok_t{lak::lmdb::rtransaction(txn)};
}

lak::lmdb::environment::operator bool() const
{
	return _env != nullptr;
}
