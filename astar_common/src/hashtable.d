module astar_common.hashtable;

import core.stdc.stdlib;
import core.stdc.string;
import core.sys.posix.pthread;

extern (C):

// #define
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

  // hashtable_create
  static hashtable_t* create(size_t struct_size, hashtable_compare_func cmp_func, hashtable_hash_func hash_func)
  {
    hashtable_t* hashtable = cast(hashtable_t*)malloc(hashtable_t.sizeof);
    if(!hashtable)
      return null;

    hashtable.struct_size = struct_size;
    hashtable.capacity = HASH_CAPACITY;

    hashtable.buckets = cast(entry_t**)calloc(hashtable.capacity, (entry_t*).sizeof);
    if(!hashtable.buckets)
    {
      free(hashtable);
      return null;
    }

    hashtable.cmp_func = cmp_func;
    hashtable.hash_func = hash_func;
    if(!hashtable.hash_func)
      hashtable.hash_func = &hash;

    hashtable.mutexes = cast(pthread_mutex_t*)malloc(HASH_MAX_MUTEXES * pthread_mutex_t.sizeof);
    if(!hashtable.mutexes)
    {
      free(hashtable.buckets);
      free(hashtable);
      return null;
    }
    foreach(size_t i; 0 .. HASH_MAX_MUTEXES)
      pthread_mutex_init(&hashtable.mutexes[i], null);

    return hashtable;
  }

  // hashtable_insert
  void insert(void* data)
  {
    size_t index = hash_func(&this, data);

    entry_t* entry = cast(entry_t*)malloc(entry_t.sizeof);
    if(!entry)
      return;

    entry.data = data;
    int mutex_id = index % HASH_MAX_MUTEXES;

    pthread_mutex_lock(&mutexes[mutex_id]);
    entry.next = buckets[index];
    buckets[index] = entry;

    pthread_mutex_unlock(&mutexes[mutex_id]);
  }

  // hashtable_contains
  void* contains(const void* data)
  {
    size_t index = hash_func(&this, data);

    int mutex_id = index % HASH_MAX_MUTEXES;

    pthread_mutex_lock(&mutexes[mutex_id]);

    entry_t* entry = buckets[index];

    if(!cmp_func)
    {
      while(entry)
      {
        if(memcmp(entry.data, data, struct_size) == 0)
        {
          pthread_mutex_unlock(&mutexes[mutex_id]);
          return entry.data;
        }
        entry = entry.next;
      }
    }
    else
    {
      while(entry)
      {
        if(cmp_func(entry.data, data))
        {
          pthread_mutex_unlock(&mutexes[mutex_id]);
          return entry.data;
        }
        entry = entry.next;
      }
    }

    pthread_mutex_unlock(&mutexes[mutex_id]);

    return null;
  }

  // hashtable_destroy
  void destroy(bool free_data)
  {
    foreach(size_t i; 0 .. capacity)
    {
      int mutex_id = i % HASH_MAX_MUTEXES;
      pthread_mutex_lock(&mutexes[mutex_id]);

      entry_t* entry = buckets[i];
      while(entry)
      {
        entry_t *next = entry.next;
        if(free_data)
          free(entry.data);
        free(entry);
        entry = next;
      }

      pthread_mutex_unlock(&mutexes[mutex_id]);

      pthread_mutex_destroy(&mutexes[mutex_id]);
    }

    free(mutexes);
    free(buckets);
    free(&this);
  }

  // hashtable_reserve
  entry_t* reserve(void* data)
  {
    size_t index = hash_func(&this, data);

    int mutex_id = index % HASH_MAX_MUTEXES;

    pthread_mutex_lock(&mutexes[mutex_id]);

    entry_t* entry = buckets[index];

    if(!cmp_func)
    {
      while(entry)
      {
        if(memcmp(entry.data, data, struct_size) == 0)
        {
          pthread_mutex_unlock(&mutexes[mutex_id]);
          return entry;
        }
        entry = entry.next;
      }
    }
    else
    {
      while(entry)
      {
        if(cmp_func(entry.data, data))
        {
          pthread_mutex_unlock(&mutexes[mutex_id]);
          return entry;
        }
        entry = entry.next;
      }
    }

    entry = cast(entry_t*)malloc(entry_t.sizeof);
    if(!entry)
    {
      pthread_mutex_unlock(&mutexes[mutex_id]);
      return null;
    }

    entry.data = data;

    entry.next = buckets[index];
    buckets[index] = entry;

    pthread_mutex_unlock(&mutexes[mutex_id]);

    return entry;
  }
}

size_t hash_function(const void* data, size_t size, size_t mod)
{
  const ubyte* bytes = cast(const ubyte*)data;

  size_t hash_value = 0;
  size_t mul = 1;
  foreach(size_t i; 0 .. size)
  {
    mul = (i % 4 == 0) ? 1 : mul * 256;
    hash_value += bytes[i] * mul;
  }

  return hash_value % mod;
}

static size_t hash(hashtable_t* hashtable, const void* data)
{
  return hash_function(data, hashtable.struct_size, hashtable.capacity);
}

/*
  C compatibility layer
*/

hashtable_t* hashtable_create(size_t struct_size, hashtable_compare_func cmp_func, hashtable_hash_func hash_func)
{
  return hashtable_t.create(struct_size, cmp_func, hash_func);
}

void hashtable_insert(hashtable_t* hashtable, void* data)
{
  hashtable.insert(data);
}

void* hashtable_contains(hashtable_t* hashtable, const void* data)
{
  return hashtable.contains(data);
}

void hashtable_destroy(hashtable_t* hashtable, bool free_data)
{
  hashtable.destroy(free_data);
}

entry_t* hashtable_reserve(hashtable_t *hashtable, void* data)
{
  return hashtable.reserve(data);
}

