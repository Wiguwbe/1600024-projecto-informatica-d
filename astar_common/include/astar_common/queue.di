// D import file generated from 'src/queue.d'
module astar_common.queue;
import core.stdc.stdlib;
import core.sys.posix.pthread;
extern (C)
{
	template queue(T)
	{
		struct queue_node_t
		{
			T data;
			queue_node_t* next;
		}
		struct queue_t
		{
			queue_node_t* head;
			queue_node_t* tail;
			size_t size;
			pthread_mutex_t lock;
			static queue_t* create()
			{
				queue_t* queue = cast(queue_t*)malloc(queue_t.sizeof);
				if (!queue)
					return null;
				queue.head = (queue.tail = null);
				queue.size = 0;
				pthread_mutex_init(&queue.lock, null);
				return queue;
			}
			void push(T data)
			{
				queue_node_t* new_node = cast(queue_node_t*)malloc(queue_node_t.sizeof);
				if (!new_node)
					return ;
				new_node.data = data;
				new_node.next = null;
				pthread_mutex_lock(&lock);
				if (!tail)
					head = (tail = new_node);
				else
				{
					tail.next = new_node;
					tail = new_node;
				}
				size++;
				pthread_mutex_unlock(&lock);
			}
			T pop()
			{
				if (is_empty())
					return null;
				pthread_mutex_lock(&lock);
				queue_node_t* node = head;
				void* data = node.data;
				head = head.next;
				if (!head)
					tail = null;
				size--;
				pthread_mutex_unlock(&lock);
				free(node);
				return data;
			}
			bool is_empty()
			{
				return head == null;
			}
			void destroy()
			{
				destroy(false);
			}
			void destroy(bool free_data)
			{
				pthread_mutex_lock(&lock);
				queue_node_t* next;
				while (head)
				{
					next = head.next;
					if (free_data)
						free(head.data);
					free(head);
					head = next;
				}
				pthread_mutex_unlock(&lock);
				pthread_mutex_destroy(&lock);
				free(&this);
			}
		}
	}
	alias q = queue!(void*);
	q.queue_t* queue_create();
	void queue_push(q.queue_t* queue, void* data);
	void* queue_pop(q.queue_t* queue);
	bool queue_is_empty(q.queue_t* queue);
	size_t queue_size(q.queue_t* queue);
	void queue_destroy(q.queue_t* queue, bool free_data);
}
