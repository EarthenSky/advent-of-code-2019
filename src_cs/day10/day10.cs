using System;
using System.Collections.Generic;  // for linked lists
using System.IO;  // for files
using System.Text;  // for StringBuilder

public class Program 
{
    public static void Main(string[] args) {
        AsteriodTracker tracker = new AsteriodTracker();
        tracker.LoadMapFromFile("input", 24, 24);
        //tracker.LoadMapFromFile("test_input", 10, 10);
        //tracker.LoadMapFromFile("test_input_2", 10, 10);

        int asteroids = tracker.GetMostAsteroids();

        Console.WriteLine( "Most Asteroids = {0}", asteroids );
    } 
}


// holds 2d graph location (integer, not floating point)
public struct Point2D {
    public int x, y;
    public Point2D(int x, int y) {
        this.x = x;
        this.y = y;
    }
}

public class Algorithms {
    // returns positive output.
    public static int Gcd(int x, int y) {
        while (y != 0) {
            int tmp = x % y;
            x = y;
            y = tmp;
        }

        return Math.Abs(x);
    }
}


public class AsteriodTracker {
    
    private int _width;
    private int _height;
    private bool _isMapLoaded;

    private string[] _map;
    public AsteriodTracker() { 
        _isMapLoaded = false;
    }

    // could get height and width from map data but don't need to.
    public void LoadMapFromFile (string filename, int width, int height) {
        _width = width;
        _height = height;

        _map = File.ReadAllLines(filename);  // good enough?
        _isMapLoaded = true;

        /* // init image data in readable form. 
        _layerArray = new string[_layerCount];
        for(int i = 0; i < _layerCount; i++) {
            _layerArray[i] = line.Substring(i * _layerSize, _layerSize);
        } */
    }

    private void FillAsteroids (LinkedList<Point2D> asteroidList/*, bool sort=false*/) {
        for (int y = 0; y < _height; y++)
            for (int x = 0; x < _width; x++) 
                if(_map[x][y] == '#')
                    asteroidList.AddFirst(new Point2D(x, y));
    }

    // figures out how many asteroids can be detected from a single point.
    private int AsteroidsFromPosition (int x, int y) {
        LinkedList<Point2D> asteroidList = new LinkedList<Point2D>();

        // Todo: sort asteroidList by radial closeness before running algorithm for better performance.
        FillAsteroids(asteroidList);
        asteroidList.Remove( new Point2D(x, y) );

/*
        // For printing out later.
        StringBuilder[] debug_map = new StringBuilder[_map.Length];
        // makes copy deep!
        for(int i = 0; i < debug_map.Length; i++)
            debug_map[i] = new StringBuilder(_map[i]);
*/

        // LinkedList should allow for more editing of the list while iterating through it.
        LinkedListNode<Point2D> node = asteroidList.First;
        while(node != null) {
            
            Point2D asteroid = node.Value;
            if ( !(asteroid.x == x && asteroid.y == y) ) {
                int difX = asteroid.x - x;
                int difY = asteroid.y - y;
                
                int gcd = Algorithms.Gcd(difX, difY);

                // gcd preserves sign so that step has direction.
                int stepX = difX / gcd;  
                int stepY = difY / gcd;

                int posX = asteroid.x + stepX;
                int posY = asteroid.y + stepY;

                // Remove all blocked asteroids from the list linked list.
                while(posX >= 0 && posX < _width && posY >= 0 && posY < _height) {
//                    if( asteroidList.Contains(new Point2D(posX, posY)) ) debug_map[posX][posY] = 'O';
                    asteroidList.Remove( new Point2D(posX, posY) );  // this function call makes this code slow. ~0(n^3)?
                    posX += stepX;
                    posY += stepY;
                }
            }
    
            node = node.Next;
        }

//        debug_map[x][y] = 'X';
//        Console.WriteLine("returning: count={0}", asteroidList.Count);
//        Console.WriteLine("----------------------------------------");
//        foreach(StringBuilder row in debug_map) Console.WriteLine(row.ToString());
//        Console.WriteLine("----------------------------------------");
        return asteroidList.Count;
    }    

    // Position with most asteroids in view.
    public int GetMostAsteroids () {
        if(!_isMapLoaded) { Console.WriteLine("Error, map not loaded"); return -1; }

        LinkedList<Point2D> asteroidList = new LinkedList<Point2D>();
        FillAsteroids(asteroidList);

        // find max position
        int max = -1;
        //Point2D point;
        foreach(Point2D asteroid in asteroidList) {
            int asteroids = AsteroidsFromPosition(asteroid.x, asteroid.y);
            if(asteroids > max) {
                max = asteroids;
                //point = new Point2D(asteroid.x, asteroid.y);
            }
        }
        
        return max;
    } 

}