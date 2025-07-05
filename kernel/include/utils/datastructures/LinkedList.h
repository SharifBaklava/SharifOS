

template <typename T>
class LinkedListNode 
{
	LinkedListNode *next;
    T data;
};

template <typename T>
class LinkedList {
	private:
	LinkedListNode<T> *head;
	LinkedListNode<T> *tail;
	size_t size;
	public:
	virtual LinkedListNode<T> * createNode()
	{

	}
	
	push_back(T data)
	{
		LinkedListNode<T> *newNode =  createNode();
		newNode->data = data;
		newNode->next = nullptr;

		if(head == nullptr) {
			head = newNode;
			tail = newNode;
		}
		else {
			tail->next = newNode;
			tail = newNode;
		}

	}
	LinkedListNode<T>* pop_back()
	{
		if (head == nullptr) {
			return nullptr; // List is empty
		}

		LinkedListNode<T>* current = head;
		LinkedListNode<T>* previous = nullptr;
		while (current->next != nullptr) {
			previous = current;
			current = current->next;
		}

		if (previous == nullptr) {
			head = nullptr;
			tail = nullptr;
		}
		else {
			previous->next = nullptr;
			tail = previous;
		}
		return current; // Return the popped node
	}
	remove()
	{

	}
};


