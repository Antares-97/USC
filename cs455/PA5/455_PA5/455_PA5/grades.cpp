// Name:    Junzhe Liu
// USC NetID:   2270250947
// CSCI 455 PA5
// Fall 2019

/*
 * grades.cpp
 * A program to test the Table class.
 * How to run it:
 *      grades [hashSize]
 * 
 * the optional argument hashSize is the size of hash table to use.
 * if it's not given, the program uses default size (Table::HASH_SIZE)
 *
 */

#include "Table.h"
#include <cstdlib>                  // cstdlib needed for call to atoi
using namespace std;

/**
   gradeInsert: Insert a  key-value pair into the table. If the key is alreary present in the table, print an error message.
   @param grades the given table
*/
void gradeInsert(Table * grades)
{
    string name;
    int score;
    cin >> name >> score;
    if(grades->insert(name, score)==false)      // Insertion error, the key is already present in the table
    {
        cout << name << " is already in the table. Insert failed."<<endl;
    }
}


/**
   gradeChange: Change the value of a given key in the table, if the key is not present in the table, print an error message.
   @param grades the given table
*/
void gradeChange(Table * grades)
{
    string name;
    int score;
    cin >> name >> score;
    int * p = grades->lookup(name);             // look up a name in the table
    if(p == NULL)                               // if the name is not in the table
    {
        cout << name << " is not in the table. Change failed."<<endl;
    }
    else
    {
        *p = score;                             // change the value of the key via the pointer
    }
}


/**
    gradeLookUp: look up a given key from the table, if the key is not present in the table, print a message indicating the absense. Else print the value of the key
    @param grades the given table
 */
void gradeLookUp(Table * grades)
{
    string name;
    cin >> name;
    int * p = grades->lookup(name);             // look up a name in the table
    if(p == NULL)                               // if the name is not present in the table
    {
        cout << name << " is not in the table. Lookup failed."<<endl;
    }
    else
    {
        cout << name << " has a score of " << *p << endl;
    }
}


/**
    gradeRemove: remove a given key from the table, if the key is not present in the table, print a message indicating the absense.
    @param grades the given table
 */
void gradeRemove(Table * grades)
{
    string name;
    cin >> name;
    if(grades->remove(name)==false)             // the key is not present in the table, remove error.
    {
        cout << name <<" is not in the table. Remove failed." << endl;
    }
}


/**
    printHelp function:  Print the command syntax mannual
 */
void printHelp()
{
    cout << "command syntax:" << endl;
    cout << "insert [name] [score] : Insert this name and score in the grade table." << endl;
    cout << "change [name] [newscore] : Change the score for name." << endl;
    cout << "lookup [name] : Lookup the name, and print out his or her score." << endl;
    cout << "remove [name] : Remove this student." << endl;
    cout << "print : Prints out all names and scores in the table." << endl;
    cout << "size : Prints out the number of entries in the table." << endl;
    cout << "stats : Prints out statistics about the hash table at this point." << endl;
    cout << "help : Prints out a brief command summary." << endl;
    cout << "quit : Exits the program." << endl;
}


/**
    prompt function: takes the command and makes the corresponding operation
    @param grades the hash table
    @param command the command string
 */
bool prompt(Table * grades, string command)
{
    if(command == "quit"){                  // If the command is "quit", return true immediately. Otherwise return false and continue the loop
        return true;
    }
    if(command == "insert"){                // Does a single insertion operation to the table
        gradeInsert(grades);
    }
    else if(command == "change"){           // Change a key in the table
        gradeChange(grades);
    }
    else if(command == "lookup"){           // Lookup a key in the table
        gradeLookUp(grades);
    }
    else if(command == "remove"){           // Remove a key in the table
        gradeRemove(grades);
    }
    else if(command == "print"){            // Print all key-value pair in the table
        grades->printAll();
    }
    else if(command == "size"){             // Print the number of entries in the table
        cout << "number of entries in the table: " << grades->numEntries() << endl;;
    }
    else if(command == "stats"){            // Print the current statistics of the table
        grades->hashStats(cout);
    }
    else if(command == "help"){             // Print command syntax
        printHelp();
    }
    else{                                   // Invalid command, print the command mannual.
        cout << "ERROR: invalid command" << endl;
        printHelp();
    }
    return false;                           // Continue the loop
}


int main(int argc, char * argv[])
{
                                            // gets the hash table size from the command line
    int hashSize = Table::HASH_SIZE;
    Table * grades;                         // Table is dynamically allocated below, so we can call
                                            // different constructors depending on input from the user.
    if (argc > 1)
    {
        hashSize = atoi(argv[1]);           // atoi converts c-string to int
        if (hashSize < 1)
        {
            cout << "Command line argument (hashSize) must be a positive number"<< endl;
            return 1;
        }
        grades = new Table(hashSize);
    }
    else                                    // no command line args given -- use default table size
    {
        grades = new Table();
    }
    
                                            // add more code here
                                            // Reminder: use -> when calling Table methods, since grades is type Table*
    grades->hashStats(cout);                // print hash statistics first after building hash table.
    cout<<"cmd> ";
    string command;
    while(cin >> command)                   // main loop
    {
        if(prompt(grades, command))         // prompt function takes the command and make the operation. if command == "quit", break the loop, else continue.
        {
            break;
        }
        cout<<"cmd> ";
    }
    
    return 0;
}

