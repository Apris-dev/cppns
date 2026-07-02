add_interface_package(cppns-container
        # Base Classes
        include/cppns/container/Container.h

        # Sequence Containers
        include/cppns/container/Vector.h
        include/cppns/container/Deque.h
        include/cppns/container/List.h
        include/cppns/container/ForwardList.h
        include/cppns/container/Array.h

        # Sorted Sequence Containers
        include/cppns/container/MinHeap.h
        include/cppns/container/MaxHeap.h

        # Container Adaptors
        include/cppns/container/Queue.h
        include/cppns/container/Stack.h

        # Associative Containers
        include/cppns/container/Set.h
        include/cppns/container/MultiSet.h
        include/cppns/container/Map.h
        include/cppns/container/MultiMap.h

        # Sorted Associative Containers
        include/cppns/container/PrioritySet.h
        include/cppns/container/PriorityMultiSet.h
        include/cppns/container/PriorityMap.h
        include/cppns/container/PriorityMultiMap.h

        # Generic
        include/cppns/container/Span.h
)

link_package(INTERFACE cppns-util)
