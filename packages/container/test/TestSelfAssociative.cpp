#include <iostream>
#include <random>
#include <cassert>

#include "TestShared.h"
#include "cppns/memory/Memory.h"

#include "cppns/container/MultiSet.h"
#include "cppns/container/PriorityMultiSet.h"
#include "cppns/container/Set.h"
#include "cppns/container/PrioritySet.h"
#include "cppns/container/Span.h"

template <typename TContainerType>
void containerTest(const std::string& containerName, TSelfAssociativeContainer<TContainerType>& container) {

	using TType = typename TSelfAssociativeContainer<TContainerType>::TType;

	std::vector<size_t> vec;
	for (size_t i = 0; i < 10; ++i) {
		vec.push_back(i);
	}

	std::random_device rd;
	std::mt19937 rng(rd());

	SHUFFLE(vec, rng);

	size_t i = 0;

	container.resize(10, [&] {
		auto object = TUnfurled<TType>::template create<SObject>(vec[i], containerName);
		++i;
		return object;
	});
	assert(container.getSize() == 10);

	const size_t size = container.getSize();
	for (size_t v = 0; v < size; ++v) {
		sstl::getUnfurled(container.top())->print();
		container.pop();
	}
	std::cout << std::endl;
}

template <typename TContainerType>
void transferTest(const std::string& containerName, TSelfAssociativeContainer<TContainerType>& container) {

	using TType = typename TSelfAssociativeContainer<TContainerType>::TType;

	{
		std::cout << "Set Transfer Test" << std::endl;

		TSet<TType> from;
		from.push(TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) {sstl::getUnfurled(obb)->print();}
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) {sstl::getUnfurled(obb)->print();}

		assert(from.getSize() == 1);

		from.transfer(container, const_cast<TType&>(from.top()));

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) {sstl::getUnfurled(obb)->print();}
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) {sstl::getUnfurled(obb)->print();}
		std::cout << std::endl;

		assert(container.getSize() == 1);

		container.clear();
	}
}

template <typename TContainerType>
void appendTest(const std::string& containerName, TSelfAssociativeContainer<TContainerType>& container) {

	using TType = typename TSelfAssociativeContainer<TContainerType>::TType;

	if constexpr (std::is_copy_constructible_v<TType>) {
		{
			std::cout << "Set Append Test" << std::endl;

			container.push(TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)1, containerName));

			TSet<TType> from;
			from.push(TUnfurled<TType>::template create<SObject>((size_t)50, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)10, containerName));

			container.append(from);
			for (const TType& obb : container) {sstl::getUnfurled(obb)->print();}

			container.clear();
		}

		{
			std::cout << "Priority Set Append Test" << std::endl;

			container.push(TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)1, containerName));

			TPrioritySet<TType> from;
			from.push(TUnfurled<TType>::template create<SObject>((size_t)50, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)10, containerName));

			container.append(from);
			for (const TType& obb : container) {sstl::getUnfurled(obb)->print();}

			container.clear();
		}
	}
}

#define SINGLE_TEST(...) \
	{ std::cout << std::endl << "--------------------" << std::endl << #__VA_ARGS__ " Test" << std::endl; } \
	{ __VA_ARGS__ container; containerTest(#__VA_ARGS__, container); transferTest(#__VA_ARGS__, container); appendTest(#__VA_ARGS__, container); }

#define DO_ASSOCIATIVE_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<Parent>) \
	SINGLE_TEST(x<TShared<Parent>>) \
	SINGLE_TEST(x<TUnique<Parent>>) \
	SINGLE_TEST(x<TUnique<Abstract>>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x container{0, 5, 10}; for (const int& i : container) { std::cout << i << std::endl; } } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x container{TUnique{0}, TUnique{5}, TUnique{10}}; for (const TUnique<int>& i : container) { std::cout << *i.get() << std::endl; } }

void testSpan(const TSpan<int>& span) {
	for (const auto& val : span) {
		std::cout << val.get() << std::endl;
	}
}

EXPORTC int run() {
	/*DO_ASSOCIATIVE_TEST(TSet)
	DO_ASSOCIATIVE_TEST(TMultiSet)
	DO_ASSOCIATIVE_TEST(TPrioritySet)
	DO_ASSOCIATIVE_TEST(TPriorityMultiSet)*/

	TSet<int> vec;

	vec.push(20);
	vec.push(52);
	vec.push(73);
	vec.push(4);

	testSpan(vec);

	return 0;
}