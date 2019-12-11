// Name:    Junzhe Liu
// USC NetID:   2270250947
// CSCI 455 PA5
// Fall 2019


#include <iostream>
#include <cassert>
#include "listFuncs.h"
using namespace std;


Node::Node(const string &theKey, int theValue)
{
    key = theKey;
    value = theValue;
    next = NULL;
}


Node::Node(const string &theKey, int theValue, Node *n)
{
    key = theKey;
    value = theValue;
    next = n;
}



//*************************************************************************
// put the function definitions for your list functions below


/**
    listLookUp function: look up a key in the list, returns the pointer pointing to the node where the key equals target
    @param list a pointer pointing to the head node of a link list
    @param target the key string
    @return a pointer pointing to the node which its key == target, or a NULL pointer if no such node is found in the given link list
 */
ListType listLookUp(ListType & list, string target)
{
    ListType p = list;
    while(p != NULL)            //traverse the list, and compare all nodes with the target string
    {
        if(p->key == target)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}


/**
   listInsert function: insert a key-value pair into the list
   @param list a pointer pointing to the head node of a link list
   @param target the key string
   @param value the corresponding value of string target
   @return false if the key is already present in the table, or true otherwise.
*/
bool listInsert(ListType & list, string target, int value)
{
    ListType p = listLookUp(list, target);      // If the key is already in the list, do not fo the insertion, return false.
    if (p != NULL)
    {
        return false;
    }
    list = new Node(target, value, list);       // Insert the new node at the head of the list, and the list pointer pointing to the new head.
    return true;
}


/**
   listChange function: change the value of a given string key in the list
   @param list a pointer pointing to the head node of a link list
   @param target the key string
   @param newValue the new value of string target
   @return false if the key is absent in the table, or true otherwise.
*/
bool listChange(ListType & list, string target, int newValue)
{
    ListType p = listLookUp(list, target);      // If the key is not in the list, do not fo the change, return false.
    if (p == NULL)
    {
        return false;
    }
    p->value = newValue;                        // Else modify the value of the key to the new value.
    return true;
}


/**
   listRemove function: remove the node of a given string key in the list
   @param list a pointer pointing to the head node of a link list
   @param target the key string
   @return false if the key is absent in the table, or true otherwise.
*/
bool listRemove(ListType & list, string target)
{
    if(list == NULL)                            // Empty list, return false immediately
    {
        return false;
    }
    ListType p = list;
    if(list->key == target)                     // If the head node is the node to be removed
    {
        list = p->next;
        p->next = NULL;                         // modify the head pointer to the next node
        return true;
    }
    while(p != NULL)                            // Otherwise, traverse the link list to find the deletion position
    {
        ListType pnext = p->next;
        if(pnext == NULL)                       // do not find the node to be removed, return false
        {
            return false;
        }
        if(pnext->key == target)                // Found
        {
            p->next = pnext->next;
            pnext->next = NULL;                 // Modify the pointer to make the removal
            return true;
        }
        p = p->next;
    }
    return false;
}


/**
   listPrint function: print all the key-value pair in the list
   @param list a pointer pointing to the head node of a link list
*/
void listPrint(ListType list)
{
    ListType p = list;
    while(p != NULL)
    {
        cout<< p->key << " " << p->value <<endl;
        p = p->next;
    }
}


/**
   listSize function: return the number of the nodes in the table
   @param list a pointer pointing to the head node of a link list
   @return number of the nodes in the table
*/
int listSize(ListType list)
{
    ListType p = list;
    int num = 0;
    while(p != NULL)
    {
        num++;
        p = p->next;
    }
    return num;
}
