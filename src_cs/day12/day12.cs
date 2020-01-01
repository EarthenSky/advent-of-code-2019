using System;
using System.Collections.Generic;  // for lists
using System.IO;  // for files
//using System.Text;  // for StringBuilder

public class Program 
{
    public static void Main(string[] args) {
        SolarSystem sys = new SolarSystem();
        sys.LoadFromFile("input");
                
        sys.SimulateTimesteps(1000);

        int energy = sys.GetTotalEnergy();
        Console.WriteLine( "energy = {0}", energy);
    } 
}



/*
;; lisp comments:

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

public class Alg {
    // returns index of nth instance of supplied char.@@@
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
}

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

}

public class SolarSystem {

    List<Planet> planets;

    public SolarSystem () {
        planets = new List<Planet>();
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

    public void LoadFromFile(string filename) {
        foreach(string line in File.ReadAllLines(filename)) {
            // disgusting code right here
            int x = int.Parse( line.Substring(3, Alg.FindNth(line, ',', 0) - 3) );
            int y = int.Parse( line.Substring(Alg.FindNth(line, ',', 0) + 4, Alg.FindNth(line, ',', 1) - (Alg.FindNth(line, ',', 0) + 4)) );
            int z = int.Parse( line.Substring(Alg.FindNth(line, ',', 1) + 4, (line.Length - 1) - (Alg.FindNth(line, ',', 1) + 4)) );
            
            // debug: //Console.WriteLine(": {0} {1} {2}", x, y, z);

            planets.Add( new Planet(x, y, z) );
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

}