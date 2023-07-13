#ifdef STATS_GEN
#  include "search_data.h"
#  include <stdio.h>
#  include <stdlib.h>
#  define BUFFER_LEN 65535

static char* serialize_buffer;
static search_data_t* singleton = NULL;

const char* action_to_str(enum search_action_e action)
{
  switch(action)
  {
    case ACTION_VISITED: return "visited";
    case ACTION_GOAL: return "goal";
    case ACTION_SUCESSOR: return "sucessor";
    case ACTION_END: return "end";
    default: return "end";
  }
}

void search_data_create(char* problem, char* instance, enum algorithm_type_e algo, int workers, serialize_function serialize_fn)
{
  singleton = (search_data_t*)malloc(sizeof(search_data_t));

  if(!singleton)
  {
    return;
  }

  singleton->entries = (linked_list_t**)malloc(sizeof(linked_list_t*) * workers);

  if(!singleton->entries)
  {
    free(singleton);
    singleton = NULL;
    return;
  }

  for(int i = 0; i < workers; i++)
  {
    singleton->entries[i] = linked_list_create();
  }

  singleton->problem = problem;
  singleton->instance = instance;
  singleton->algorithm = algo;
  singleton->workers = workers;
  singleton->serialize_func = serialize_fn;
}

search_data_t* search_data_attach()
{
  return singleton;
}

void search_data_destroy()
{
  if(!singleton)
  {
    return;
  }

  for(int i = 0; i < singleton->workers; i++)
  {
    while(linked_list_size(singleton->entries[i]))
    {
      search_data_entry_t* entry = linked_list_pop_back(singleton->entries[i]);
      free(entry);
    }

    linked_list_destroy(singleton->entries[i]);
  }

  free(singleton->entries);
  free(singleton);
}

void search_data_start()
{
  if(!singleton)
  {
    return;
  }

  clock_gettime(CLOCK_MONOTONIC, &singleton->start_time);
}

void search_data_end(double offset)
{
  if(!singleton)
  {
    return;
  }

  struct timespec timestamp;
  clock_gettime(CLOCK_MONOTONIC, &timestamp);

  search_data_add_entry(0, NULL, ACTION_END, offset);

  singleton->execution_time = (timestamp.tv_sec - singleton->start_time.tv_sec);
  singleton->execution_time += (timestamp.tv_nsec - singleton->start_time.tv_nsec) / 1000000000.0;
  singleton->execution_time -= offset;
}

void search_data_add_entry(int worker, state_t* state, enum search_action_e action, double offset)
{
  if(!singleton)
  {
    return;
  }

  if(worker > singleton->workers)
  {
    return;
  }

  search_data_entry_t* entry = (search_data_entry_t*)malloc(sizeof(search_data_entry_t));
  if(!entry)
  {
    return;
  }

  struct timespec timestamp;
  clock_gettime(CLOCK_MONOTONIC, &timestamp);

  entry->frametime = (timestamp.tv_sec - singleton->start_time.tv_sec);
  entry->frametime += (timestamp.tv_nsec - singleton->start_time.tv_nsec) / 1000000000.0;
  entry->frametime -= offset;
  entry->action = action;
  entry->state = state;

  linked_list_append(singleton->entries[worker], entry);
}

void search_data_print()
{
  if(!singleton)
  {
    return;
  }

  serialize_buffer = (char*)malloc(sizeof(char) * BUFFER_LEN);
  if(!serialize_buffer)
  {
    return;
  }

  printf("{\n");
  printf("\"problem\":\"%s\",", singleton->problem);
  printf("\"instance\":\"%s\",", singleton->instance);
  switch(singleton->algorithm)
  {
    case ALGO_SEQUENTIAL: printf("\"type\":\"sequential\","); break;
    case ALGO_PARALLEL_FIRST: printf("\"type\":\"parallel_first\","); break;
    case ALGO_PARALLEL_EXHAUSTIVE: printf("\"type\":\"parallel_exhaustive\","); break;

    default: break;
  }
  printf("\"workers\":%d,", singleton->workers);
  printf("\"entries\": [");
  for(int w = 0; w < singleton->workers; w++)
  {
    for(size_t i = 0; i < linked_list_size(singleton->entries[w]); i++)
    {
      search_data_entry_t* entry = (search_data_entry_t*)linked_list_get(singleton->entries[w], i);
      memset(serialize_buffer, 0, BUFFER_LEN);
      if(entry->action != ACTION_END)
      {
        singleton->serialize_func(serialize_buffer, entry);
      }
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
  printf("],\"execution_time\":%.9f}\n", singleton->execution_time);

  free(serialize_buffer);
}

void search_data_save(const char* output_file)
{
  if(!singleton)
  {
    return;
  }

  serialize_buffer = (char*)malloc(sizeof(char) * BUFFER_LEN);
  if(!serialize_buffer)
  {
    return;
  }

  FILE* fp = fopen(output_file, "w");
  if(fp == NULL)
  {
    return;
  }

  fprintf(fp, "{\n");
  fprintf(fp, "\"problem\":\"%s\",", singleton->problem);
  fprintf(fp, "\"instance\":\"%s\",", singleton->instance);
  switch(singleton->algorithm)
  {
    case ALGO_SEQUENTIAL: fprintf(fp, "\"type\":\"sequential\","); break;
    case ALGO_PARALLEL_FIRST: fprintf(fp, "\"type\":\"parallel_first\","); break;
    case ALGO_PARALLEL_EXHAUSTIVE: fprintf(fp, "\"type\":\"parallel_exhaustive\","); break;

    default: break;
  }
  fprintf(fp, "\"workers\":%d,", singleton->workers);
  fprintf(fp, "\"entries\": [,");
  for(int w = 0; w < singleton->workers; w++)
  {
    for(size_t i = 0; i < linked_list_size(singleton->entries[w]); i++)
    {
      search_data_entry_t* entry = (search_data_entry_t*)linked_list_get(singleton->entries[w], i);
      memset(serialize_buffer, 0, BUFFER_LEN);
      if(entry->action != ACTION_END)
      {
        singleton->serialize_func(serialize_buffer, entry);
      }
      if(w + i > 0)
      {
        fprintf(fp,
                ",{\"frametime\":%.9f,\"action\":\"%s\",\"data\":{%s}]",
                entry->frametime,
                action_to_str(entry->action),
                serialize_buffer);
      }
      else
      {
        fprintf(fp,
                "{\"frametime\":%.9f,\"action\":\"%s\",\"data\":{%s}]",
                entry->frametime,
                action_to_str(entry->action),
                serialize_buffer);
      }
    }
  }
  fprintf(fp, "],\"execution_time\":%.9f}\n", singleton->execution_time);

  fclose(fp);
  free(serialize_buffer);
}

#endif
