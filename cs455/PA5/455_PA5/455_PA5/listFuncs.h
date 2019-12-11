// Name:    Junzhe Liu
// USC NetID:   2270250947
// CSCI 455 PA5
// Fall 2019


//*************************************************************************
// Node class definition 
// and declarations for functions on ListType

// Note: we don't need Node in Table.h
// because it's used by the Table class; not by any Table client code.

// Note2: it's good practice to not put "using" statement in *header* files.  Thus
// here, things from std libary appear as, for example, std::string

#ifndef LIST_FUNCS_H
#define LIST_FUNCS_H
  

struct Node
{
    std::string key;
    int value;
    
    Node *next;
    
    Node(const std::string &theKey, int theValue);
    
    Node(const std::string &theKey, int theValue, Node *n);
};


typedef Node * ListType;

//*************************************************************************
//add function headers (aka, function prototypes) for your functions
//that operate on a list here (i.e., each includes a parameter of type
//ListType or ListType&).  No function definitions go in this file.


/**
   listLookUp function: look up a key in the list, returns the pointer pointing to the node where the key equals target
   @param list a pointer pointing to the head node of a link list
   @param target the key string
   @return a pointer pointing to the node which its key == target, or a NULL pointer if no such node is found in the given link list
*/
ListType listLookUp(ListType & list, std::string target);               

/**
   listInsert function: insert a key-value pair into the list
   @param list a pointer pointing to the head node of a link list
   @param target the key string
   @param value the corresponding value of string target
   @return false if the key is already present in the table, or true otherwise.
*/
bool listInsert(ListType & list, std::string target, int value);

/**
   listChange function: change the value of a given string key in the list
   @param list a pointer pointing to the head node of a link list
   @param target the key string
   @param newValue the new value of string target
   @return false if the key is absent in the table, or true otherwise.
*/
bool listChange(ListType & list, std::string target, int newValue);

/**
   listRemove function: remove the node of a given string key in the list
   @param list a pointer pointing to the head node of a link list
   @param target the key string
   @return false if the key is absent in the table, or true otherwise.
*/
bool listRemove(ListType & list, std::string target);

/**
   listPrint function: print all the key-value pair in the list
   @param list a pointer pointing to the head node of a link list
*/
void listPrint(ListType list);

/**
   listSize function: return the number of the nodes in the table
   @param list a pointer pointing to the head node of a link list
   @return number of the nodes in the table
*/
int listSize(ListType list);


// keep the following line at the end of the file
#endif
