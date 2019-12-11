// Name: 	Junzhe Liu
// USC NetID: 	2270250947
// CS 455 PA4
// Fall 2019


public class ScoreTable 
{
	/**
	 * This class has information about Scrabble scores for scrabble letters and words. 
	 * In scrabble not every letter has the same value. 
	 * Letters that occur more often in the English language are worth less 
	 * (e.g., 'e' and 's' are each worth 1 point), 
	 * and letters that occur less often are worth more (e.g., 'q' and 'z' are worth 10 points each). 
	 * We used hard-coded values in its data. 
	 * 
	 * This class should work for both upper and lower case versions of the letters, 
	 * e.g., 'a' and 'A' will have the same score. 
	 */
	
	
	private int [] letterScores;
	
	
	public ScoreTable()				// Hard-coded letter value table
	{
		letterScores = new int [26];
		letterScores[0] = 1;		// a / A
		letterScores[1] = 3;		// b / B
		letterScores[2] = 3;		// c / C
		letterScores[3] = 2;		// d / D
		letterScores[4] = 1;		// e / E
		letterScores[5] = 4;		// f / F
		letterScores[6] = 2;		// g / G
		letterScores[7] = 4;		// h / H
		letterScores[8] = 1;		// i / I
		letterScores[9] = 8;		// j / J
		letterScores[10] = 5;		// k / K
		letterScores[11] = 1;		// l / L
		letterScores[12] = 3;		// m / M
		letterScores[13] = 1;		// n / N
		letterScores[14] = 1;		// o / O
		letterScores[15] = 3;		// p / P
		letterScores[16] = 10;		// q / Q
		letterScores[17] = 1;		// r / R
		letterScores[18] = 1;		// s / S
		letterScores[19] = 1;		// t / T
		letterScores[20] = 1;		// u / U
		letterScores[21] = 4;		// v / V
		letterScores[22] = 4;		// w / W
		letterScores[23] = 8;		// x / X
		letterScores[24] = 4;		// y / Y
		letterScores[25] = 10;		// z / Z
	}
	
	
	public int calcScore(String word)			// Compute the total score of a given word
	{
		int score = 0;
		word = Rack.filter(word);				// remove those non-letter characters in the String (e.g. @*^&$)
		word = word.toLowerCase();				// turn all Uppercase letter into Lowercase, since 'a' and 'A' has the same score
		for(int i=0; i< word.length(); i++)
		{
			char letter = word.charAt(i);
			score += letterScores[letter-'a'];	// add the score letter by letter
		}
		return score;
	}
	
}
