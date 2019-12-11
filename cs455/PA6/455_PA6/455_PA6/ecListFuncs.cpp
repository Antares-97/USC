/*  Name:   Junzhe Liu
 *  USC NetID:  2270250947
 *  CS 455 Fall 2019
 *
 *  See ecListFuncs.h for specification of each function.
 */


// for NULL
#include <cstdlib>

// in case you want to use assert statements
#include <cassert>

#include "ecListFuncs.h"

using namespace std;


void longestRun(ListType list, int & maxRunVal, int & maxRunLen)
{
    if(list == NULL)
        return;
    ListType p = list;
    int num = p->data;
    int len = 1;
    maxRunVal = p->data;
    maxRunLen = 1;
    while( p!= NULL )
    {
        if(len > maxRunLen)
        {
            maxRunLen = len;
            maxRunVal = num;
        }
        len = 1;
        num = p->data;
        p = p->next;
        while ( p != NULL && p->data == num)
        {
            len++;
            p = p->next;
        }
    }
    if(len > maxRunLen)
    {
        maxRunLen = len;
        maxRunVal = num;
    }
}


void removeMultiplesOf3(ListType & list)
{
    ListType p = list;
    while(p != NULL && p->data % 3 == 0)
    {
        list = list->next;
        p->next = NULL;
        delete p;
        p = list;
    }
    if(p == NULL)
        return;
    ListType prev = p;
    p = p->next;
    while (p != NULL)
    {
        if(p->data %3 == 0)
        {
            prev->next = p->next;
            ListType temp = p;
            p = p->next;
            delete temp;
            temp = NULL;
        }
        else
        {
            prev = p;
            p = p->next;
        }
    }
}


void insertMiddle(ListType & list, int midVal)
{
    int len = 0;
    ListType p = list;
    while (p != NULL)
    {
        len++;
        p = p->next;
    }
    if(len == 0)
    {
        list = new Node(midVal, NULL);
        return;
    }
    if(len == 1)
    {
        list = new Node(midVal, list);
        return;
    }
    int mid = len/2;
    p = list;
    for(int i=1; i<mid; i++)
        p = p->next;
    ListType q = new Node(midVal, p->next);
    p->next = q;
}


ListType merge(ListType list1, ListType list2)
{
    if(list2 == NULL)
        return list1;
    if(list1 == NULL)
        return list2;
    
    ListType p1 = list1;
    ListType p2 = list2;
    bool flag = true;                   //  true if last element belongs to list1
    while(p1 != NULL && p2 != NULL)
    {
        if(p1->data > p2->data)
        {
            flag = true;
            ListType prev2 = p2;
            p2 = p2->next;
            while(p2 != NULL && p1->data > p2->data)
            {
                prev2 = p2;
                p2 = p2->next;
            }
            prev2->next = p1;
            if(p1 == list1)
                list1 = list2;
        }
        else if(p1->data < p2->data)
        {
            flag = false;
            ListType prev1 = p1;
            p1 = p1->next;
            while(p1 != NULL && p1->data < p2->data)
            {
                prev1 = p1;
                p1 = p1->next;
            }
            prev1->next = p2;
        }
        else
        {
            ListType temp = flag ? p2 : p1;
            if(flag)
                p2 = p2->next;
            else
                p1 = p1->next;
            delete temp;
            temp = NULL;
        }
    }
    
    if(p1 == NULL)      // p1 is at the end of list1, but p2 isn't.
        p1 = p2;        // put the rest element in p2 at the end of list 1

    return list1;
}

