package src.util;

import java.util.Objects;

public class Point {
    public int x, y;
    public Point (int x, int y) {
        this.x = x;
        this.y = y;
    }

    public Point (String s) {
        if (s == "Down") {
            this.x = 0;
            this.y = 1;
        } else if (s == "Up") {
            this.x = 0;
            this.y = -1;
        } else if (s == "Left") {
            this.x = -1;  // TODO: double check if this is right.
            this.y = 0;
        } else if (s == "Right") {
            this.x = 1;
            this.y = 0;
        }
    }

    public Point plus(Point p) {
        return new Point(x + p.x, y + p.y);
    }

    public Point inverse() {
        return new Point(-this.x, -this.y);
    }

    public String toString() {
        return "(" + x + ", " + y + ")";
    }
    
    public boolean equals(Point p) {
        return p != null && p.x == this.x && p.y == this.y;
    }

    @Override
    public int hashCode() {
        return Objects.hash(x, y);
        /*
        int hash = 17;
        hash = hash * 31 + x;
        hash = hash * 31 + y;
        return hash;*/
    }
    
    /*
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        else if (obj == null || getClass() != obj.getClass())
            return false;
        Point other = (Point) obj;
        return !(this.x != other.x || this.y != other.y);
    }
    */

    @Override
    public boolean equals(Object obj) {
        return obj != null && ((Point) obj).x == this.x && ((Point) obj).y == this.y;
    }
    
}
