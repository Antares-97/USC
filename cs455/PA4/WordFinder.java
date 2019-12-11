// Name: 	Junzhe Liu
// USC NetID: 	2270250947
// CS 455 PA4
// Fall 2019

import java.io.*; 
import java.util.Scanner;
import java.util.TreeSet;
import java.util.NavigableMap;

public class WordFinder 
{
	/**
	 * Class WordFinder contains the main method. 
	 * This class will have a main that's responsible for processing the command-line argument, 
	 * and handling any error processing. It will probably also have the main command loop. 
	 * Most of the other functionality will be delegated to 
	 * other object(s) created in main and their methods.
	 */
	
	/**
	 * Main Function
	 * Process the command-line argument
	 * tries to open the designated dictionary file
	 * Might raise FileNotFoundException error, if the dictionary file is not found
	 * Handle this exception by printing out the message
	 * 
	 * Also contains the loop of finding all the matching words for a given rack
	 */
	public static void main(String[] args) 
	{	
		AnagramDictionary myDic = null;
		String defaultDic = "/Users/liujunzhe/Desktop/Daily/USC/CS455/PA4/sowpods.txt";			//default dictionary
		if(args.length>0){			// If another dictionary file is designated, open that file
			try {
				myDic = new AnagramDictionary(args[0]);				// args[0] is the dictionary file name
			} catch (FileNotFoundException e) {
				System.out.println(e.getMessage());					// handle FileNotFoundException by printing message
			}
		}
		else{
			try {
				myDic = new AnagramDictionary(defaultDic);			// build dictionary on the default
			} catch (FileNotFoundException e) {
				System.out.println(e.getMessage());
			}
		}
		
		Scanner in = new Scanner(System.in);
		System.out.print("Type . to quit.\nRack? ");
		while(in.hasNext())					// Main loop, keep on looking for matchings of the input rack
		{
			String word = in.next();
			if(word.equals("."))			// break the loop when the input is "."
			{
				break;
			}
			findWord(word, myDic);			// Search the given rack in the dictionary, print out the results
			System.out.print("Rack? ");
		}
		in.close();
	}
	
	
	/**
	 * A private function within the main loop which does the search
	 * @param word: the input word, we need to find out all the matchings in the dictionary
	 * @param myDic: the given dictionary, we search the word in this dictionary and return all matching words
	 */
	private static void findWord(String word, AnagramDictionary myDic)
	{
		NavigableMap<Integer, TreeSet<String> > allFound = myDic.getAllMatchsOf(word);
		/*
		 * @param: allFound: a Map which stores all the matchings of "word" in the "myDic"
		 * the keys of allFound are the scores of the matched words
		 * allFound.get(key) is a treeSet, stores all the matching words that has the same score "key"
		 * treeSet has 2 benefits: 1. automatically sort the words in alphabetical order; 2.prevent duplicate
		 * I chose NavigableMap because with its method "descendingMap",
		 * we can easily sort the map in the order of decreasing score.
		 */
		
		if(allFound.size()==0)					// No matchings words were found
		{
			System.out.println("We can make 0 words from \"" + word + "\"");
			return;
		}
		int num = 0;							// Used to compute the total number of matching words
		for(int key: allFound.keySet())			// key: the score of the matching words
		{
			num += allFound.get(key).size();	// allFound.get(key) returns all the matching words that has the same score "key"
		}
		System.out.println("We can make " + num + " words from \"" + word + "\"");
		System.out.println("All of the words with their scores (sorted by score):");
		allFound = allFound.descendingMap();	// descendingMap() returns a map in the reversed order of key,
												// Sort the map in descending score
		for(int key: allFound.keySet())			// In decreasing order of the scores
		{
			TreeSet<String> tempSet = allFound.get(key);
			for(String tempWord: tempSet)
			{
				System.out.println(key+": "+ tempWord);
			}
		}
		return;
	}
}
