#include "lak/bitset.hpp"

#include "lak/test.hpp"

BEGIN_TEST(bitset)
{
	lak::bitset<lak::endian::native, uint32_t, 8, 16, 30> bitset = {
	  {1, 1, 0, 1, 0, 0, 0}};

	DEBUG(lak::bitset<lak::endian::native, uint32_t, 1, 2, 3>::bit_count);
	DEBUG(lak::bitset<lak::endian::native, uint32_t, 1, 2, 3>::byte_count);

	[[maybe_unused]] auto thing = bitset.get<0>();

	DEBUG(bitset.get<0>());
	DEBUG(bitset.get<1>());
	DEBUG(bitset.get<2>());

	lak::bitset<lak::endian::native, uint32_t, 5, 6, 5> bitset2;
	bitset2.set<0>(0b10101);
	bitset2.set<1>(0b100000);
	bitset2.set<2>(0b11011);
	DEBUG(bitset2.get<0>());
	DEBUG(bitset2.get<1>());
	DEBUG(bitset2.get<2>());

	// std::cout << std::setfill('0');

	DEBUG("little");
	lak::bitset<lak::endian::little, uint8_t, 4, 8, 4> little;
	little.set<0>(0xA);
	little.set<1>(0xB);
	little.set<2>(0xC);
	for (size_t i = 0; i < little._value.size(); ++i)
		// std::cout << setw(2)
		DEBUG(uintmax_t(little._value[i]));

	DEBUG("big");
	lak::bitset<lak::endian::big, uint8_t, 4, 8, 4> big;
	big.set<0>(0xA);
	big.set<1>(0xB);
	big.set<2>(0xC);
	for (size_t i = 0; i < big._value.size(); ++i)
		// std::cout << setw(2)
		DEBUG(uintmax_t(big._value[i]));

	return 0;
}
END_TEST()