package src.util;

// shhh, I'm tired and it works, so dw. all good. yup.

public class PointPairPair {
    public PointPair a, b;
    public PointPairPair (PointPair a, PointPair b) {
        this.a = a;
        this.b = b;
    }
    
    public String toString() {
        return "{" + a.toString() + ", " + b.toString() + "}";
    }
}
