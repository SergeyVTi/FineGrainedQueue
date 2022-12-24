#include <iostream>
#include <queue>
#include <thread>
#include <string>
#include <condition_variable>
#include <shared_mutex>
#include <mutex>

struct Node {
	Node(const int& val) : value(val), node_mutex(new std::mutex) {
	}
	~Node() {
		delete node_mutex;
	}
	int value;
	Node* next = nullptr;
	std::mutex* node_mutex;
};

class FineGrainedQueue {
		Node* head = nullptr;
		std::shared_mutex* queue_mutex;
		std::mutex* lock_cout;
		std::condition_variable_any data_cond;

	public:
		FineGrainedQueue() : queue_mutex(new std::shared_mutex),
			lock_cout(new std::mutex) {
		}
		~FineGrainedQueue() {
			delete head;
			delete queue_mutex;
			delete lock_cout;
		}

		void remove(int value);
		void insertIntoMiddle(int value, int pos);
		void locked_output(const std::string& str);
		int size();

		bool output = true;
};

