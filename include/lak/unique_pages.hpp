#ifndef LAK_UNIQUE_PAGES_HPP
#define LAK_UNIQUE_PAGES_HPP

#include "lak/span.hpp"

namespace lak
{
	struct unique_pages : private lak::span<void>
	{
	private:
		inline unique_pages(lak::span<void> pages) : lak::span<void>(pages) {}

		void clear();

	public:
		static unique_pages make(size_t min_size, size_t *actual_size = nullptr);

		inline ~unique_pages() { clear(); }

		unique_pages() = default;

		inline unique_pages(unique_pages &&other)
		{
			lak::swap<lak::span<void>>(*this, other);
		}

		inline unique_pages &operator=(unique_pages &&other)
		{
			lak::swap<lak::span<void>>(*this, other);
			return *this;
		}

		inline lak::span<void> span() const
		{
			return static_cast<lak::span<void>>(*this);
		}

		using lak::span<void>::data;
		using lak::span<void>::empty;
		using lak::span<void>::size;
		using lak::span<void>::size_bytes;
	};

#if 0
  struct shared_pages : private lak::span<void>
  {
  private:
    lak::reference_count *_count = nullptr;

    shared_pages(lak::span<void> pages)
    {
      ASSERT_GREATER(pages.size(), sizeof(lak::reference_count));
      auto &span = static_cast<lak::span<void> &>(*this);

      span =
        lak::span<void>(static_cast<lak::reference_count *>(pages.data()) + 1,
                        pages.size_bytes() - sizeof(lak::reference_count));

      _count = static_cast<lak::reference_count *>(pages.data());
      new (_count) lak::reference_count();

      ++*_count;
    }

    void clear()
    {
      if (!_count) return;

      static_cast<lak::span<void> &>(*this) = {};

      if (auto &count = *lak::exchange(_count, nullptr); --count)
        ASSERT(lak::page_free(*this));
    }

  public:
    static shared_pages make(size_t min_size)
    {
      ASSERT_GREATER(min_size, 0);
      min_size += sizeof(lak::reference_count);
      auto pages = lak::page_reserve(min_size);
      if (!pages.empty() && pages.size() < min_size)
      {
        ASSERT(lak::page_free(pages));
        pages = {};
      }
      if (pages.empty() || !lak::page_commit(pages)) throw std::bad_alloc();
      return shared_pages(pages);
    }

    ~shared_pages() { clear(); }

    shared_pages() = default;

    shared_pages(const shared_pages &other)
    : lak::span<void>(static_cast<lak::span<void>>(other))
    {
      _count = other._count;
      if (_count) ++*_count;
    }

    shared_pages(shared_pages &&other)
    : lak::span<void>(static_cast<lak::span<void>>(other))
    {
      _count = lak::exchange(other._count, nullptr);
      other.clear();
    }

    shared_pages &operator=(const shared_pages &other)
    {
      static_cast<lak::span<void> &>(*this) = other;

      _count = other._count;
      if (_count) ++*_count;

      return *this;
    }

    shared_pages &operator=(shared_pages &&other)
    {
      lak::swap<lak::span<void>>(*this, other);
      lak::swap(_count, other._count);

      return *this;
    }

    // -Werror=class-conversion bug
    // operator lak::span<void>()
    // {
    //   return static_cast<lak::span<void> &>(*this);
    // }

    using lak::span<void>::data;
    using lak::span<void>::empty;
    using lak::span<void>::size;
    using lak::span<void>::size_bytes;
  };
#endif
}

#endif
