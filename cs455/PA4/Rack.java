// Name: 	Junzhe Liu
// USC NetID: 	2270250947
// CS 455 PA4
// Fall 2019

import java.util.ArrayList;
import java.util.Arrays;


/**
   A Rack of Scrabble tiles
   This Rack Class contains several methods to process a string
   @param canonical: canonical form of a word -- a sorted version of the characters in the word.
   @param unique: remove duplicate letters in the canonical form, only unique letters remaining.
   @param mult: the multiplicity of that letter in the unique at the same array index.
 */
public class Rack 
{
	private String canonical;				// a sorted version of the characters in the word. (e.g. pace->acep)
	private String unique;					// unique letters in the canonical
	private int [] mult;					// the multiplicity of that letter at the same array index.
	private ArrayList<String> allSubstring;	// All the substrings of this word
	
	
	public Rack(String word)				// Constructor: takes a string
	{
		canonical = findCanonical(word);	// returns the canonical form of the string
		
		unique = "";
		ArrayList<Integer> multiplicity = new ArrayList<Integer>();
		int i = 0, num = 1;
		while(i < canonical.length())		// traverse the canonical form from left to right
		{
			char c = canonical.charAt(i);	// We meet another different character
			num = 1;						// the multiplicity of this letter starts from 1
			i++;							// move to the next letter
			while(i < canonical.length() && canonical.charAt(i) == c)
			{
				i++;						// jump over all the letters that equals to the current letter
				num++;						// The multiplicity increase accumulatively
			}
			unique += c;
			multiplicity.add(num);			// put this letter into the unique string and its multiplicity into the arraylist
		}
		mult = new int[unique.length()];
		for(int k=0; k < unique.length(); k++)
		{
			mult[k] = multiplicity.get(k);	// copy the values of multiplicity into the integer array mult.
		}
		allSubstring = allSubsets(unique, mult, 0);		// find all the substrings of this word
	}
	
	
	/**
	 * Returns the canonical form of the string
	 * @param word
	 * @return sorted version of the characters in the word.
	 */
	public static String findCanonical(String word)
	{
		word = filter(word);						// remove all the non-letter characters
		char [] wordArray = word.toCharArray();		// toCharArray() returns an integer array version of the string
		Arrays.sort(wordArray);						// with an integer array, we can sort it in increasing order
		String s = new String(wordArray);			// build a new string according to the sorted integer array
		return s;									// this will return the canonical form of the string
	}
	
	
	/**
	 * Applied Regular Expression, remove all the non-letter characters
	 * @param word
	 * @return A string will all letters
	 */
	public static String filter(String word)
	{
		word = word.replaceAll("[^(A-Za-z)]", "");	// Regular Expression, remove all non-letter characters
		return word;
	}
	
	
	/**
	 * return the arraylist which stores all the substrings of canonical
	 */
	public ArrayList<String> getAllSubString()
	{
		return allSubstring;
	}

	
	/**
      Finds all subsets of the multiset starting at position k in unique and mult.
      unique and mult describe a multiset such that mult[i] is the multiplicity of the char
           unique.charAt(i).
      PRE: mult.length must be at least as big as unique.length()
           0 <= k <= unique.length()
      @param unique a string of unique letters
      @param mult the multiplicity of each letter from unique.  
      @param k the smallest index of unique and mult to consider.
      @return all subsets of the indicated multiset.  Unlike the multiset in the parameters,
      each subset is represented as a String that can have repeated characters in it.
      @author Claire Bono
    */
   private static ArrayList<String> allSubsets(String unique, int[] mult, int k) 
   {
      ArrayList<String> allCombos = new ArrayList<>();
      
      if (k == unique.length()) {  // multiset is empty
         allCombos.add("");
         return allCombos;
      }
      
      // get all subsets of the multiset without the first unique char
      ArrayList<String> restCombos = allSubsets(unique, mult, k+1);
      
      // prepend all possible numbers of the first char (i.e., the one at position k) 
      // to the front of each string in restCombos.  Suppose that char is 'a'...
      
      String firstPart = "";          // in outer loop firstPart takes on the values: "", "a", "aa", ...
      for (int n = 0; n <= mult[k]; n++) 
      {   
         for (int i = 0; i < restCombos.size(); i++) 
         {  // for each of the subsets 
                                                        // we found in the recursive call
            // create and add a new string with n 'a's in front of that subset
            allCombos.add(firstPart + restCombos.get(i));  
         }
         firstPart += unique.charAt(k);  // append another instance of 'a' to the first part
      }  
      return allCombos;
   }

}
