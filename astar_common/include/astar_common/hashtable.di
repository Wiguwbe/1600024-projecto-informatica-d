// D import file generated from 'src/hashtable.d'
module astar_common.hashtable;
import core.stdc.stdlib;
import core.stdc.string;
import core.sys.posix.pthread;
extern (C)
{
	enum int HASH_MAX_MUTEXES = 8192;
	enum int HASH_CAPACITY = 65533;
	alias hashtable_compare_func = bool function(const void*, const void*);
	alias hashtable_hash_func = size_t function(hashtable_t*, const void*);
	struct entry_t
	{
		void* data;
		entry_t* next;
	}
	struct hashtable_t
	{
		size_t struct_size;
		size_t capacity;
		entry_t** buckets;
		hashtable_compare_func cmp_func;
		hashtable_hash_func hash_func;
		pthread_mutex_t* mutexes;
		static hashtable_t* create(size_t struct_size, hashtable_compare_func cmp_func, hashtable_hash_func hash_func);
		void insert(void* data);
		void* contains(const void* data);
		void destroy(bool free_data);
		entry_t* reserve(void* data);
	}
	size_t hash_function(const void* data, size_t size, size_t mod);
	static size_t hash(hashtable_t* hashtable, const void* data);
	hashtable_t* hashtable_create(size_t struct_size, hashtable_compare_func cmp_func, hashtable_hash_func hash_func);
	void hashtable_insert(hashtable_t* hashtable, void* data);
	void* hashtable_contains(hashtable_t* hashtable, const void* data);
	void hashtable_destroy(hashtable_t* hashtable, bool free_data);
	entry_t* hashtable_reserve(hashtable_t* hashtable, void* data);
}
