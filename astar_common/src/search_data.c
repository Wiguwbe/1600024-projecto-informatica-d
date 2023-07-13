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
static double search_execution_time;
static linked_list_t** search_entries;
static struct timespec search_start_time;
static bool search_active;
static allocator_t* search_entry_allocator;

// Callbacks necessárias para guardar estatísticas
serialize_function search_serialize_func;

static char* action_labels[] = { "visited", "goal", "successor", "end" };
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
  search_active = false;
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
  clock_gettime(CLOCK_MONOTONIC, &search_start_time);
  search_active = true;
}

void search_data_end(double offset)
{
  struct timespec timestamp;
  clock_gettime(CLOCK_MONOTONIC, &timestamp);
  search_data_add_entry(0, NULL, ACTION_END, offset);
  search_execution_time = (timestamp.tv_sec - search_start_time.tv_sec);
  search_execution_time += (timestamp.tv_nsec - search_start_time.tv_nsec) / 1000000000.0;
  search_execution_time -= offset;
  search_active = false;
}

void search_data_add_entry(int worker, state_t* state, enum search_action_e action, double offset)
{
  if(worker > search_workers || !search_active)
  {
    return;
  }

  search_data_entry_t* entry = (search_data_entry_t*)allocator_alloc(search_entry_allocator);
  if(entry == NULL)
  {
    return;
  }

  clock_gettime(CLOCK_MONOTONIC, &entry->timestamp);
  entry->offset = offset;
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
  for(int w = 0; w < search_workers; w++)
  {
    for(size_t i = 0; i < linked_list_size(search_entries[w]); i++)
    {
      search_data_entry_t* entry = (search_data_entry_t*)linked_list_get(search_entries[w], i);
      memset(serialize_buffer, 0, BUFFER_LEN);
      double frametime = (entry->timestamp.tv_sec - search_start_time.tv_sec);
      frametime += (entry->timestamp.tv_nsec - search_start_time.tv_nsec) / 1000000000.0;
      frametime -= entry->offset;

      if(entry->action != ACTION_END)
      {
        search_serialize_func(serialize_buffer, entry);
      }
      if(w + i > 0)
      {
        printf(",{\"frametime\":%.9f,\"action\":\"%s\",\"data\":{%s}}",
               frametime,
               action_to_str(entry->action),
               serialize_buffer);
      }
      else
      {
        printf("{\"frametime\":%.9f,\"action\":\"%s\",\"data\":{%s}}",
               frametime,
               action_to_str(entry->action),
               serialize_buffer);
      }
    }
  }
  printf("],\"execution_time\":%.9f}\n", search_execution_time);

  free(serialize_buffer);
}
#endif
