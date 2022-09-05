#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Global variable
struct Node *head, *tail;

union Link
{
  long long dword;

  struct
  {
    struct Node* p; // pointer to Node
    bool d; // Boolean d for deletion status
  };
};


union Vlink
{
  long long dword;

  struct
  {
    struct Node* p; // pointer to Value
    bool d; //boolean d for deletion
  };
};


struct Node {
  int key, level, validLevel;
  union Vlink value;
  union Link* next; // Array of next with level, optionally can be Link*
	struct Node* prev; // Pointer to previous node
  int reference_counter;
}

struct Node * MALLOC_NODE() {
  return malloc(sizeof(struct Node));
}

struct Node * RELEASE_NODE(struct Node* node) {
  node->reference_counter--;

  if (node->reference_counter == 0) {
    // the function will recursively call RELEASE_NODE on the nodes that this node has owned pointers to
    RELEASE_NODE( node->prev );
    return node;
  }

  return node;
}


struct Node* READ_NODE(union Link* address) {
  // De-reference pointer if the node pointed is not there
  if (address->d) {
    // if the node is marked by deletion, pretend the node is not there
    return NULL;
  };

  // return the node pointed
  struct Node* node = address->p;
  node->reference_counter++;
  return address->p;
}


struct Node* COPY_NODE(struct Node* nodeptr) {
  // Copies node and increase the reference counter by 1
  nodeptr->reference_counter++;
  return nodeptr;
}

//RELEASE_NODE(node:pointer to Node)

struct Node* CreateNode(int level, int key, void* value)
{
  union Vlink new_vlink;
  new_vlink.p = value;
  new_vlink.d= false;

  struct Node* node = MALLOC_NODE();  // Create new node
  node->prev = NULL;
  node->validLevel = 0;
  node->level = level;
  node->key = key;
  //TODO add next here?
  node->next = malloc(sizeof(union Link) * level);
  node->value = new_vlink;
  return node;
}


struct Node* ReadNext(struct Node** node1, int level) {
  if ( (*node1) -> value.d==true ) {
    *node1 = HelpDelete(*node1,level);
    node2 = READ_NODE((*node1)->next[level]);
  }

  while ( node2==NULL ) {
    *node1 = HelpDelete(*node1,level);
    node2 = READ_NODE((*node1).next[level]);
  }

  return node2;

}


struct Node* ScanKey(struct Node** node1, int level, int key)
{
  struct Node *node2;
  node2 = ReadNext(node1, level);

  while(node2->key < key)
    {
      RELEASE_NODE(*node1);
      *node1 = node2;
      node2 = ReadNext(node1, level);
    }
  return node2;
}


bool Insert(int key, void* value)
{
  // TODO generate random value
  int level = 1;

  // Local variables (for all functions/procedures)
  struct Node *newNode = CreateNode(level, key, value);

  COPY_NODE(newNode); // Increase reference counter of new node by 1

  struct Node* node1 = COPY_NODE(head);

  /* savedNodes[maxLevel]; */


  /***************************************************************************/
  /*                             Create new node                             */
  /***************************************************************************/

  // Should be log(all nodes)
  int maxLevel = 10;
  struct Node *savedNodes[maxLevel];


  for(int i = maxLevel-1; i >= 1; i--)
    {
      struct Node* node2 = ScanKey(&node1, i, key);
      RELEASE_NODE(node2);
      if(i < level) {
        savedNodes[i] = COPY_NODE(node1);
      }
    }

  while(true) {
      struct Node* node2 = ScanKey(&node1, 0, key);

      union Vlink value2 = node2->value;

      if(node2->value.d == false && node2->key == key)
        {
          if(__sync_bool_compare_and_swap((long long*)&value2, *(long long*)&value2, *(long long*)value))
            {
              RELEASE_NODE(node1);
              RELEASE_NODE(node2);
              for(i = 1; i <= level-1; i++)
                RELEASE_NODE(savedNodes[i]);
              RELEASE_NODE(newNode);
              RELEASE_NODE(newNode);
              return true;
            }
          else
            {
              RELEASE_NODE(node2);
              continue;
            }
        }

      newNode->next[0] = node1->next[0];

      RELEASE_NODE(node2);

      /* union Link newNode_link; */
      /* newNode_link.p = newNode; */
      /* newNode_link.d = false; */

      if(__sync_bool_compare_and_swap((long long*)&node1->next[0], *(long long*)&node1->next[0], *(long long*)&newNode_link))
        {
          RELEASE_NODE(node1);
          break;
        }
      // Back-Off
    }

  for(i = 1; i <= level-1; i++)
    {
      newNode->validLevel = i;
      node1 = savedNodes[i];

      while(true)
        {
          node2 = ScanKey(&node1, i, key);

          // Why node1->next[1] is node2 link?
          newNode->next[i] = node1->next[i];

          RELEASE_NODE(node2);

          if(newNode->value.d == true || __sync_bool_compare_and_swap((long long*)&node1->next[i], *(long long*)&node1->next[i], *(long long*)&newNode))
            {
              RELEASE_NODE(node1);
              break;
            }
          // Back-Off
        }
    }

  newNode->validLevel = level;

  //newNode->timeInsert =getNextTimeStamp();

  if(newNode->value.d == true)
    newNode = HelpDelete(newNode, 0);

  RELEASE_NODE(newNode);

  return true;
}
