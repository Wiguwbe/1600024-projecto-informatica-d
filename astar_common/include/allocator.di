// D import file generated from 'src/allocator.d'
module astar_common.allocator;
import core.stdc.stdlib;
import core.sys.posix.pthread;
extern (C)
{
	struct allocator_t
	{
		size_t struct_size;
		size_t page_size;
		void** pages;
		size_t num_pages;
		size_t current_page;
		size_t offset;
		pthread_mutex_t mutex;
		static allocator_t* create(size_t struct_size);
		void destroy();
		void* alloc();
	}
	allocator_t* allocator_create(size_t struct_size);
	void allocator_destroy(allocator_t* allocator);
	void* allocator_alloc(allocator_t* allocator);
}
