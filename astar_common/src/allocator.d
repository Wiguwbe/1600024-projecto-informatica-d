module astar_common.allocator;

import core.stdc.stdlib;
import core.sys.posix.pthread;
//import core.sys.posix.sys.types

extern (C):

/*
  New D code
*/

struct allocator_t
{
  size_t struct_size;
  size_t page_size;
  void** pages;
  size_t num_pages;
  size_t current_page;
  size_t offset;
  pthread_mutex_t mutex;

  // allocator_create
  static allocator_t* create(size_t struct_size)
  {
    allocator_t* allocator = cast(allocator_t*)malloc(allocator_t.sizeof);
    if(!allocator)
      return null;

    allocator.struct_size = struct_size;
    allocator.page_size = 1*1024*1024*1024;
    allocator.pages = null;
    allocator.num_pages = 0;
    allocator.current_page = 0;
    allocator.offset = 0;
    pthread_mutex_init(&allocator.mutex, null);

    return allocator;
  }

  // allocator_destroy
  void destroy()
  {
    foreach(size_t i; 0 .. this.num_pages)
      free(this.pages[i]);
    free(this.pages);

    this.pages = null;
    this.num_pages = 0;
    this.current_page = 0;
    this.offset = 0;
    pthread_mutex_destroy(&this.mutex);
    free(&this);
  }

  // allocator_alloc
  void* alloc()
  {
    pthread_mutex_lock(&this.mutex);

    void _alloc_page()
    {
      this.num_pages++;
      this.pages = cast(void**)malloc(this.num_pages * (void*).sizeof);
      this.pages[this.current_page] = malloc(this.page_size);
    }

    if(!this.num_pages)
      _alloc_page();

    if(this.offset + this.struct_size > this.page_size)
    {
      this.current_page++;
      this.offset = 0;

      if(this.current_page >= this.num_pages)
        _alloc_page();
    }

    void* ptr = this.pages[this.current_page] + this.offset;
    this.offset += this.struct_size;

    pthread_mutex_unlock(&this.mutex);

    return ptr;
  }
}

/*
  C compatibility layer
*/

allocator_t* allocator_create(size_t struct_size)
{
  return allocator_t.create(struct_size);
}

void allocator_destroy(allocator_t* allocator)
{
  allocator.destroy();
}

void* allocator_alloc(allocator_t* allocator)
{
  return allocator.alloc();
}
