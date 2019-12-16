using System;

public class Program 
{
    public static void Main(string[] args) {
        int start = 387638, end = 919123;
        AnalysisPart2 a = new AnalysisPart2(start, end);

        // Get possible outputs
        Console.WriteLine("Possible combinations = {0}", a.GetCombinations());
    } 
}

// this is the code for part 1
public class AnalysisPart1 
{
    private int start;
    private int end;

    public AnalysisPart1(int start, int end) {
        this.start = start;
        this.end = end;
    }

    public int GetCombinations() {
        int counter = 0;
        
        // initialize the array of numbers representing a password. (damn, look at that lambda)
        int[] number = Array.ConvertAll(start.ToString().ToCharArray(), c => c - '0');

        // check every number within the range end to start.
        for(int i = 0; i < end - start; i++) {

            // check all pairs of characters and determine validity based on rules.
            bool goodNumber = false;
            for (int j = 1; j < number.Length; j++) {
                if(number[j - 1] > number[j]) {
                    goodNumber = false;
                    break;
                } else if(number[j - 1] == number[j]) {
                    goodNumber = true;
                }
            }

            if (goodNumber) counter++;

            // increment number;
            for (int j = number.Length - 1; j >= 0; j--) {
                if(number[j] != 9) {
                    number[j]++;
                    break;
                } else {
                    number[j] = 0;
                }
            }
        }

        return counter;
    }
} 

// this is the code for part 2
public class AnalysisPart2
{
    const int LEN = 6;
    private int start;
    private int end;

    public AnalysisPart2(int start, int end) {
        this.start = start;
        this.end = end;
    }

    public int GetCombinations() {
        int counter = 0;
        
        // initialize the array of numbers representing a password. (damn, look at that lambda)
        int[] number = Array.ConvertAll(start.ToString().ToCharArray(), c => c - '0');

        // check every number within the range end to start.
        for(int i = 0; i < end - start; i++) {
            bool goodNumber = false;

            // check for double digits using a 4 grid size lense and edge cases.
            if (number[0] == number[1] && number[2] != number[0] ||
                    number[LEN - 1] == number[LEN - 2] && number[LEN - 3] != number[LEN - 2]) {
                goodNumber = true;
            } else {
                for (int j = 3; j < LEN; j++) {
                    if(number[j - 1] == number[j - 2] && number[j - 3] != number[j - 2] && number[j - 1] != number[j]) {
                        goodNumber = true;
                        break;
                    }
                }
            }

            // make sure number is increasing
            for (int j = 1; j < LEN; j++) {
                if(number[j - 1] > number[j]) {
                    goodNumber = false;
                    break;
                } 
            }
            
            if (goodNumber) counter++;

            // increment number;
            for (int j = number.Length - 1; j >= 0; j--) {
                if(number[j] != 9) {
                    number[j]++;
                    break;
                } else {
                    number[j] = 0;
                }
            }
        }

        return counter;
    }
} 