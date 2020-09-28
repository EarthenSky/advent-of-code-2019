package src.day20;

import src.util.Point;
import src.util.PointPair;
import src.util.CharMatrix;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.PriorityQueue;


public class PortalPathfinder {
    private CharMatrix map;
    private Point start;
    private Point goal;

    private HashMap<String, Point> portalIdMap;

    public PortalPathfinder (CharMatrix map) {
        this.map = map;

        TagFinder tf = new TagFinder(map);
        this.start = tf.findOuterTagPos('A', 'A');
        this.goal = tf.findOuterTagPos('Z', 'Z');
        this.portalIdMap = tf.generateMap();
        System.out.println(portalIdMap.toString());
    }

    public int getPathLen() {
        return a_star(this.map, this.start, this.goal).size();
    }

    // ********************************************************************* //

    private Point getTeleportedLocation(char ch, char ch2, Point from) {
        String s = "" + ch + ch2 + from.x + "," + from.y; // true key
        if (this.portalIdMap.containsKey(s)) {
            return this.portalIdMap.get(s);
        } else {
            return new Point(-1, -1); // the null point.
        }
    }

    // not the best with portals.
    private int manhattanDistance(Point p1, Point p2) {
        int val = 0;
        if (p1.x < p2.x) {
            val += p2.x - p1.x;
        } else {
            val += p1.x - p2.x;
        }
        if (p1.y < p2.y) {
            val += p2.y - p1.y;
        } else {
            val += p1.y - p2.y;
        }
        return val;
    }

    private ArrayList<Point> reconstructPath(HashMap<Point, Point> cameFrom, Point goal) {
        // this with-capacity is just a random-sucky estimate.
        ArrayList<Point> totalPath = new ArrayList<Point>(cameFrom.size() / 4);
        Point current = goal;
        while(cameFrom.containsKey(current)) {
            Point lastPoint = cameFrom.get(current);
            totalPath.add(lastPoint);
            current = lastPoint;
        }
        Collections.reverse(totalPath);
        return totalPath;
    }

    // A* finds a path from start to goal.
    // h is the heuristic function. h(n) estimates the cost to reach goal from node n.
    ArrayList<Point> a_star(CharMatrix map, Point start, Point goal) {
        // For node n, cameFrom[n] is the node immediately preceding it on the cheapest path from start
        // to n currently known.
        HashMap<Point, Point> cameFrom = new HashMap<Point, Point>();

        // For node n, gScore[n] is the cost of the cheapest path from start to n currently known.
        HashMap<Point, Integer> gScore = new HashMap<Point, Integer>(); // map with default value of Infinity
        gScore.put(start, 0);
        
        // The set of discovered nodes that may need to be (re-)expanded.
        // Initially, only the start node is known.
        // This is usually implemented as a min-heap or priority queue rather than a hash-set.
        Comparator<Point> cmp = new Comparator<Point>() {
            @Override
            public int compare(Point p1, Point p2) {
                int p1f = (gScore.get(p1)); //+ manhattanDistance(p1, goal));
                int p2f = (gScore.get(p2)); //+ manhattanDistance(p2, goal));
                return p1f - p2f;
            }
        };
        
        // TODO: make sure this is correct
        PriorityQueue<Point> openQueue = new PriorityQueue<Point>(cmp);
        openQueue.add(start);

        while ( !openQueue.isEmpty() ) {
            //System.out.println(openQueue.toString());
            Point current = openQueue.remove();
            //System.out.println(current.toString());
            if(current.equals(goal)) {
                return reconstructPath(cameFrom, goal);
            }

            Point neighbors[] = { new Point("Up"), new Point("Down"), new Point("Left"), new Point("Right") };
            for (Point actionPoint : neighbors) {
                Point neighborPoint = current.plus(actionPoint);

                // prune nodes which are not walkable. -> player nodes are considered walkable.
                char ch = map.getCh(neighborPoint);
                if (ch == '#') {
                    continue;
                }

                Point resultPoint;
                if (Character.isUpperCase(ch)) {
                    char ch2 = map.getCh(neighborPoint.plus(actionPoint));
                    if (actionPoint.equals(new Point("Down")) || actionPoint.equals(new Point("Right"))) {
                        resultPoint = this.getTeleportedLocation(ch, ch2, neighborPoint);
                    } else { // case: reverse order.
                        resultPoint = this.getTeleportedLocation(ch2, ch, neighborPoint);
                    }
                    if ( resultPoint.equals(new Point(-1, -1)) ) {
                        continue;
                    }
                } else {
                    resultPoint = neighborPoint;
                }

                // gTentative is the distance from start to the neighbor through current
                int gTentative = gScore.get(current) + 1;
                if (gScore.containsKey(resultPoint) && gTentative >= gScore.get(resultPoint)) {
                    continue;
                }

                // This path to neighbor is better than any previous one. (or none exist) Record it!
                cameFrom.put(resultPoint, current);
                gScore.put(resultPoint, gTentative); // this automagically updates the cost function.
                if (openQueue.contains(resultPoint) == false) {
                    openQueue.add( resultPoint );
                }
            }
        }

        // means that no solution was found.
        return new ArrayList<Point>(); 
    }
}
