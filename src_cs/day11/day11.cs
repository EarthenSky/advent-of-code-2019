using System;
using System.Collections.Generic;  // for linked lists
using System.IO;  // for files
using System.Text;  // for StringBuilder

public class Program 
{
    public static void Main(string[] args) {
        AsteriodTracker tracker = new AsteriodTracker();
        tracker.LoadMapFromFile("input", 24, 24);
        //tracker.LoadMapFromFile("test_input_3", 20, 20);
                
        int asteroids = tracker.GetMostAsteroids();
        Console.WriteLine( "Most Asteroids = {0}", asteroids );

        Point2D rock200 = tracker.GetNthLazeredRock(200);
        Console.WriteLine( "Special Asteroid Lazer Number = {0}", 100 * rock200.x + rock200.y);
        
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

// holds 2d location ($x and $y)and a piece of data called $val (val is double)
public struct DataPoint2D {
    public int x, y;
    public int val;
    public DataPoint2D(int x, int y, int val) {
        this.x = x;
        this.y = y;
        this.val = val;
    }

    public Point2D ToPoint2D() {
        return new Point2D(x, y);
    }

    public int DistanceFrom(DataPoint2D p) {
        Func<int, int> square = n => n * n;
        return (int)Math.Round(Math.Sqrt( square(p.x - x) + square(p.y - y) ));
    }
  
    public int DistanceFrom(Point2D p) {
        Func<int, int> square = n => n * n;
        return (int)Math.Round(Math.Sqrt( square(p.x - x) + square(p.y - y) ));
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

    private Point2D _station;
    private bool _isStationSet;

    private string[] _map;
    public AsteriodTracker() { 
        _isMapLoaded = false;
        _isStationSet = false;
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
                if(_map[y][x] == '#')
                    asteroidList.AddFirst(new Point2D(x, y));
    }

    private void FillAsteroidAngles(List<DataPoint2D> asteroidList) {
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {

                // Todo: is this being counted properly? Check Atan2()'s Output
                if(_map[y][x] == '#' && !(_station.x == x && _station.y == y)) {
                    int difX = -(_station.x - x);
                    int difY = _station.y - y;

                    double angle = Math.Atan2(difX, difY);

                    if(difX < 0) angle += Math.PI * 2;

                    asteroidList.Add(new DataPoint2D(x, y, (int)(angle * 18000 / Math.PI)));
                }
            }
        }
    }

    // figures out how many asteroids can be detected from a single point.
    private int AsteroidsFromPosition (int x, int y) {
        LinkedList<Point2D> asteroidList = new LinkedList<Point2D>();

        // Todo: sort asteroidList by radial closeness before running algorithm for better performance.
        FillAsteroids(asteroidList);
        asteroidList.Remove( new Point2D(x, y) );

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
                    asteroidList.Remove( new Point2D(posX, posY) );  // this function call makes this code slow. ~0(n^3)?
                    posX += stepX;
                    posY += stepY;
                }
            }
    
            node = node.Next;
        }

        return asteroidList.Count;
    }    

    // Position with most asteroids in view.
    public int GetMostAsteroids () {
        if(!_isMapLoaded) { Console.WriteLine("Error, map not loaded"); return -1; }

        LinkedList<Point2D> asteroidList = new LinkedList<Point2D>();
        FillAsteroids(asteroidList);

        // find max position
        int max = -1;
        Point2D station = new Point2D(-1, -1);
        foreach(Point2D asteroid in asteroidList) {
            int asteroids = AsteroidsFromPosition(asteroid.x, asteroid.y);
            if(asteroids > max) {
                max = asteroids;
                station = asteroid;
            }
        }

        _station = station;
        _isStationSet = true;
        
        return max;
    } 

    // sort by angle, then closeness to the station (dp -> data point)
    private int AsteriodSort(DataPoint2D dp1, DataPoint2D dp2) {
        if(dp1.val == dp2.val) {
            int dis1 = dp1.DistanceFrom(_station);
            int dis2 = dp2.DistanceFrom(_station);
            return dis1 == dis2 ? 0 : (dis1 > dis2 ? 1 : -1);
        } else {
            return dp1.val > dp2.val ? 1 : -1;
        }
    }

    // Gets the posiotion of the rock destroyed by nth by the lazer. (n > 0)
    public Point2D GetNthLazeredRock(int n) {
        if(!_isMapLoaded) { Console.WriteLine("Error, map not loaded"); return new Point2D(-1, -1); }
        if(!_isStationSet) { Console.WriteLine("Error, station not set"); return new Point2D(-2, -2); }
        
        List<DataPoint2D> astSorted = new List<DataPoint2D>(); // linked list faster?
        FillAsteroidAngles(astSorted);

        // sort by angle, then closeness to the station.
        astSorted.Sort(AsteriodSort);  

        LinkedList<DataPoint2D> astOrderd = new LinkedList<DataPoint2D>(); // proper order (for lazering)

        int lastTaken = astSorted[0].val + 1; 
        int i = 0;
        while(astSorted.Count > 0) {
            // Since this is at the top, we know that astSorted[0] exists
            if (i > astSorted.Count - 1) {
                lastTaken = astSorted[0].val + 1;  // First asteroid of an iteration is always taken.
                i = 0;
            }

            // Take only the first item in a group of duplicate positions. 
            if(astSorted[i].val != lastTaken) {
                astOrderd.AddLast(astSorted[i]);
                lastTaken = astSorted[i].val;

                // Remove item and fix index.
                astSorted.RemoveAt(i);
            } else {
                i++;
            }
        }

        // Move to nth node.
        i = 0;
        LinkedListNode<DataPoint2D> node = astOrderd.First; 
        while(i < n-1) {
            node = node.Next;
            i++;
        }

        return node.Value.ToPoint2D();
    }

}