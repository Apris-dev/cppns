#include <iostream>
#include <thread>

#include "cppns/container/Vector.h"
#include "cppns/memory/Memory.h"
#include "cppns/util/Threading.h"

#include "TestShared.h"

EXPORTC int run() {

	/*{
		CWorker worker;

		for (int i = 0; i < 10; ++i) {
			worker.add([] {
				std::cout << "Hello World" << std::endl;
			});
		}


		const CPersistentThread thread{worker};

		thread.wait();

		thread.sleep(5000);

		for (int i = 0; i < 10; ++i) {
			worker.add([] {
				std::cout << "Hello World (2)" << std::endl;
			});
		}

		thread.wait();

		return 0;
	}*/

	CWorker worker;
	const CThreadPool pool{worker, 10};

	for (int i = 0; i < 10; ++i) {
		worker.add([] {
			std::cout << "Hello World!" << std::endl;
		});
	}

	pool.wait();

	pool.sleep(1000);

	for (int i = 0; i < 10; ++i) {
		worker.add([] {
			std::cout << "Hello World! (2)" << std::endl;
		});
	}

	pool.wait();

	return 0;

	TThreadSafe<TVector<TUnique<SObject>>> vec;

	char c = 'y';
	while (c != 'n') {
		std::thread threadOne([&] {
			vec->push(TUnique<SObject>{100, "Thread One"});
			vec->top()->print();
			//vec->doFor(0, [](const TUnique<SObject>& obj) { obj->print(); });
			vec->popAt(static_cast<size_t>(0));
		});
		vec->push(TUnique<SObject>{101, "Thread Two"});
		vec->top()->print();
		//vec->doFor(0, [](TUnique<SObject>& obj) { obj->print(); });
		vec->popAt(static_cast<size_t>(0));

		std::cout << "Continue?" << std::endl;
		std::cin >> c;

		threadOne.join();
	}

	vec->clear();

	return 0;
}