#ifdef STATS_GEN
#  include "search_data.h"
#  include "allocator.h"
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <time.h>
#  define BUFFER_LEN 65535

static char* serialize_buffer;
static char* search_problem;
static char* search_instance;
static enum algorithm_type_e search_algorithm;
static int search_workers;
static linked_list_t** search_entries;
static struct timespec current_timestamp;
static allocator_t* search_entry_allocator;
static double current_frametime;

// Callbacks necessárias para guardar estatísticas
serialize_function search_serialize_func;

static char* action_labels[] = { "visited", "goal", "successor" };
const char* action_to_str(enum search_action_e action)
{
  return action_labels[(int)action];
}

static char* algorithm_labels[] = { "sequential", "parallel_first", "parallel_exhaustive" };
const char* algorithm_to_str(enum algorithm_type_e algorithm)
{
  return algorithm_labels[(int)algorithm];
}

void search_data_create(char* problem, char* instance, enum algorithm_type_e algo, int workers, serialize_function serialize_fn)
{
  search_entries = (linked_list_t**)calloc(workers, sizeof(linked_list_t*));
  if(search_entries == NULL)
  {
    search_data_destroy();
    return;
  }

  for(int i = 0; i < workers; i++)
  {
    search_entries[i] = linked_list_create();
  }

  search_entry_allocator = allocator_create(sizeof(search_data_entry_t));

  search_problem = problem;
  search_instance = instance;
  search_algorithm = algo;
  search_workers = workers;
  search_serialize_func = serialize_fn;
}

void search_data_destroy()
{
  if(search_entries == NULL)
  {
    return;
  }

  allocator_destroy(search_entry_allocator);

  for(int i = 0; i < search_workers; i++)
  {
    linked_list_destroy(search_entries[i]);
  }
  free(search_entries);
  search_entries = NULL;
}

void search_data_start()
{
  clock_gettime(CLOCK_MONOTONIC, &current_timestamp);
  current_frametime = 0;
}

void search_data_tick()
{
  struct timespec timestamp = current_timestamp;
  clock_gettime(CLOCK_MONOTONIC, &current_timestamp);
  current_frametime += (current_timestamp.tv_sec - timestamp.tv_sec);
  current_frametime += (current_timestamp.tv_nsec - timestamp.tv_nsec) / 1000000000.0;
}

void search_data_add_entry(int worker, state_t* state, enum search_action_e action)
{
  if(worker > search_workers)
  {
    return;
  }

  search_data_entry_t* entry = (search_data_entry_t*)allocator_alloc(search_entry_allocator);
  if(entry == NULL)
  {
    return;
  }

  entry->frametime = current_frametime;
  entry->action = action;
  entry->state = state;

  linked_list_append(search_entries[worker], entry);
}

void search_data_print()
{
  serialize_buffer = (char*)malloc(sizeof(char) * BUFFER_LEN);
  if(!serialize_buffer)
  {
    return;
  }

  printf("{\n");
  printf("\"problem\":\"%s\",", search_problem);
  printf("\"instance\":\"%s\",", search_instance);
  printf("\"type\":\"%s\",", algorithm_to_str(search_algorithm));
  printf("\"workers\":%d,", search_workers);
  printf("\"entries\": [");
  double execution_time = 0;
  for(int w = 0; w < search_workers; w++)
  {
    for(size_t i = 0; i < linked_list_size(search_entries[w]); i++)
    {
      search_data_entry_t* entry = (search_data_entry_t*)linked_list_get(search_entries[w], i);
      memset(serialize_buffer, 0, BUFFER_LEN);

      if(entry->frametime > execution_time)
      {
        execution_time = entry->frametime;
      }
      search_serialize_func(serialize_buffer, entry);
      if(w + i > 0)
      {
        printf(",{\"frametime\":%.9f,\"action\":\"%s\",\"data\":{%s}}",
               entry->frametime,
               action_to_str(entry->action),
               serialize_buffer);
      }
      else
      {
        printf("{\"frametime\":%.9f,\"action\":\"%s\",\"data\":{%s}}",
               entry->frametime,
               action_to_str(entry->action),
               serialize_buffer);
      }
    }
  }
  printf(",{\"frametime\":%.9f,\"action\":\"end\",\"data\":{}}", execution_time);
  printf("],\"execution_time\":%.9f}\n", execution_time);

  free(serialize_buffer);
}
#endif
