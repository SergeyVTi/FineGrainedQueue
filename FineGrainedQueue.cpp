#include "FineGrainedQueue.h"

using namespace std;

void FineGrainedQueue::insertIntoMiddle(int value, int pos)
{
	int counter = 0;
	Node *prev = nullptr;
	Node *cur = nullptr;
	Node *node = nullptr;

	{
		unique_lock<shared_mutex> lock(*queue_mutex);

		// обработка случая пустого списка
		if (!head)
		{
			head = new Node(value);
			locked_output("insert: empty queue insertion, val: " + to_string(
																	   value));
			return;
		}

		prev = this->head;

		// обработка вставки первого элемента
		if (pos == 0)
		{
			node = new Node(value);
			head = node;
			head->next = prev;

			locked_output("insert: 1 pos insertion, val: " + to_string(value));
			return;
		}

		if (prev->next)
		{
			cur = prev->next;
		}

		counter = 1;

		prev->node_mutex->lock();

		if (cur)
			cur->node_mutex->lock();
		else
		{
			node = new Node(value);
			prev->next = node;

			prev->node_mutex->unlock();

			locked_output("insert: end queue insertion, val: " + to_string(value));
			return;
		}
	}

	while (cur)
	{
		if (pos == counter)
		{
			node = new Node(value);
			node->next = cur;
			prev->next = node;

			prev->node_mutex->unlock();
			cur->node_mutex->unlock();

			locked_output("insert: " + to_string(counter) +
						  " pos insertion, val: " +
						  to_string(value));
			return;
		}

		counter++;
		Node *old_prev = prev;
		prev = cur;
		cur = cur->next;

		old_prev->node_mutex->unlock();

		if (cur)
			cur->node_mutex->lock();
		else
		{
			node = new Node(value);
			prev->next = node;

			prev->node_mutex->unlock();

			locked_output("insert: end queue insertion, val: " + to_string(value));
			return;
		}
	}
}

void FineGrainedQueue::remove(int value)
{
	Node *prev = nullptr;
	Node *cur = nullptr;

	while (true)
	{
		{
			unique_lock<shared_mutex> lock(*queue_mutex);

			if (!head)
				continue;

			prev = head;

			prev->node_mutex->lock();

			if (prev->next)
			{
				cur = prev->next;

				if (cur->value == value)
				{
					cur->node_mutex->lock();
					prev->next = cur->next;
					prev->node_mutex->unlock();
					cur->node_mutex->unlock();
					delete cur;

					locked_output("remove: 2 pos queue, val: " + to_string(value) );
					return;
				}
			}

			if (prev->value == value)
			{
				if (cur)
				{
					cur->node_mutex->lock();
					head = cur;
				}
				else
				{
					head = nullptr;
				}

				prev->node_mutex->unlock();

				if (cur)
				{
					cur->node_mutex->unlock();
				}

				delete prev;

				locked_output("remove: 1 pos queue, val: " + to_string(value));

				return;
			}

			if (cur)
				cur->node_mutex->lock();
		}

		while (cur)
		{
			if (cur->value == value)
			{
				prev->next = cur->next;
				prev->node_mutex->unlock();
				cur->node_mutex->unlock();

				delete cur;

				locked_output("remove, val: " + to_string(value));
				return;
			}

			Node *old_prev = prev;
			prev = cur;
			cur = cur->next;
			old_prev->node_mutex->unlock();

			if (cur)
				cur->node_mutex->lock();
		}

		prev->node_mutex->unlock();
		locked_output("remove: cant find val: " + to_string(value));
		// + "; queue size: " + to_string(size())
	}
}

void FineGrainedQueue::locked_output(const string &str)
{
	if (!output)
		return;
	lock_guard<mutex> raii(*lock_cout);
	cout << "[thread: " << this_thread::get_id() << "] " << str << endl;
}

int FineGrainedQueue::size()
{
	Node *cur;
	int counter = 0;
	if (!head)
		return 0;

	shared_lock<shared_mutex>(*queue_mutex);

	cur = head;
	while (cur)
	{
		counter++;
		cur = cur->next;
	}

	return counter;
}
