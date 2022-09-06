#include "lock-free_priority_queue_test.h"
#include "timer.c"
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 2

// Global variable
struct Node *head, *tail;

// Can be any value to link the values for each node
int MAXLEVEL = 1;

struct Node *MALLOC_NODE() {
  return malloc(sizeof(struct Node));
}

struct Node *READ_NODE(union Link *address) {
  if (address->d)
    return NULL;

  return address->p;
}

struct Node *COPY_NODE(struct Node *node) {
  return node;
}

void RELEASE_NODE(struct Node *node) { return; }

struct Node *CreateNode(int level, int key, void *value) {

  /***************************************************************************/
  /*                            Create a new node                                   */
  /***************************************************************************/
  printf("Create node\n");

  union VLink vlink_value;

  // Create link to current node
  vlink_value.p = value;
  vlink_value.d = false;

  struct Node *node = MALLOC_NODE(); // need this for safe pointer handling
  node->prev = NULL;
  node->validLevel = 0;
  node->level = level;
  node->key = key;
  node->next = malloc(sizeof(union Link) * level);
  node->value = vlink_value;
  return node;
}

static int rand_level() {
  /****************************************************************************************/
  /* Generate a random level for integer based on the maxlevel allowed in data structure  */
  /****************************************************************************************/
  int level = 1;
  while (rand() < RAND_MAX / 2 && level < MAXLEVEL)
    level++;
  return level;
}


struct Node *ReadNext(struct Node **node1, int level) {
  printf("read next\n");
  // Local variables (for all functions/procedures)
  struct Node *node2;

  if ((*node1)->value.d == true)
    *node1 = HelpDelete(*node1, level);

  node2 = READ_NODE(&(*node1)->next[level]);

  while (node2 == NULL) {
    *node1 = HelpDelete(*node1, level);
    node2 = READ_NODE(&(*node1)->next[level]);
  }
  return node2;
}


struct Node *ScanKey(struct Node **node1, int level, int key) {
  printf("scankey\n");
  // Local variables (for all functions/procedures)
  struct Node *node2;

  node2 = ReadNext(node1, level);
  while (node2->key < key) {
    RELEASE_NODE(*node1);
    *node1 = node2;
    node2 = ReadNext(node1, level);
  }
  return node2;
}

bool Insert(int key, void *value) {
  printf("insert\n");
  // Local variables (for all functions/procedures)
  struct Node *newNode, *savedNodes[MAXLEVEL];
  struct Node *node1, *node2, *prev, *last;
  int i;

  int level = rand_level();

  newNode = CreateNode(level, key, value);
  COPY_NODE(newNode);
  node1 = COPY_NODE(head);

  for (i = MAXLEVEL - 1; i >= 1; i--) {
    node2 = ScanKey(&node1, 0, key);
    RELEASE_NODE(node2);
    if (i < level)
      savedNodes[i] = COPY_NODE(node1);
  }

  while (true) {
    node2 = ScanKey(&node1, 0, key);

    if (node2->value.d == false && node2->key == key) {
      if (__sync_bool_compare_and_swap((long long *)&(node2->value),
                                       *(long long *)&node2->value,
                                       *(long long *)value)) {
        RELEASE_NODE(node1);
        RELEASE_NODE(node2);
        for (i = 1; i <= level - 1; i++)
          RELEASE_NODE(savedNodes[i]);
        RELEASE_NODE(newNode);
        RELEASE_NODE(newNode);
        return true;
      } else {
        RELEASE_NODE(node2);
        continue;
      }
    }

    newNode->next[0] = node1->next[0];

    RELEASE_NODE(node2);

    union Link newNode_link;
    newNode_link.p = newNode;
    newNode_link.d = false;

    if (__sync_bool_compare_and_swap((long long *)&node1->next[0],
                                     *(long long *)&node1->next[0],
                                     *(long long *)&newNode_link)) {
      RELEASE_NODE(node1);
      break;
    }
  }

  for (i = 1; i <= level - 1; i++) {
    newNode->validLevel = i;
    node1 = savedNodes[i];

    while (true) {
      node2 = ScanKey(&node1, i, key);
      newNode->next[i] = node1->next[i];

      RELEASE_NODE(node2);

      if (newNode->value.d == true ||
          __sync_bool_compare_and_swap((long long *)&node1->next[i],
                                       *(long long *)&node1->next[i],
                                       *(long long *)&newNode)) {
        RELEASE_NODE(node1);
        break;
      }
    }
  }

  newNode->validLevel = level;

  if (newNode->value.d == true)
    newNode = HelpDelete(newNode, 0);

  RELEASE_NODE(newNode);

  return true;
}

void RemoveNode(struct Node *node, struct Node **prev, int level) {
  printf("remove node\n");
  // Local variables (for all functions/procedures)
  struct Node *last;

  union Link empty_link;
  empty_link.p = NULL;
  empty_link.d = true;

  while (true) {
    if (node->next[level].p == empty_link.p && node->next[level].d == true)
      break;

    last = ScanKey(prev, level, node->key);
    RELEASE_NODE(last);

    if (last != node ||
        (node->next[level].p == empty_link.p && node->next[level].d == true))
      break;

    union Link new_link;
    new_link.p = node->next[level].p;
    new_link.d = false;
    if (__sync_bool_compare_and_swap((long long *)&(*prev)->next[level],
                                     *(long long *)&(*prev)->next[level],
                                     *(long long *)&new_link)) {
      node->next[level] = empty_link;
      break;
    }

    if (node->next[level].p == empty_link.p &&
        node->next[level].d == empty_link.d)
      break;

    // Back-Off
  }
}

void *DeleteMin() {
  printf("delete min\n");
  // Local variables (for all functions/procedures)
  struct Node *newNode, *savedNodes[MAXLEVEL];
  struct Node *node1, *node2, *prev, *last;
  int i;

  prev = COPY_NODE(head);

  while (true) {
    // Checking if the head is the same as the tail
    node1 = ReadNext(&prev, 0);
    if (node1 == tail) {
      RELEASE_NODE(prev);
      RELEASE_NODE(node1);
      return NULL;
    }
  retry:
    if (node1 != prev->next[0].p) {
      RELEASE_NODE(node1);
      continue;
    }

    if (node1->value.d == false) {
      union VLink new_value;
      new_value.p = node1->value.p;
      new_value.d = true;
      if (__sync_bool_compare_and_swap((long long *)&node1->value,
                                       *(long long *)&node1->value,
                                       *(long long *)&new_value)) {
        node1->prev = prev;
        break;
      } else
        goto retry;
    } else if (node1->value.d == true)
      node1 = HelpDelete(node1, 0);
    RELEASE_NODE(prev);
    prev = node1;
  }

  for (i = 0; i <= node1->level - 1; i++) {
    union Link new_link;
    new_link.d = true;
    do {
      new_link = node1->next[i];
      // Until d = true or CAS
    } while ((node1->next[i].d != true) &&
             !(__sync_bool_compare_and_swap((long long *)&node1->next[i],
                                            *(long long *)&node1->next[i],
                                            *(long long *)&new_link)));
  }

  prev = COPY_NODE(head);
  for (i = node1->level - 1; i >= 0; i--)
    RemoveNode(node1, &prev, i);

  union VLink value = node1->value;

  RELEASE_NODE(prev);
  RELEASE_NODE(node1);
  RELEASE_NODE(node1); /*Delete the node*/

  return value.p;
}

struct Node *HelpDelete(struct Node *node, int level) {
  printf("help delete\n");
  // Local variables (for all functions/procedures)
  struct Node *node2, *prev;
  int i;

  union Link new_link;
  new_link.d = true;

  for (i = level; i <= node->level - 1; i++) {
    do {
      new_link.p = node->next[i].p;
    } while (node->next[i].d != true &&
             !(__sync_bool_compare_and_swap((long long *)&node->next[i],
                                            *(long long *)&node->next[i],
                                            *(long long *)&new_link)));
  }

  prev = node->prev;

  if (!prev || level >= prev->validLevel) {
    prev = COPY_NODE(head);

    for (i = MAXLEVEL - 1; i >= level; i--) {
      node2 = ScanKey(&prev, i, node->key);
      RELEASE_NODE(node2);
    }
  } else
    COPY_NODE(prev);

  RemoveNode(node, &prev, level);
  RELEASE_NODE(node);

  return prev;
}

void *simultaneousChange(void *threadid) {
  int *id_ptr, taskid;
  sleep(1);

  id_ptr = (int *)threadid;

  taskid = *id_ptr;
  int val = 6;

  printf("Thread: Taskid is %i\n", taskid);
  if (taskid == 1) {
    DeleteMin();
  } else {
    Insert(6, &val);
  }
  return (NULL);
}


int main(void) {
  pthread_t threads[NUM_THREADS];

  int i, val0 = 0, val1 = 7, val2 = 2, val3 = 12;

  head = CreateNode(1, -1000000, &val0);
  tail = CreateNode(1, 1000000, &val0);
  head->next[0].p = tail;
  head->next[0].d = false;

  Insert(7, &val1);
  Insert(2, &val2);
  Insert(12, &val3);

  // Remove 2 and insert 4 at the same time
  /* struct timespec start, end, startComp, endComp; */
  /* clock_gettime(CLOCK_MONOTONIC, &startComp); */

  int t, rc;

  for (t = 0; t <= 1; t++) {
    printf("Main: Creating thread %d\n", t);
    rc = pthread_create(&threads[t], NULL, simultaneousChange, (void *)&t);
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      return (EXIT_FAILURE);
    }
  }
    /* clock_gettime(CLOCK_MONOTONIC, &endComp); */
    /* pthread_join(&threads[t], rc); */


    sleep(5);
    PrintList(head);

    /* double ans = get_time_taken(endComp, startComp); */
    /* printf("Executing took %lf seconds.\n", ans); */

    pthread_exit(NULL);
    exit(0);
    /* return 0; */
  }
