// Name: 	Junzhe Liu
// USC NetID: 	2270250947
// CS 455 PA4
// Fall 2019

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Scanner;
import java.util.NavigableMap;


/**
   A dictionary of all anagram sets. 
   Note: the processing is case-sensitive; so if the dictionary has all lower
   case words, you will likely want any string you test to have all lower case
   letters too, and likewise if the dictionary words are all upper case.
 */

public class AnagramDictionary 
{
	/*
	 * myAnagramDic: a map store all input anagrams and its corresponding words
	 * key: anagrams, the canonical form of the words
	 * value: words contain the same exact letters in a different order, belongs to a same anagram
	 */
	private Map<String, TreeSet<String>> myAnagramDic;		
	// We use TreeSet to store words so it can automatically sort them in alphabetical order and prevent duplicates.

	
    /**
      Create an anagram dictionary from the list of words given in the file
      indicated by fileName.  
      PRE: The strings in the file are unique.
      @param fileName  the name of the file to read from
      @throws FileNotFoundException if the file is not found
     */
    public AnagramDictionary(String fileName) throws FileNotFoundException 
    {
	   File inFile = new File(fileName);				// Try to open this file
	   myAnagramDic = new HashMap<String, TreeSet<String>>();		// Create a HashMap
	   if(!inFile.exists())
	   {
		   throw new FileNotFoundException("Dictionary file not found: " + fileName);		// error message
	   }
	   try (Scanner in = new Scanner(inFile))
	   {
		   while(in.hasNext())			// till end of this file
		   {
			   String word = in.next();
			   String canon = Rack.findCanonical(word);			// find the canonical form, it's also the anagram
			   TreeSet<String> anagrams= 
					   myAnagramDic.containsKey(canon) ? myAnagramDic.get(canon) : new TreeSet<String>();
			   anagrams.add(word);			// add the word into the set, TreeSet will maintain the alphabetical order
			   myAnagramDic.put(canon, anagrams);			// modify the value
		   }
	   }
    }
   

   /**
      Get all anagrams of the given string. This method is case-sensitive.
      E.g. "CARE" and "race" would not be recognized as anagrams.
      @param s string to process
      @return a list of the anagrams of s
    */
    public ArrayList<String> getAnagramsOf(String word) 
    {
    		String canon = Rack.findCanonical(word);			// get the canonical form of this word
		if(myAnagramDic.containsKey(canon))				// if this canonical form is inside the keySet of dictionary
		{
			return new ArrayList<String>(myAnagramDic.get(canon));			// return this TreeSet<String>
		}
		else				// We do not find this anagram inside the dictionary
		{
			return new ArrayList<String>();				// return an empty ArrayList
		}
    }
    
    
    /**
     * Get all matching words of the given word. This method is also case-sensitive
     * @param word: a given string
     * @return a map with its key indicating the score of the words, 
     * and its corresponding value is a TreeSet containing all matching words in the dictionary with the same score.
     */
    public NavigableMap<Integer, TreeSet<String>> getAllMatchsOf(String word)
    {
 	   NavigableMap<Integer, TreeSet<String> > allMatchings = new TreeMap<Integer, TreeSet<String> >();
 	   /*
 	    * a map with its key indicating the score of the words, 
 	    * and its corresponding value is a TreeSet containing all matching words in the dictionary with the same score.
 	    * TreeSet will automatically sort the words in alphabetical order
 	    * We also use TreeMap to implement the NavigableMap because we can sort the scores in an increasing order
 	    * With NavigableMap.descendingMap() we can inverse the mapping according to the order of decreasing score.
 	    */
 	   
 	   Rack rack = new Rack(word);			// build a rack of this word, this includes filter and findcanonical
 	   ScoreTable table = new ScoreTable();	// Score calculator
 	   ArrayList<String> subString = rack.getAllSubString();			// get all substrings of this word
 	   for(String s: subString)				// for every substring, find all its anagrams and put it into the TreeSet
 	   {
 		   ArrayList<String> result = this.getAnagramsOf(s);			// find all anagrams of this substring
 		   if(result.size()==0)				// Nothing Found
 		   {
 			   continue;
 		   }
 		   int score = table.calcScore(s);	// Compute the score of this substring
 		   TreeSet<String> set = 
 				   allMatchings.containsKey(score) ? allMatchings.get(score) : new TreeSet<String>();
 		   set.addAll(result);				// Add all anagrams into the TreeSet
 		   allMatchings.put(score, set);		// put the modified TreeSet back into the NavigableMap
 	   }
 	   return allMatchings;
    }
}
