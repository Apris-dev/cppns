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
		assert(vec[i] == i);
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
		auto parent = sstl::getUnfurled(container.top());
		parent->print();
		if (const auto object = dynamic_cast<const SObject*>(parent)) {
			assert(containerName == object->name);
		}
		container.pop();
	}
	std::cout << std::endl;

	container.clear();

	assert(container.isEmpty());
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
		assert(sstl::getUnfurled(from.top())->id == 100);

		from.transfer(container, const_cast<TType&>(from.top()));

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) {sstl::getUnfurled(obb)->print();}
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) {sstl::getUnfurled(obb)->print();}
		std::cout << std::endl;

		assert(from.isEmpty());
		assert(container.getSize() == 1);
		assert(sstl::getUnfurled(container.top())->id == 100);

		container.clear();

		assert(container.isEmpty());
	}
}

template <typename TContainerType>
void appendTest(const std::string& containerName, TSelfAssociativeContainer<TContainerType>& container) {

	using TType = typename TSelfAssociativeContainer<TContainerType>::TType;

	if constexpr (std::is_copy_constructible_v<TType>) {
		{
			std::cout << "Set Append Test" << std::endl;

			container.push(TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			if constexpr (!TContainerTraits<TContainerType>::bHasHashing) assert(sstl::getUnfurled(container.top())->id == 5);
			container.push(TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			if constexpr (!TContainerTraits<TContainerType>::bHasHashing) assert(sstl::getUnfurled(container.top())->id == 5);
			container.push(TUnfurled<TType>::template create<SObject>((size_t)1, containerName));
			if constexpr (!TContainerTraits<TContainerType>::bHasHashing) assert(sstl::getUnfurled(container.top())->id == 1);

			TSet<TType> from;
			from.push(TUnfurled<TType>::template create<SObject>((size_t)50, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)10, containerName));

			{
				std::vector numbers = {10, 50, 80};

				for (auto& obb : from) {
					int idd = sstl::getUnfurled(obb)->id;
					assert(CONTAINS(numbers, idd));
					ERASE(numbers, idd);
				}

				assert(numbers.empty());
			}

			container.append(from);

			for (const TType& obb : container) {sstl::getUnfurled(obb)->print();}

			// Hashed types do not guarantee order
			if constexpr (!TContainerTraits<TContainerType>::bHasHashing) {
				TContainerType copyOf;
				copyOf.append(container);

				assert(sstl::getUnfurled(copyOf.top())->id == 1);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 5);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 8);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 10);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 50);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 80);
			} else {
				std::vector numbers = {1, 5, 8, 10, 50, 80};

				for (auto& obb : container) {
					int idd = sstl::getUnfurled(obb)->id;
					assert(CONTAINS(numbers, idd));
					ERASE(numbers, idd);
				}

				assert(numbers.empty());
			}

			container.clear();

			assert(container.isEmpty());
		}

		{
			std::cout << "Priority Set Append Test" << std::endl;

			container.push(TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			if constexpr (!TContainerTraits<TContainerType>::bHasHashing) assert(sstl::getUnfurled(container.top())->id == 5);
			container.push(TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			if constexpr (!TContainerTraits<TContainerType>::bHasHashing) assert(sstl::getUnfurled(container.top())->id == 5);
			container.push(TUnfurled<TType>::template create<SObject>((size_t)1, containerName));
			if constexpr (!TContainerTraits<TContainerType>::bHasHashing) assert(sstl::getUnfurled(container.top())->id == 1);

			TPrioritySet<TType> from;
			from.push(TUnfurled<TType>::template create<SObject>((size_t)50, containerName));
			assert(sstl::getUnfurled(from.top())->id == 50);
			from.push(TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			assert(sstl::getUnfurled(from.top())->id == 50);
			from.push(TUnfurled<TType>::template create<SObject>((size_t)10, containerName));
			assert(sstl::getUnfurled(from.top())->id == 10);

			container.append(from);

			for (const TType& obb : container) {sstl::getUnfurled(obb)->print();}

			// Hashed types do not guarantee order
			if constexpr (!TContainerTraits<TContainerType>::bHasHashing) {
				TContainerType copyOf;
				copyOf.append(container);

				assert(sstl::getUnfurled(copyOf.top())->id == 1);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 5);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 8);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 10);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 50);
				copyOf.pop();
				for (const TType& obb : copyOf) {sstl::getUnfurled(obb)->print();}
				assert(sstl::getUnfurled(copyOf.top())->id == 80);
			} else {
				std::vector numbers = {1, 5, 8, 10, 50, 80};

				for (auto& obb : container) {
					int idd = sstl::getUnfurled(obb)->id;
					assert(CONTAINS(numbers, idd));
					ERASE(numbers, idd);
				}

				assert(numbers.empty());
			}

			container.clear();

			assert(container.isEmpty());
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

cppns_main() {

	DO_ASSOCIATIVE_TEST(TSet)
	DO_ASSOCIATIVE_TEST(TMultiSet)
	DO_ASSOCIATIVE_TEST(TPrioritySet)
	DO_ASSOCIATIVE_TEST(TPriorityMultiSet)

	return 0;
}