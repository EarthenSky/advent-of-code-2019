package src.day20;

import src.util.CharMatrix;

public class Run {
    public static void main (String[] args) {
        part1();
    }

    public static void part1() {
        System.out.println("Starting day20 pt 1");

        CharMatrix map = new CharMatrix();
        map.fromFile("src_java/input_d20.txt");
        map.print();

        map.optimize();
        map.print();

        PortalPathfinder pp = new PortalPathfinder(map);
        System.out.println("Path len = " + pp.getPathLen());
        
        System.out.println("Done");
    }
}