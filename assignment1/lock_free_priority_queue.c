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
  Vlink value;
  Link[level] next; // Array of next with level, optionally can be Link*
	struct Node* prev; // Pointer to previous node
  int reference_counter;
}


struct Node* MALLOC_NODE()
{
  return malloc(sizeof(struct Node));
}


struct Node* READ_NODE(union Link* address) {
  // De-reference pointer if the node pointed is not there
  if (address->d) {
    // if the node is marked by deletion, pretend the node is not there
    return NULL;
  };

  // return the node pointed
  struct Node node = address->p;
  node.reference_counter++;
  return address->p;
}


struct Node* COPY_NODE(node:pointer to Node) {
  // Copies node and increase the reference counter by 1
  node.reference_counter++;
  return node;
}

//RELEASE_NODE(node:pointer to Node)

struct Node* CreateNode(int level, int key, void* value)
{
  union Vlink new_vlink;
  new_vlink.p = value
    new_vlink.d= false;

  struct Node* node = MALLOC_NODE();  // Create new node
  node->prev = NULL;
  node->validLevel = 0;
  node->level = level;
  node->key = key;
  //TODO add next here?
  node->next = malloc(sizeof(union Link) * level);
  node->value = new_vlink
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

