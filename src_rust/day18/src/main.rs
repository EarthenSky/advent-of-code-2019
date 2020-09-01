use std::fs::File;

use priority_queue::PriorityQueue;
use std::cmp::Reverse;

mod util;
mod types;
use types::{CharMatrix, Node, Point2D};

// Notes: 
// Input is assumed to be well formed.

fn main() {
    let guard = pprof::ProfilerGuard::new(100).unwrap();
    part1();
    if let Ok(report) = guard.report().build() {
        let file = File::create("flamegraph.svg").unwrap();
        report.flamegraph(file).unwrap();
        println!("Flamegraph Generated");
    };
}

fn part1() {
    println!("Day 18 pt1 start");

    // Load map as vector.
    let mut map = CharMatrix::from_file("input_test");
    map.print();

    let mut key_positions: Vec<Point2D> = Vec::new();
    // stores all key positions for quick use later.
    for (i, ch) in map.data.iter().enumerate() {
        if ch.is_ascii_lowercase() {
            key_positions.push( Point2D::new(i % map.width, i / map.width) );
        }
    }

    // store all possible nodes
    let mut frontier = PriorityQueue::new();
    frontier.push( Node::make_root(map), 0_isize );

    let mut i = 0;
    while !frontier.is_empty() && i < 10000 {
        let (current, path_len) = frontier.pop().unwrap();
        if i % 100 == 0 {
            //println!("frontier_len = {}", frontier.len());
            //println!("path_len = {}", -path_len);
            //current.print();
        }

        if current.is_goal() {
            // return the first path which is a goal.
            println!("Shortest length is: {}", -path_len);
            break;
        }

        // Use floodfill algorithm to find currently accessible keys.
        for key in current.find_accessible_keys(&key_positions) {
            // Use A* to find distance to keys.
            let mut new_node = current.got_key(key);
            let g_cost = new_node.path_len;

            // Add total g-cost of key_getting_action to frontier, do the lowest 
            // g-cost action each iteration and then generate new subsequent 
            //actions with higher g-costs from that action.
            frontier.push( new_node, -(g_cost as isize) );
        }
        
        i += 1;
    }
    
    println!("Day 18 pt1 done");
}
