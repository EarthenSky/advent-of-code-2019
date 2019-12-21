using System;

public class Program 
{
    // this function is a static member of the class program.
    public static void Main(string[] args) 
    {
        Console.WriteLine("Hello World Mono! \nPrinting supplied arguments:");
        PrintArgs(args);
        Console.WriteLine("Program Done!");
    } 

    public static void PrintArgs(string[] args) {
        int i = 0;
        foreach(string arg in args)
            Console.WriteLine("arg{0}: {1}", i++, arg);
    }
}