#include "lak/test.hpp"

#include "lak/file.hpp"
#include "lak/lmdb/lmdb.hpp"
#include "lak/string_literals.hpp"

BEGIN_TEST(lmdb)
{
	lak::create_directory("ignore").UNWRAP();

	lak::lmdb::environment env =
	  lak::lmdb::environment::open(
	    "ignore/lmdb.db", 10, lak::lmdb::environment_flags::no_subdir)
	    .UNWRAP();

	{
		SCOPED_CHECKPOINT("dropping root database");

		lak::lmdb::rwtransaction txn = env.begin_rwtransaction().UNWRAP();

		lak::lmdb::rwdatabase db = txn.open_database().UNWRAP();

		db.drop().UNWRAP();

		txn.commit().UNWRAP();
	}

	{
		SCOPED_CHECKPOINT("root database status");

		lak::lmdb::rtransaction txn = env.begin_rtransaction().UNWRAP();

		lak::lmdb::rdatabase db = txn.open_database().UNWRAP();

		const auto stat{db.stat().UNWRAP()};

		DEBUG_EXPR(stat.ms_psize);
		DEBUG_EXPR(stat.ms_depth);
		DEBUG_EXPR(stat.ms_branch_pages);
		DEBUG_EXPR(stat.ms_leaf_pages);
		DEBUG_EXPR(stat.ms_overflow_pages);
		DEBUG_EXPR(stat.ms_entries);
	}

	{
		SCOPED_CHECKPOINT("adding values to root database");

		lak::lmdb::rwtransaction txn = env.begin_rwtransaction().UNWRAP();

		lak::lmdb::rwdatabase db = txn.open_database().UNWRAP();

		db.put("hello"_span, "world"_span);
		db.put("how"_span, "are you"_span);

		txn.commit().UNWRAP();
	}

	{
		SCOPED_CHECKPOINT("reading values from root database");

		lak::lmdb::rtransaction txn = env.begin_rtransaction().UNWRAP();

		lak::lmdb::rdatabase db = txn.open_database().UNWRAP();

		lak::lmdb::rcursor csr = db.open_cursor().UNWRAP();

		auto print_key_val = [](const lak::lmdb::key_value &kv) -> lak::monostate
		{
			DEBUG("key: ", lak::astring_view(lak::span<const char>(kv.key)));
			DEBUG("val: ", lak::astring_view(lak::span<const char>(kv.value)));
			return lak::monostate{};
		};

		if (csr(MDB_FIRST).map(print_key_val).is_ok())
		{
			while (csr(MDB_NEXT).map(print_key_val).is_ok())
				;
		}

		const auto stat{db.stat().UNWRAP()};

		DEBUG_EXPR(stat.ms_psize);
		DEBUG_EXPR(stat.ms_depth);
		DEBUG_EXPR(stat.ms_branch_pages);
		DEBUG_EXPR(stat.ms_leaf_pages);
		DEBUG_EXPR(stat.ms_overflow_pages);
		DEBUG_EXPR(stat.ms_entries);
	}

	return 0;
}
END_TEST()
