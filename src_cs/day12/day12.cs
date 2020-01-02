using System;
using System.Collections;  // for hash table
using System.Collections.Generic;  // for lists
using System.IO;  // for files
using System.Diagnostics;  // for stopwatch
//using System.Text;  // for StringBuilder

using Algorithms;

public class Program 
{
    public static void Main(string[] args) {
        Stopwatch stopWatch = new Stopwatch();
        stopWatch.Start();

        SolarSystem sys = new SolarSystem();
        sys.LoadFromFile("input");
        
        Console.WriteLine("Part One:");
        PartOne(sys);

        Console.WriteLine("Part Two:");
        PartTwo(sys);

        stopWatch.Stop();
        Console.WriteLine("Time elapsed = {0} seconds", stopWatch.Elapsed.TotalSeconds);
    } 

    private static void PartOne (SolarSystem sys) {    
        sys.SimulateTimesteps(1000);

        int energy = sys.GetTotalEnergy();
        Console.WriteLine( "energy = {0}", energy);
    } 

    private static void PartTwo (SolarSystem sys) {
        sys.ResetPlanets();

        long xSteps = sys.StepsUntilAxisRepeats(Axis.X);
        long ySteps = sys.StepsUntilAxisRepeats(Axis.Y);
        long zSteps = sys.StepsUntilAxisRepeats(Axis.Z);

        Console.WriteLine( "xSteps = {0}", xSteps);
        Console.WriteLine( "ySteps = {0}", ySteps);
        Console.WriteLine( "zSteps = {0}", zSteps);

        long lcmXY = Alg.Lcm(xSteps, ySteps);
        Console.WriteLine( "lcm(x, y) = {0}", lcmXY );

        Console.WriteLine( "matchSteps = {0}", Alg.Lcm(lcmXY, zSteps) );


    } 
}


/*
;; lisp comments ( I failed to learn lisp D: ):

;; class solar system
;; ;; member function: simulate-timesteps (n)
;; ;; member function: get-total-system-energy

;; class planet
;; ;; position & velocity

;; create a solar system class instance
;; simulate 100 timesteps
;; get the total system energy


;; done
*/

public enum Axis { X, Y, Z }

public class Planet {
    public int x, y, z;
    public int velX, velY, velZ;

    public Planet(int x, int y, int z) {
        this.x = x;
        this.y = y;
        this.z = z;
        
        velX = 0;
        velY = 0;
        velZ = 0;
    }

    public int GetAxis(Axis axis) {
        if(axis == Axis.X) {
            return x;
        } else if(axis == Axis.Y) {
            return y;
        } else { //if(axis == Axis.Z) {
            return z;
        }
    }

    public int GetVelAxis(Axis axis) {
        if(axis == Axis.X) {
            return velX;
        } else if(axis == Axis.Y) {
            return velY;
        } else { //if(axis == Axis.Z) {
            return velZ;
        }
    }

    public void SetAxis(Axis axis, int val) {
        if(axis == Axis.X) {
            x = val;
        } else if(axis == Axis.Y) {
            y = val;
        } else if(axis == Axis.Z) {
            z = val;
        }
    }

    public void SetVelAxis(Axis axis, int val) {
        if(axis == Axis.X) {
            velX = val;
        } else if(axis == Axis.Y) {
            velY = val;
        } else if(axis == Axis.Z) {
            velZ = val;
        }
    }

    public void AddToAxis(Axis axis, int val) {
        if(axis == Axis.X) {
            x += val;
        } else if(axis == Axis.Y) {
            y += val;
        } else if(axis == Axis.Z) {
            z += val;
        }
    }

    public void AddToVelAxis(Axis axis, int val) {
        if(axis == Axis.X) {
            velX += val;
        } else if(axis == Axis.Y) {
            velY += val;
        } else if(axis == Axis.Z) {
            velZ += val;
        }
    }

}

public class SolarSystem {

    private List<Planet> planets;
    private List<Planet> planetsOrigin;

    public SolarSystem () {
        planets = new List<Planet>();
        planetsOrigin = new List<Planet>();
    }

    // applies gravity for one timestep
    private void ApplyGravity () {
        // iterate through the set of unique anti-reflexive pairs.
        foreach(Planet p1 in planets) {
            foreach(Planet p2 in planets) {
                if(p1 != p2) {
                    // calculate gravity pull
                    if (p1.x != p2.x) p1.velX += (p1.x > p2.x) ? -1 : 1;
                    if (p1.y != p2.y) p1.velY += (p1.y > p2.y) ? -1 : 1;
                    if (p1.z != p2.z) p1.velZ += (p1.z > p2.z) ? -1 : 1;
                }
            }
        }
    }

     // applies velocity for one timestep
    private void ApplyVelocity () {
        foreach(Planet p in planets) {
            p.x += p.velX;
            p.y += p.velY;
            p.z += p.velZ;
        }
    }

    // applies gravity for one timestep
    private void ApplyGravityOnAxis (Axis axis) {
        // iterate through the set of unique anti-reflexive pairs.
        foreach(Planet p1 in planets) {
            foreach(Planet p2 in planets) {
                if(p1 != p2) {
                    // calculate gravity pull
                    if (p1.GetAxis(axis) != p2.GetAxis(axis)) 
                        p1.AddToVelAxis(axis, (p1.GetAxis(axis) > p2.GetAxis(axis)) ? -1 : 1);
                }
            }
        }
    }

    // applies velocity for one timestep
    private void ApplyVelocityOnAxis (Axis axis) {
        foreach(Planet p in planets) {
            p.AddToAxis(axis, p.GetVelAxis(axis));
        }
    }

    private string CurrentAxisState(Axis axis) {
        string strOut = "";
        for(int i = 0; i < planets.Count; i++)
            strOut += planets[i].GetAxis(axis).ToString() + " " + planets[i].GetVelAxis(axis).ToString() + " ";
        
        return strOut;
    }

    // resets planets list 
    public void ResetPlanets() {
        planets.Clear();
        foreach(Planet p in planetsOrigin) {
            planets.Add(p);
        }
    }

    public void LoadFromFile(string filename) {
        foreach(string line in File.ReadAllLines(filename)) {
            // disgusting code right here
            int x = int.Parse( line.Substring(3, Alg.FindNth(line, ',', 0) - 3) );
            int y = int.Parse( line.Substring(Alg.FindNth(line, ',', 0) + 4, Alg.FindNth(line, ',', 1) - (Alg.FindNth(line, ',', 0) + 4)) );
            int z = int.Parse( line.Substring(Alg.FindNth(line, ',', 1) + 4, (line.Length - 1) - (Alg.FindNth(line, ',', 1) + 4)) );
            
            // debug: //Console.WriteLine(": {0} {1} {2}", x, y, z);

            planets.Add( new Planet(x, y, z) );
            planetsOrigin.Add( new Planet(x, y, z) );
        }
    }

    public void SimulateTimesteps(int n) {
        for(int i = 0; i < n; i++){
            ApplyGravity();
            ApplyVelocity();
        }
    }

    // get the total calculated energy of the system
    public int GetTotalEnergy() {
        int sum = 0;
        foreach(Planet p in planets) {
            Func<int, int, int, int> AbsSum = (x, y, z) => (Math.Abs(x) + Math.Abs(y) + Math.Abs(z));
            sum += AbsSum(p.x, p.y, p.z) * AbsSum(p.velX, p.velY, p.velZ);
        }

        return sum;
    }

/*
    private bool RepeatedStateX() {
        for(int i = 0; i < planets.Count; i++) {
            if(planets[i].x != planetsOrigin[i].x || planets[i].velX != planetsOrigin[i].velX)
                return false;
        }

        // else, return true.    
        return true;
    }
*/

    public int StepsUntilAxisRepeats(Axis axis) {
        Hashtable stateHash = new Hashtable();

        int counter = 0;

        Console.WriteLine("----------------------------");
        Console.WriteLine(CurrentAxisState(axis));
        Console.WriteLine(counter);
        
        while(!stateHash.Contains( CurrentAxisState(axis) )) {
            stateHash.Add(CurrentAxisState(axis), counter);
            counter++;
            ApplyGravityOnAxis(axis);
            ApplyVelocityOnAxis(axis);
            //if (RepeatedStateX()) Console.WriteLine("wut: {0}", counter);
        }

        Console.WriteLine(CurrentAxisState(axis));
        Console.WriteLine(stateHash[CurrentAxisState(axis)]);
        Console.WriteLine("----------------------------");

        return counter;
    }

}
