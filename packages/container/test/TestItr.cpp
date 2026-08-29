#include <iostream>
#include <vector>

#include "cppns/container/Container.h"
#include "cppns/container/Vector.h"
#include "cppns/container/MaxHeap.h"
#include "cppns/container/MinHeap.h"
#include "cppns/container/Deque.h"
#include "cppns/container/List.h"
#include "cppns/container/ForwardList.h"
#include "cppns/container/Stack.h"
#include "cppns/container/Array.h"
#include "cppns/container/Queue.h"

template <typename TContainerType>
void func(const TSequenceContainer<TContainerType>& val) {
    for (const auto& v : val) {
        std::cout << "Value is: " << v << std::endl;
    }
}

#define DO_TEST(x) \
    std::cout << "-----" #x "-----" << std::endl << std::endl; \
    { x<int> val; val.push(1); val.push(5); val.push(2); val.push(3); func(val); } \
    std::cout << std::endl;

#define DO_ARRAY_TEST(x) \
    std::cout << "-----" #x "-----" << std::endl << std::endl; \
    { x<int, 4> val; val.push(1); val.push(5); val.push(2); val.push(3); func(val); } \
    std::cout << std::endl;

#ifdef USING_BOOTSTRAPPER
EXPORTC int run() {
#else
int main() {
#endif

    DO_TEST(TVector)
    DO_TEST(TMaxHeap)
    DO_TEST(TMinHeap)
    DO_TEST(TDeque)
    DO_TEST(TList)
    DO_TEST(TForwardList)
    DO_ARRAY_TEST(TArray)
    DO_TEST(TStack)
    DO_TEST(TQueue)

    TForwardList test{0, 1};

    auto itr = test.begin();

    ++itr;

    return 0;
}