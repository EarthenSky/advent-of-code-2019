using System;
//using System.Collections.Generic;  // for lists
//using System.IO;  // for files

public class Program 
{
    public static void Main(string[] args) {
        Analysis a = new Analysis();
        a.FillFromFile("input");
        
        Console.WriteLine("total orbits = {0}", a.GetOrbitsCount());
        Console.WriteLine("distance between SAN and YOU = {0}", a.FindDistance("SAN", "YOU"));
    } 
}


public class _ {


}
