#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    Node *next;
};

typedef struct Node Node;

static Node *first = NULL;

static int insertElement(int value) {    
if(value >= 0)
    {
		//Check if this is the first node
        if(first == NULL)
        {
            first = (struct Node *)malloc(sizeof(struct Node));
            if(first == NULL)
            {
                return -1;
            }
            else
            {
                first->data = value;      
                first->next = NULL; 
                return value;
            }
        }
		//Any node after the first one
        else
        {
            struct Node *temp;
            temp = first;

			//Check if value already exists in stack
			//Also sets temp to top node of stack
            while(temp != NULL)
            {
                if(temp->data == value) return -1;
                if(temp->next == NULL) break;
                temp = temp->next;
            }
            
			//New Value: Add new Node
            Node *newNode = NULL;
            newNode = (struct Node *)malloc(sizeof(struct Node));
			
            if(newNode == NULL)
            {
                return -1;
            }
            else
            {
                newNode->data = value;
				//temp is the top node of the stack
                temp->next = newNode;
                newNode->next = NULL;
                return value;
            }
        }
    }
	return -1;
}

static int removeElement(void) {
	
	//head does not exist (list empty)
	if (first == NULL) return -1;
	
	int value;
	
	//only head remains
	if (first->next == NULL)
	{
		value = first->data;
		free(first);
		return value;
	}
	
	Node *last = NULL;
	last = first->next;
	
	Node *prevLast = NULL;
	prevLast = first;
	
	//find newest two nodes
	while (last->next)
	{
		prevLast = last;
		last = last->next;
	}
	
	if(last != NULL and prevLast != NULL)
	{
		value = last->data;
		free(last);
		prevLast->next = NULL;
		return value;
	}
	
	return -1;
}

int main (int argc, char* argv[]) {
	printf("insert 47: %d\n", insertElement(47));
	printf("insert 11: %d\n", insertElement(11));
	printf("insert 23: %d\n", insertElement(23));
	printf("insert 11: %d\n", insertElement(11));

	printf("remove: %d\n", removeElement());
	printf("remove: %d\n", removeElement());

	// TODO: add more tests
	exit(EXIT_SUCCESS);
}
