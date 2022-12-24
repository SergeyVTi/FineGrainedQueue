#include "FineGrainedQueue.h"
#include <future>
#include <thread>
#include <vector>

using namespace std;

constexpr int SIZE = 10000;

FineGrainedQueue que;

void insertThread() {
	for(int i = 0; i < SIZE; i++) {
		que.insertIntoMiddle(i, rand() % SIZE);//
	}
}

void removeThread() {
	for(int i = 0; i < SIZE; i++) {
		que.remove(i);
	}
}

int main() {
	srand(10);

	vector<future<void>> futures;

	que.output = true;//вывод в консоль

	for (size_t i = 0; i < thread::hardware_concurrency() / 2; ++i)
		futures.push_back(async(launch::async, insertThread));

	for (size_t i = 0; i < thread::hardware_concurrency() / 2; ++i)
		futures.push_back(async(launch::async, removeThread));

	for(auto& f : futures)
		f.get();

	cout << "Queue size: " << que.size() << endl;
}