#include <iostream>
#include <random>
#include <cassert>

#include "TestShared.h"
#include "cppns/memory/Memory.h"

#include "cppns/container/Array.h"
#include "cppns/container/Deque.h"
#include "cppns/container/ForwardList.h"
#include "cppns/container/Vector.h"
#include "cppns/container/MaxHeap.h"
#include "cppns/container/MinHeap.h"
#include "cppns/container/List.h"
#include "cppns/container/Queue.h"
#include "cppns/container/Span.h"
#include "cppns/container/Stack.h"

template <typename TContainerType>
void containerTest(const std::string& containerName, TSequenceContainer<TContainerType>& container) {

	using TType = typename TSequenceContainer<TContainerType>::TType;

	std::vector<size_t> vec;
	for (size_t i = 0; i < 10; ++i) {
		vec.push_back(i+500);
		assert(vec[i] == i+500);
	}

	std::random_device rd;
	std::mt19937 rng(rd());

	SHUFFLE(vec, rng);

	container.resize(10, [&](const size_t index) {
		TType obj = TUnfurled<TType>::template create<SObject>(vec[index], containerName);
		return obj;
	});
	assert(container.getSize() == 10);

	const size_t size = container.getSize();
	for (size_t i = 0; i < size; ++i) {
		auto parent = sstl::getUnfurled(container.top());
		parent->print();
		if (const auto object = dynamic_cast<const SObject*>(parent)) {
			assert(containerName == object->name);
		}
		container.pop();
	}
	std::cout << std::endl;

	container.clear();

	if constexpr (!TContainerTraits<TContainerType>::bIsLimitedSize) {
		assert(container.isEmpty());
	}
}


template <typename TContainerType>
void transferTest(const std::string& containerName, TSequenceContainer<TContainerType>& container) {

	using TType = typename TSequenceContainer<TContainerType>::TType;

	{
		std::cout << "Vector Transfer Test" << std::endl;

		TVector<TType> from;
		from.push(TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) { sstl::getUnfurled(obb)->print(); }
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }

		assert(from.getSize() == 1);
		assert(from.contains(100));

		from.transfer(container, 0);

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) { sstl::getUnfurled(obb)->print(); }
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }
		std::cout << std::endl;

		assert(from.isEmpty());
		if constexpr (!TContainerTraits<TContainerType>::bIsLimitedSize) {
			assert(container.getSize() == 1);
		}
		assert(container.contains(100));

		container.clear();

		if constexpr (!TContainerTraits<TContainerType>::bIsLimitedSize) {
			assert(container.isEmpty());
		}
	}

	{
		std::cout << "List Transfer Test" << std::endl;

		TList<TType> from;
		from.push(TUnfurled<TType>::template create<SObject>((size_t)100, containerName));

		std::cout << "Pre Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) { sstl::getUnfurled(obb)->print(); }
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }

		assert(from.getSize() == 1);
		assert(from.contains(100));

		from.transfer(container, 0);

		std::cout << "Post Transfer" << std::endl;
		std::cout << "from:" << std::endl;
		for (const TType& obb : from) { sstl::getUnfurled(obb)->print(); }
		std::cout << "to:" << std::endl;
		for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }
		std::cout << std::endl;

		assert(from.isEmpty());
		if constexpr (!TContainerTraits<TContainerType>::bIsLimitedSize) {
			assert(container.getSize() == 1);
		}
		assert(container.contains(100));

		container.clear();

		if constexpr (!TContainerTraits<TContainerType>::bIsLimitedSize) {
			assert(container.isEmpty());
		}
	}
}

template <typename TContainerType>
void appendTest(const std::string& containerName, TSequenceContainer<TContainerType>& container) {

	using TType = typename TSequenceContainer<TContainerType>::TType;

	if constexpr (std::is_copy_constructible_v<TType> && !TSequenceContainer<TContainerType>::bIsLimitedSize) {
		{
			std::cout << "Vector Append Test" << std::endl;

			container.push(TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)1, containerName));

			assert(container.containsAll(1, 5, 8));

			TVector<TType> from;
			from.push(TUnfurled<TType>::template create<SObject>((size_t)50, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)10, containerName));

			assert(from.containsAll(10, 50, 80));

			container.append(from);

			for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }

			assert(container.containsAll(1, 5, 8, 10, 50, 80));

			container.clear();

			if constexpr (!TContainerTraits<TContainerType>::bIsLimitedSize) {
				assert(container.isEmpty());
			}
		}

		{
			std::cout << "List Append Test" << std::endl;

			container.push(TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)1, containerName));

			assert(container.containsAll(1, 5, 8));

			TList<TType> from;
			from.push(TUnfurled<TType>::template create<SObject>((size_t)50, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)10, containerName));

			assert(from.containsAll(10, 50, 80));

			container.append(from);

			for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }

			assert(container.containsAll(1, 5, 8, 10, 50, 80));

			container.clear();

			if constexpr (!TContainerTraits<TContainerType>::bIsLimitedSize) {
				assert(container.isEmpty());
			}
		}

		{
			std::cout << "Forward List Append Test" << std::endl;

			container.push(TUnfurled<TType>::template create<SObject>((size_t)5, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)8, containerName));
			container.push(TUnfurled<TType>::template create<SObject>((size_t)1, containerName));

			assert(container.containsAll(1, 5, 8));

			TForwardList<TType> from;
			from.push(TUnfurled<TType>::template create<SObject>((size_t)10, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)80, containerName));
			from.push(TUnfurled<TType>::template create<SObject>((size_t)50, containerName));

			assert(from.containsAll(10, 50, 80));

			container.append(from);

			for (const TType& obb : container) { sstl::getUnfurled(obb)->print(); }

			assert(container.containsAll(1, 5, 8, 10, 50, 80));

			container.clear();

			if constexpr (!TContainerTraits<TContainerType>::bIsLimitedSize) {
				assert(container.isEmpty());
			}
		}
	}
}

#define SINGLE_TEST(...) \
	{ std::cout << std::endl << "--------------------" << std::endl << #__VA_ARGS__ " Test" << std::endl; } \
	{ __VA_ARGS__ container; containerTest(#__VA_ARGS__, container); transferTest(#__VA_ARGS__, container); appendTest(#__VA_ARGS__, container); }

#define DO_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<Parent>) \
	SINGLE_TEST(x<TShared<Parent>>) \
	SINGLE_TEST(x<TUnique<Parent>>) \
	SINGLE_TEST(x<TUnique<Abstract>>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x container{0, 5, 10}; for (auto& i : container) { std::cout << i << std::endl; } } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x container{TUnique{0}, TUnique{5}, TUnique{10}}; for (auto& i : container) { std::cout << *i << std::endl; } }


#define DO_ARRAY_TEST(x) \
    std::cout << std::endl << std::endl << "******************** " #x " ********************" << std::endl << std::endl; \
	SINGLE_TEST(x<Parent, 10>) \
	SINGLE_TEST(x<TShared<Parent>, 10>) \
	SINGLE_TEST(x<TUnique<Parent>, 10>) \
	SINGLE_TEST(x<TUnique<Abstract>, 10>) \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Constructor Test" << std::endl; } \
	{ x<int, 3> container{0, 5, 10}; for (auto& i : container) { std::cout << i << std::endl; } } \
	{ std::cout << std::endl << "--------------------" << std::endl << #x " Unique Constructor Test" << std::endl; } \
	{ x<TUnique<int>, 3> container{TUnique{0}, TUnique{5}, TUnique{10}}; for (auto& i : container) { std::cout << *i << std::endl; } }

void testSpan(const TSpan<int>& span) {
	for (const auto& val : span) {
		std::cout << val.get() << std::endl;
	}
}

cppns_main() {

	DO_TEST(TVector)
	DO_TEST(TMaxHeap)
	DO_TEST(TMinHeap)
	DO_TEST(TDeque)
	DO_TEST(TList)
	DO_TEST(TForwardList)
	DO_ARRAY_TEST(TArray)
	DO_TEST(TStack)
	DO_TEST(TQueue)

	return 0;
}