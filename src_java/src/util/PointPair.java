package src.util;

public class PointPair {
    public Point a, b;
    public PointPair (Point a, Point b) {
        this.a = a;
        this.b = b;
    }
    
    public String toString() {
        return "{" + a.toString() + ", " + b.toString() + "}";
    }
}
