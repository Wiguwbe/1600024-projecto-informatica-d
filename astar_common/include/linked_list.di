// D import file generated from 'src/linked_list.d'
module astar_common.linked_list;
import core.stdc.stdlib;
extern (C)
{
	template linked_list(T)
	{
		struct linked_list_node_t
		{
			linked_list_node_t* next;
			T data;
		}
		struct linked_list_t
		{
			linked_list_node_t* head;
			linked_list_node_t* tail;
			size_t size;
			void clear()
			{
				while (this.head)
				{
					linked_list_node_t* next = this.head.next;
					free(this.head);
					this.head = next;
					this.size--;
				}
			}
			void append(T data)
			{
				linked_list_node_t* new_node = cast(linked_list_node_t*)malloc(linked_list_node_t.sizeof);
				new_node.data = data;
				new_node.next = null;
				if (!this.head)
					this.head = (this.tail = new_node);
				else
				{
					this.tail.next = new_node;
					this.tail = new_node;
				}
				this.size++;
			}
			void insert(T data, size_t index)
			{
				if (index > this.size)
					return ;
				linked_list_node_t* new_node = cast(linked_list_node_t*)malloc(linked_list_node_t.sizeof);
				new_node.data = data;
				if (!this.head)
				{
					new_node.next = null;
					this.head = (this.tail = new_node);
				}
				else if (index == 0)
				{
					new_node.next = this.head;
					this.head = new_node;
				}
				else
				{
					linked_list_node_t* prev_node = this.head;
					foreach (size_t i; 0 .. index - 1)
					{
						prev_node = prev_node.next;
					}
					new_node.next = prev_node.next;
					prev_node.next = new_node;
				}
				this.size++;
			}
			void remove(size_t index)
			{
				if (index >= this.size)
					return ;
				if (this.size == 1)
				{
					free(this.head);
					this.head = (this.tail = null);
				}
				else if (index == 0)
				{
					linked_list_node_t* node_to_remove = this.head;
					this.head = this.head.next;
					free(node_to_remove);
				}
				else
				{
					linked_list_node_t* prev_node = this.head;
					foreach (size_t i; 0 .. index - 1)
					{
						prev_node = prev_node.next;
					}
					linked_list_node_t* node_to_remove = prev_node.next;
					prev_node.next = node_to_remove.next;
					if (index == this.size - 1)
						this.tail = prev_node;
					free(node_to_remove);
				}
				this.size--;
			}
			T pop_back()
			{
				if (!this.size)
					return cast(T)null;
				linked_list_node_t* node = this.head;
				this.head = node.next;
				this.size--;
				T data = node.data;
				free(node);
				return data;
			}
			T get(size_t index)
			{
				if (index >= this.size)
					return cast(T)null;
				linked_list_node_t* node = this.head;
				foreach (size_t i; 0 .. index)
				{
					node = node.next;
				}
				return node.data;
			}
		}
		linked_list_t* create()
		{
			linked_list_t* list = cast(linked_list_t*)malloc(linked_list_t.sizeof);
			list.head = (list.tail = null);
			list.size = 0;
			return list;
		}
	}
	alias ll = linked_list!(void*);
	ll.linked_list_t* linked_list_create();
	void linked_list_destroy(ll.linked_list_t* list);
	void linked_list_append(ll.linked_list_t* list, void* data);
	void linked_list_insert(ll.linked_list_t* list, void* data, size_t index);
	void linked_list_remove(ll.linked_list_t* list, size_t index);
	void* linked_list_pop_back(ll.linked_list_t* list);
	size_t linked_list_size(ll.linked_list_t* list);
	void* linked_list_get(ll.linked_list_t* list, size_t index);
}
