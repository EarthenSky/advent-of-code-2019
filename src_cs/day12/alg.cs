using System;

// Useful algorithms
namespace Algorithms {

    public class Alg {
        // returns index of nth instance of supplied char.
        public static int FindNth(string s, char ch, int n) {
            int counter = -1, i = 0;
            foreach(char ci in s) {
                if(ci == ch) {
                    counter++;

                    if(counter == n)  // case: found nth ch
                        return i; 
                }
                i++;
            } 
            
            return -1;
        }

        // Greatest common factor
        public static long Gcf(long a, long b) {
            while (b != 0)
            {
                long temp = b;
                b = a % b;
                a = temp;
            }
            return a;
        }

        // Greatest common factor
        public static int Gcf(int a, int b) {
            while (b != 0)
            {
                int temp = b;
                b = a % b;
                a = temp;
            }
            return a;
        }

        // Lowest Common Multiple (long)
        public static long Lcm(long a, long b) {
            return (a / Gcf(a, b)) * b;
        }

        // Lowest Common Multiple
        public static int Lcm(int a, int b) {
            return (a / Gcf(a, b)) * b;
        }
        
    }
    
}
