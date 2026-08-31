#include <iostream>
#include <random>
#include <cassert>
#include <cstdint>

#include "TestShared.h"

#ifdef USING_CPPNS_MEMORY
#include "cppns/memory/Memory.h"
#endif

#include "cppns/container/Map.h"
#include "cppns/container/PriorityMap.h"
#include "cppns/container/MultiMap.h"
#include "cppns/container/PriorityMultiMap.h"
#include "cppns/container/Span.h"

enum class MapEnum : uint8_t {
	NONE,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE
};

#ifdef USING_CPPNS_ARCHIVE
inline COutputArchive& operator<<(COutputArchive& inArchive, const MapEnum& obj) {
	inArchive << (size_t)obj;
	return inArchive;
}
#endif

std::string enumToString(const MapEnum val) {
	switch (val) {
		case MapEnum::NONE:
			return "NONE";
		case MapEnum::ONE:
			return "ONE";
		case MapEnum::TWO:
			return "TWO";
		case MapEnum::THREE:
			return "THREE";
		case MapEnum::FOUR:
			return "FOUR";
		case MapEnum::FIVE:
			return "FIVE";
		case MapEnum::SIX:
			return "SIX";
		case MapEnum::SEVEN:
			return "SEVEN";
		case MapEnum::EIGHT:
			return "EIGHT";
		case MapEnum::NINE:
			return "NINE";
	}
	return "";
}

template <typename TContainerType>
void containerTest(const std::string& containerName, TAssociativeContainer<TContainerType>& container) {

	using TType = typename TAssociativeContainer<TContainerType>::TValueType;

	std::vector<size_t> vec;
	for (size_t i = 0; i < 10; ++i) {
		vec.push_back(i);
		assert(vec[i] == i);
	}

	std::random_device rd;
	std::mt19937 rng(rd());

	SHUFFLE(vec, rng);

	size_t i = 0;

	container.resize(10, [&] {
		auto pair = TPair<MapEnum, TType>{(MapEnum)vec[i], TUnfurled<TType>::template create<SObject>(vec[i], containerName)};
		++i;
		return pair;
	});
	assert(container.getSize() == 10);

	const size_t size = container.getSize();
	for (size_t v = 0; v < size; ++v) {
		auto enm = static_cast<MapEnum>(v);
		if (auto parent = sstl::getUnfurled(container.get(enm))) {
			std::cout << "Key: " << enumToString(enm) << " ";
			assert(v == parent->id);
			if (const auto object = dynamic_cast<SObject*>(parent)) {
				assert(containerName == object->name);
			}
			parent->print();
			container.pop(enm);
		}
	}

	std::cout << std::endl;

	container.clear();

	assert(container.isEmpty());
}

template <typename TContainerType>
void transferTest(const std::string& containerName, TAssociativeContainer<TContainerType>& container) {

	using TType = typename TAssociativeContainer<TContainerType>::TValueType;

	{
		std::cout << "Map Transfer Test" << std::endl;

		TMap<MapEnum, TType> from;
		from.push(MapEnum::NONE, TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const auto& obb : from) {
			std::cout << "Key: " << enumToString(obb.first()) << " ";
			sstl::getUnfurled(obb.second())->print();
		}
		std::cout << "to:" << std::endl;
		for (const auto& obb : container) {
			std::cout << "Key: " << enumToString(obb.first()) << " ";
			sstl::getUnfurled(obb.second())->print();
		}

		assert(from.getSize() == 1);
		assert(sstl::getUnfurled(from.get(MapEnum::NONE))->id == 100);

		from.transfer(container, MapEnum::NONE);

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const auto& obb : from) {
			std::cout << "Key: " << enumToString(obb.first()) << " ";
			sstl::getUnfurled(obb.second())->print();
		}
		std::cout << "to:" << std::endl;
		for (const auto& obb : container) {
			std::cout << "Key: " << enumToString(obb.first()) << " ";
			sstl::getUnfurled(obb.second())->print();
		}
		std::cout << std::endl;

		assert(from.isEmpty());
		assert(container.getSize() == 1);
		assert(sstl::getUnfurled(container.get(MapEnum::NONE))->id == 100);

		container.clear();

		assert(container.isEmpty());
	}
}

template <typename TContainerType>
void appendTest(const std::string& containerName, TAssociativeContainer<TContainerType>& container) {

	using TType = typename TAssociativeContainer<TContainerType>::TValueType;

	if constexpr (std::is_copy_constructible_v<TType>) {
		{
			std::cout << "Map Append Test" << std::endl;

			container.push(MapEnum::TWO, TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			container.push(MapEnum::THREE, TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			container.push(MapEnum::ONE, TUnfurled<TType>::template create<SObject>((size_t)1, containerName));

			assert(sstl::getUnfurled(container.get(MapEnum::TWO))->id == 5);
			assert(sstl::getUnfurled(container.get(MapEnum::THREE))->id == 8);
			assert(sstl::getUnfurled(container.get(MapEnum::ONE))->id == 1);

			TMap<MapEnum, TType> from;
			from.push(MapEnum::FIVE, TUnfurled<TType>::template create<SObject>((size_t)50, containerName));
			from.push(MapEnum::SIX, TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			from.push(MapEnum::FOUR, TUnfurled<TType>::template create<SObject>((size_t)10, containerName));

			assert(sstl::getUnfurled(from.get(MapEnum::FIVE))->id == 50);
			assert(sstl::getUnfurled(from.get(MapEnum::SIX))->id == 80);
			assert(sstl::getUnfurled(from.get(MapEnum::FOUR))->id == 10);

			container.append(from);

			assert(sstl::getUnfurled(container.get(MapEnum::FIVE))->id == 50);
			assert(sstl::getUnfurled(container.get(MapEnum::SIX))->id == 80);
			assert(sstl::getUnfurled(container.get(MapEnum::FOUR))->id == 10);

			for (const auto& obb : container) {
				std::cout << "Key: " << enumToString(obb.first()) << " ";
				sstl::getUnfurled(obb.second())->print();
			}

			container.clear();

			assert(container.isEmpty());
		}

		{
			std::cout << "Priority Map Append Test" << std::endl;

			container.push(MapEnum::TWO, TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			container.push(MapEnum::THREE, TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			container.push(MapEnum::ONE, TUnfurled<TType>::template create<SObject>((size_t)1, containerName));

			assert(sstl::getUnfurled(container.get(MapEnum::TWO))->id == 5);
			assert(sstl::getUnfurled(container.get(MapEnum::THREE))->id == 8);
			assert(sstl::getUnfurled(container.get(MapEnum::ONE))->id == 1);

			TPriorityMap<MapEnum, TType> from;
			from.push(MapEnum::FIVE, TUnfurled<TType>::template create<SObject>((size_t)50, containerName));
			from.push(MapEnum::SIX, TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			from.push(MapEnum::FOUR, TUnfurled<TType>::template create<SObject>((size_t)10, containerName));

			assert(sstl::getUnfurled(from.get(MapEnum::FIVE))->id == 50);
			assert(sstl::getUnfurled(from.get(MapEnum::SIX))->id == 80);
			assert(sstl::getUnfurled(from.get(MapEnum::FOUR))->id == 10);

			container.append(from);

			assert(sstl::getUnfurled(container.get(MapEnum::FIVE))->id == 50);
			assert(sstl::getUnfurled(container.get(MapEnum::SIX))->id == 80);
			assert(sstl::getUnfurled(container.get(MapEnum::FOUR))->id == 10);

			for (const auto& obb : container) {
				std::cout << "Key: " << enumToString(obb.first()) << " ";
				sstl::getUnfurled(obb.second())->print();
			}

			container.clear();

			assert(container.isEmpty());
		}
	}
}

#define SINGLE_TEST(...) \
	{ std::cout << std::endl << "--------------------" << std::endl << #__VA_ARGS__ " Test" << std::endl; } \
	{ __VA_ARGS__ container; containerTest(#__VA_ARGS__, container); transferTest(#__VA_ARGS__, container); appendTest(#__VA_ARGS__, container); }

#define DO_MAP_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<MapEnum, Parent>) \
	SINGLE_TEST(x<MapEnum, TShared<Parent>>) \
	SINGLE_TEST(x<MapEnum, TUnique<Parent>>) \
	SINGLE_TEST(x<MapEnum, TUnique<Abstract>>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x container{TPair{MapEnum::NONE, 0}, TPair{MapEnum::ONE, 5}, TPair{MapEnum::TWO, 10}}; for (const auto& pair : container) { std::cout << pair.second() << std::endl; } } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x container{TPair{MapEnum::NONE, TUnique{0}}, TPair{MapEnum::ONE, TUnique{5}}, TPair{MapEnum::TWO, TUnique{10}}}; for (const auto& pair : container) { std::cout << *pair.second().get() << std::endl; } } \

void testSpan(const TSpan<MapEnum, int>& span) {
	for (const auto& val : span) {
		std::cout << val.get().second() << std::endl;
	}
}

cppns_main() {
	DO_MAP_TEST(TMap)
	DO_MAP_TEST(TMultiMap)
	DO_MAP_TEST(TPriorityMap)
	DO_MAP_TEST(TPriorityMultiMap)

	return 0;
}