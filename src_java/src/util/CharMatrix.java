package src.util;

import src.util.Point;

import java.nio.file.Paths;
import java.nio.file.Files;
import java.nio.charset.StandardCharsets;
import java.io.IOException;

public class CharMatrix {
    public int width;
    public char[] data;

    // empty constructor allows for load from file interface.
    public CharMatrix() { }

    // removes \n and 
    public CharMatrix (String data) {
        this.loadString(data);
    }

    public void fromFile(String path) {
        try {
            String fileString = Files.readString(
                Paths.get(System.getProperty("user.dir") + "\\" + path), 
                StandardCharsets.US_ASCII);
            this.loadString(fileString);
        } catch (IOException e) {
            System.out.println("File unable to be read -> This is your fault programmer.");
            e.printStackTrace();
        }
    }

    // initializes data structure with information.
    private void loadString(String data) {
        data = data.replace("\r", "");
        this.width = data.indexOf('\n');

        String osCleanData = data.replace("\n", "");
        this.data = osCleanData.toCharArray();
    }

    public void optimize() {
        // This function turns spaces which never need to be accessed (don't lead to finish)
        // into walls so that A* & walkable space calls are faster.
        // I didn't optimize this function because it is a pre-processing step.
        boolean check_again = true;
        while (check_again) {
            check_again = false;
            int i = 0;
            for (char ch : this.data) {
                int x = i % this.width, y = i / this.width;
                if (!this.isEdge(x, y) && ch == '.') {
                    int adj[] = { x+1, y, x, y+1, x-1, y, x, y-1 };
                    int wall_count = 0, portal_count = 0;
                    for (int j = 0; j < adj.length; j += 2) {
                        char c = this.getCh(adj[j], adj[j + 1]);
                        if (c == '#') {
                            wall_count += 1;
                        } else if (Character.isUpperCase(c)) {
                            portal_count += 1;
                        }
                    }

                    // case: this position is a boring ass corner. Also, corners can make more corners.
                    if (wall_count >= 3 && portal_count == 0) { 
                        this.setCh(x, y, '#');
                        check_again = true;
                    } else if(wall_count == 3 && portal_count == 1) {
                        // TODO: optimize, by removing portal & filling in.
                    }
                }

                i += 1;
            }
        }
    }

    public char getCh(int x, int y) {
        return this.data[y * width + x];
    }
    public char getCh(Point p) {
        return this.data[p.y * width + p.x];
    }

    public void setCh(int x, int y, char ch) {
        this.data[y * width + x] = ch;
    }
    public void setCh(Point p, char ch) {
        this.data[p.y * width + p.x] = ch;
    }

    private boolean isEdge(int x, int y) {
        return x == 0 || x == this.width || y == 0 || y == this.data.length / this.width;
    }

    public void print() {
        int i = 1;
        for (char ch : this.data) {
            System.out.print(ch);
            if (i % this.width == 0) 
                System.out.print("\n");
            i += 1;
        }
    }
}