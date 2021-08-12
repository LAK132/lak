#ifndef LAK_LMDB_HPP
#define LAK_LMDB_HPP

#include "lak/result.hpp"
#include "lak/string_view.hpp"

#include "lmdb.h"

#include <filesystem>

namespace lak
{
	namespace lmdb
	{
		struct error
		{
			int value;

			lak::astring to_string() const;
		};

		template<typename T = lak::monostate>
		using result = lak::result<T, lak::lmdb::error>;

		enum struct environment_flags : unsigned int
		{
			none          = 0,
			fixed_map     = MDB_FIXEDMAP,
			no_subdir     = MDB_NOSUBDIR,
			no_sync       = MDB_NOSYNC,
			read_only     = MDB_RDONLY,
			no_meta_sync  = MDB_NOMETASYNC,
			write_map     = MDB_WRITEMAP,
			map_async     = MDB_MAPASYNC,
			no_tls        = MDB_NOTLS,
			no_lock       = MDB_NOLOCK,
			no_read_ahead = MDB_NORDAHEAD,
			no_mem_init   = MDB_NOMEMINIT,
		};

		environment_flags operator|(const environment_flags &a,
		                            const environment_flags &b);
		environment_flags operator&(const environment_flags &a,
		                            const environment_flags &b);

		enum struct database_flags : unsigned int
		{
			none         = 0,
			reverse_key  = MDB_REVERSEKEY,
			dup_sort     = MDB_DUPSORT,
			interger_key = MDB_INTEGERKEY,
			dup_fixed    = MDB_DUPFIXED,
			integer_dup  = MDB_INTEGERDUP,
			reverse_dup  = MDB_REVERSEDUP,
			create       = MDB_CREATE,
		};

		database_flags operator|(const database_flags &a, const database_flags &b);
		database_flags operator&(const database_flags &a, const database_flags &b);

		enum struct write_flags : unsigned int
		{
			none         = 0,
			no_overwrite = MDB_NOOVERWRITE,
			no_dup_date  = MDB_NODUPDATA,
			current      = MDB_CURRENT,
			reserve      = MDB_RESERVE,
			append       = MDB_APPEND,
			append_dup   = MDB_APPEND,
			multiple     = MDB_MULTIPLE,
		};

		write_flags operator|(const write_flags &a, const write_flags &b);
		write_flags operator&(const write_flags &a, const write_flags &b);

		template<bool READ_ONLY>
		struct cursor;
		template<bool READ_ONLY>
		struct database;
		template<bool READ_ONLY>
		struct transaction;
		struct environment;

		struct key_value
		{
			lak::span<const void> key;
			lak::span<const void> value;
		};

		template<bool READ_ONLY>
		struct cursor
		{
		private:
			MDB_cursor *_csr = nullptr;

			template<bool READ_ONLY>
			friend struct database;

			cursor(MDB_cursor *csr);

		public:
			cursor() = default;
			cursor(cursor &&other);
			cursor &operator=(cursor &&other);
			~cursor();

			// if part of a write transaction, must be called before the end of the
			// transaction.
			void close();

			lak::lmdb::result<key_value> operator()(MDB_cursor_op op);
			lak::lmdb::result<key_value> at(lak::span<const void> key);
			lak::lmdb::result<key_value> at_min(lak::span<const void> min_key);
		};

		using rwcursor = cursor<false>;
		using rcursor  = cursor<true>;

		extern template struct cursor<true>;
		extern template struct cursor<false>;

		template<bool READ_ONLY>
		struct database
		{
		private:
			MDB_txn *_txn;
			MDB_dbi _dbi;

			template<bool READ_ONLY>
			friend struct transaction;

			database(MDB_txn *txn, MDB_dbi dbi);

		public:
			database(const database &other) = default;
			database &operator=(const database &other) = default;

			lak::lmdb::result<lak::span<const void>> get(lak::span<const void> key);

			lak::lmdb::result<> put(
			  lak::span<const void> key,
			  lak::span<const void> value,
			  lak::lmdb::write_flags flags = lak::lmdb::write_flags::none);

			// value is ignored if duplicate items is not supported.
			lak::lmdb::result<> del(lak::span<const void> key,
			                        lak::span<const void> value = {});

			lak::lmdb::result<database_flags> flags();

			lak::lmdb::result<cursor<READ_ONLY>> open_cursor();
		};

		using rwdatabase = database<false>;
		using rdatabase  = database<true>;

		extern template struct database<true>;
		extern template struct database<false>;

		template<bool READ_ONLY>
		struct transaction
		{
		private:
			MDB_txn *_txn = nullptr;

			template<bool READ_ONLY>
			friend struct database;
			friend struct environment;

			transaction(MDB_txn *txn);

		public:
			transaction() = default;
			transaction(transaction &&other);
			transaction &operator=(transaction &&other);
			~transaction();

			lak::lmdb::result<> commit();

			void abort();

			lak::lmdb::result<database<READ_ONLY>> open_database(
			  const char *name                = nullptr,
			  lak::lmdb::database_flags flags = lak::lmdb::database_flags::none);

			operator bool() const;
		};

		using rwtransaction = transaction<false>;
		using rtransaction  = transaction<true>;

		extern template struct transaction<true>;
		extern template struct transaction<false>;

		struct environment
		{
		private:
			MDB_env *_env = nullptr;

			environment(MDB_env *env);

		public:
			environment() = default;
			environment(environment &&other);
			environment &operator=(environment &&other);
			~environment();

			static lak::lmdb::result<environment> open(
			  const std::filesystem::path &path,
			  MDB_dbi max_dbs         = 1,
			  environment_flags flags = environment_flags::none,
			  mdb_mode_t mode         = 0);

			void close();

			size_t max_key_size();

			lak::lmdb::result<rwtransaction> begin_transaction(
			  rwtransaction *parent = nullptr);

			lak::lmdb::result<rtransaction> begin_read_only_transaction(
			  rwtransaction *parent = nullptr);

			lak::lmdb::result<rtransaction> begin_read_only_transaction(
			  rtransaction *parent);

			operator bool() const;
		};
	}
}

#include <ostream>

std::ostream &operator<<(std::ostream &strm, const lak::lmdb::error &err);

#endif
