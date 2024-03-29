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
            this.x = -1;
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
    }
    
    @Override
    public boolean equals(Object obj) {
        return obj != null && ((Point) obj).x == this.x && ((Point) obj).y == this.y;
    }
    
}
