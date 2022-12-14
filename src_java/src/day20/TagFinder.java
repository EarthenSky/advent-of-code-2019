package src.day20;

import src.util.Point;
import src.util.PointPair;
import src.util.PointPairPair;
import src.util.CharMatrix;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

interface Runner {
    void run(char ch, char ch2, int x, int y, Point stand);
}

public class TagFinder {
    public CharMatrix map;
    public Point outerCorner, innerCorner;
    public Point outerSize, innerSize;

    public TagFinder (CharMatrix map) {
        this.map = map;
        findRectAreas();
    }

    // returns first tag found & only checks outer
    // point returned is on the floor.
    public Point findOuterTagPos(char ch, char ch2) {
        int x = outerCorner.x, y = outerCorner.y;
        while(x != (outerSize.x - 1)) {
            char curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                char nextCh = map.getCh(x, y + 1);
                if (curCh == ch && nextCh == ch2)
                    return new Point(x, y + 2);
            }
            x += 1;
        }
        while(y != (outerSize.y - 1)) {
            char curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                char nextCh = map.getCh(x + 1, y);
                if (curCh == ch && nextCh == ch2)
                    return new Point(x - 1, y);
            }
            y += 1;
        }
        while(x != outerCorner.x) {
            char curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                char nextCh = map.getCh(x, y + 1);
                if (curCh == ch && nextCh == ch2)
                    return new Point(x, y - 1);
            }
            x -= 1;
        }
        while(y != outerCorner.y) {
            char curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                char nextCh = map.getCh(x + 1, y);
                if (curCh == ch && nextCh == ch2)
                    return new Point(x + 2, y);
            }
            y -= 1;
        }

        System.out.println("Error, could not find tag.");
        return new Point(-1, -1);
    }

    public HashMap<String, Point> generateMap() {
        // TODO: portalIdMap should be it's own class so I don't have to deal with this hacky syntax below.
        // Buuuut since it already works & I don't have to edit it, maybe I just don't touch it.
        HashMap<String, PointPairPair> portalIdMap = new HashMap<String, PointPairPair>(); // map with default value of Infinity
    
        // function to do check & add on the above variables.
        Runner addToMap = (char ch, char ch2, int xPos, int yPos, Point stand) -> {
            String id = "" + ch + ch2;
            if (!portalIdMap.containsKey(id)) { // case: adding first
                portalIdMap.put(id, new PointPairPair( new PointPair(new Point(xPos, yPos), stand), new PointPair(new Point(-1, -1), new Point(-1, -1)) ));
            } else { // case: adding second
                PointPairPair ppp = portalIdMap.remove(id);
                portalIdMap.put(id, new PointPairPair( new PointPair(ppp.a.a, ppp.a.b), new PointPair(new Point(xPos, yPos), stand) ));
            }
        };
        
        int x = outerCorner.x, y = outerCorner.y;
        char curCh = '\0', nextCh = '\0';

        // outer
        while(x != (outerSize.x - 1)) {
            curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                nextCh = map.getCh(x, y + 1);
                addToMap.run(curCh, nextCh, x, y + 1, new Point(x, y + 2));
            }
            x += 1;
        }
        while(y != (outerSize.y - 1)) {
            curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                nextCh = map.getCh(x + 1, y);
                addToMap.run(curCh, nextCh, x, y, new Point(x - 1, y));
            }
            y += 1;
        }
        while(x != outerCorner.x) {
            curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                nextCh = map.getCh(x, y + 1);
                addToMap.run(curCh, nextCh, x, y, new Point(x, y - 1));
            }
            x -= 1;
        }
        while(y != outerCorner.y) {
            curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                nextCh = map.getCh(x + 1, y);
                addToMap.run(curCh, nextCh, x + 1, y, new Point(x + 2, y));
            }
            y -= 1;
        }

        System.out.println(innerSize.toString());

        // inner
        x = innerCorner.x;
        y = innerCorner.y;
        while(x != (innerSize.x - 1)) {
            curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                nextCh = map.getCh(x, y + 1);
                addToMap.run(curCh, nextCh, x, y, new Point(x, y - 1));
            }
            x += 1;
        }
        while(y != (innerSize.y - 1)) {
            curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                nextCh = map.getCh(x + 1, y);
                addToMap.run(curCh, nextCh, x + 1, y, new Point(x + 2, y));
            }
            y += 1;
        }
        while(x != innerCorner.x) {
            curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                nextCh = map.getCh(x, y + 1);
                addToMap.run(curCh, nextCh, x, y + 1, new Point(x, y + 2));
            }
            x -= 1;
        }
        while(y != innerCorner.y) {
            curCh = map.getCh(x, y);
            if (Character.isUpperCase(curCh)) {
                nextCh = map.getCh(x + 1, y);
                addToMap.run(curCh, nextCh, x, y, new Point(x - 1, y));
            }
            y -= 1;
        }

        System.out.println(portalIdMap.toString());
        
        HashMap<String, Point> truePortalIdMap = new HashMap<String, Point>();
        for (String key : portalIdMap.keySet()) {
            PointPairPair ppp = portalIdMap.get(key);
            if (ppp.b.a.equals(new Point(-1, -1))) { // case: no second pointpair
                continue;
            } else {
                String newKey = key + ppp.a.a.x + "," + ppp.a.a.y;
                truePortalIdMap.put(newKey, ppp.b.b);
            }
        }

        // check second direction
        for (String key : portalIdMap.keySet()) {
            PointPairPair ppp = portalIdMap.get(key);
            if (ppp.b.a.equals(new Point(-1, -1))) { // case: no second pointpair
                continue;
            } else { // first
                String newKey = key + ppp.b.a.x + "," + ppp.b.a.y;
                truePortalIdMap.put(newKey, ppp.a.b);
            }
        }

        return truePortalIdMap;
    }

    // ********************************************************************* //

    // Assumption: ring size (i.e word size) is 2.
    private void findRectAreas() {
        // moves diagonally to find inner donut hole
        int x = 2, y = 2;
        while (map.getCh(x, y) == '#' || map.getCh(x, y) == '.') {
            x += 1;
            y += 1;
        }

        outerCorner = new Point(0, 0);
        innerCorner = new Point(x, y);

        // moves to far corner of donut.
        while (map.getCh(x, y) != '#' && map.getCh(x, y) != '.') {
            x += 1;
        }
        x -= 1;
        while (map.getCh(x, y) != '#' && map.getCh(x, y) != '.') {
            y += 1;
        }
        y -= 1;

        innerSize = new Point(x, y);
        outerSize = new Point(map.width - 1, (map.data.length / map.width) - 1);
    }

}
