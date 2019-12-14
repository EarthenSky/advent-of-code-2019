#include <iostream>
#include <sstream>
#include <fstream>
//#include <cmath>
#include <cstdlib>

#include <vector>
#include <string>

#include "day3.h"


namespace day3 {
    struct Point2D {
        int x, y;
        Point2D(int x, int y) : x(x), y(y) {}
        Point2D(Point2D &p) { x = p.x; y = p.y; }
    };

    struct LineSeg {
        Point2D start;
        Point2D end;
        bool isH;  // is Horizontal
        LineSeg(Point2D start, Point2D end, bool isH) 
            : start(start), end(end), isH(isH) {}
        LineSeg(int x1, int y1, int x2, int y2, bool isH) 
            : start(x1, y1), end(x2, y2), isH(isH) {} 
    };

    void part1();
    void part2();

    void day3() {
        part1();
        //part2();
        std::cout << "/* ending day3 */\n";
    }

    // checks if a horizontal and vertical line intersect.
    bool intersects(LineSeg &v, LineSeg &h) {
        int smallerX = h.start.x > h.end.x ? h.end.x : h.start.x; 
        int largerX = h.start.x > h.end.x ? h.start.x : h.end.x; 
        int smallerY = v.start.y > v.end.y ? v.end.y : v.start.y;
        int largerY =  v.start.y > v.end.y ? v.start.y : v.end.y;

        std::cout << "(" << v.start.x << ", " << v.start.y << "),";
        std::cout << "(" << v.end.x << ", " << v.end.y << "),\t";
        std::cout << "(" << h.start.x << ", " << h.start.y << "),";
        std::cout << "(" << h.end.x << ", " << h.end.y << ")\n";

        if (v.start.x >= smallerX && v.start.x <= largerX &&
            h.start.y >= smallerY && h.start.y <= largerY) {
            return true;
        } else {
            return false;
        } 
    }
    
    // read info from file
    void readWiresFromLine(std::vector<LineSeg*> &wire, std::ifstream &fileStream, int &x, int &y) {
        std::string line;
        std::getline(fileStream, line);
        std::stringstream lineStream(line);

        std::string s;
        while (std::getline(lineStream, s, ',')) {
            char direction = s[0];
            int xmod = 0, ymod = 0;
            bool isH = false;
            int delta = std::stoi(s.substr(1, s.length() - 1));

            // figure out direction of movement.
            switch(direction) {
                case 'U': ymod = delta; isH = false; break; 
                case 'D': ymod = -delta; isH = false; break;
                case 'L': xmod = -delta; isH = true; break;
                case 'R': xmod = delta; isH = true; break;
                default: std::cout << "aaaaaah -- not good!\n\n\n";
            }

            // remember to free this after!
            LineSeg* tmp = new LineSeg(x, y, x + xmod, y + ymod, isH);
            wire.push_back(tmp);
            
            x += xmod;
            y += ymod;
            //std::cout << "(" << x << ", " << y << ")" << ",";
        }
    }

    void part1() {
        std::cout << "/* starting day3 part 1 */\n";

        // init wires
        std::vector<LineSeg*> wire1;
        std::vector<LineSeg*> wire2;
        std::vector<Point2D*> crosses;

        // get file info as lines.
        std::ifstream file("resc/day3a.txt");
        int x = 0, y = 0;
        readWiresFromLine(wire1, file, x, y);
        x = 0; y = 0;
        readWiresFromLine(wire2, file, x, y);
        
        // construct array of line segments from wires
        // compare all linesegments that intersect
        for(LineSeg* line1 : wire1) {
            for(LineSeg* line2 : wire2) {
                if (line1->isH != line2->isH) {
                    LineSeg* h = line1->isH ? line1 : line2;
                    LineSeg* v = line1->isH ? line2 : line1;
                    if(intersects(*v, *h)) {
                        crosses.push_back( new Point2D(h->start.y, v->end.x) );
                        std::cout << "gotem\n";
                    }
                }
            }
        }

        std::cout << "\npause\n";

        // take min distance of intersect point.
        int min = 1000000;
        for(Point2D* p : crosses) {
            int dis = std::abs(p->x) + std::abs(p->y);
            std::cout << "(" << p->x << ", " << p->y << ")" << ",";
            if(dis < min && dis > 0) {
                min = dis;
                std::cout << "smallest update: " << min << '\n';
            }
        }

        std::cout << "smallest: " << min << '\n';

        // clean up
        for(Point2D* p : crosses) { delete p; }
        for(LineSeg* l : wire1) { delete l; }
        for(LineSeg* l : wire2) { delete l; }
    }


    void part2() {
        std::cout << "/* starting day3 part 2 */\n";

    }
}