// D import file generated from 'astar_common/src/channel.d'
module astar_common.channel;
import core.stdc.stdlib;
import core.stdc.string;
import core.sys.posix.pthread;
extern (C)
{
	enum int QUEUE_BUFFER_SIZE = 2048;
	struct channel_t
	{
		void** queues;
		size_t* queue_pos;
		size_t* queue_size;
		pthread_mutex_t* queue_lock;
		size_t struct_size;
		size_t num_queues;
		static channel_t* create(size_t num_queues, size_t struct_size);
		void send(size_t queue_index, void* data);
		void* receive(size_t queue_index, size_t* len);
		void destroy();
		bool has_messages(size_t queue_index);
	}
	channel_t* channel_create(size_t num_queues, size_t struct_size);
	void channel_send(channel_t* channel, size_t queue_index, void* data);
	void* channel_receive(channel_t* channel, size_t queue_index, size_t* len);
	void channel_destroy(channel_t* channel);
	bool channel_has_messages(channel_t* channel, size_t queue_index);
}
