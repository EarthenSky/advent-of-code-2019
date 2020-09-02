use std::collections::{HashSet, HashMap};
type AstarCache = HashMap<(Vec<(char, Point2D)>, Point2D, Point2D), usize>;

use std::fs::File;

use priority_queue::PriorityQueue;

mod util;
mod types;
use types::{CharMatrix, Node, Point2D};

// Notes: 
// Input is assumed to be well formed.

fn main() {
    part1();
}

fn part1_flamegraph() {
    let guard = pprof::ProfilerGuard::new(1000).unwrap();
    part1();
    if let Ok(report) = guard.report().build() {
        let file = File::create("flamegraph.svg").unwrap();
        report.flamegraph(file).unwrap();
        println!("Flamegraph Generated");
    };
}

// I had to run this analysis for 4 mins on my computer unfortunately...
fn part1() {
    println!("Day 18 pt1 start");

    // Load map as vector.
    let mut map = CharMatrix::from_file("input");
    map.print();
    map.propagate_walls(true);
    map.propagate_keys();
    map.print();

    // stores all key & door positions & cleans out the map. Extracts player 
    //position as well
    let mut player_pos = Point2D::new(0, 0);
    let mut key_positions: Vec<(char, Point2D)> = Vec::new();
    let mut door_positions: Vec<(char, Point2D)> = Vec::new();
    for (i, ch) in map.data.clone().iter().enumerate() {
        if *ch == '@' {
            player_pos = Point2D::new(i % map.width, i / map.width);
            map.set(player_pos, '.');
        } else if ch.is_ascii_lowercase() {
            let p = Point2D::new(i % map.width, i / map.width);
            key_positions.push( (*ch, p) );
            map.set(p, '.');
        } else if ch.is_ascii_uppercase() {
            let p = Point2D::new(i % map.width, i / map.width);
            door_positions.push( (*ch, p) );
            map.set(p, '.');
        }
    }

    // to avoid duplicate checking. This works because nodes are accessed in order.
    // TODO: if checked is invalid, store fcost as well and only prune nodes if they're the same & have a 
    // worse or equal f cost. (or maybe just g-cost)
    let mut checked: HashSet<u64> = HashSet::new();
    let mut astar_calls: AstarCache = HashMap::new();
    let mut pruned = 0;

    // store all possible nodes
    let mut frontier = PriorityQueue::new();
    frontier.push( Node::make_root(key_positions, door_positions, player_pos), 0_isize );

    let mut i = 0;
    while !frontier.is_empty() {
        let (current, path_len) = frontier.pop().unwrap();
        checked.insert(current.state_hash());

        if i % 20000 == 0 {
            println!("------------------------- {}", i);
            let g_cost: usize = current.path_len;
            let mut points: Vec<Point2D> = current.keys.iter().map(|(_ch, p)| *p).collect();
            points.push(current.player_pos);
            //let f_cost: usize = util::closest_distance(&points); 
            //let f_cost: usize = util::closest_astar(&mut astar_calls, &map, &current.doors, &points);
            let f_cost: usize = util::closest_astar_greedy(&mut astar_calls, &map, &points);
            println!("g_cost = {}", g_cost);
            println!("f_cost = {}", f_cost);
            println!("path_len = {}", -path_len);
            //println!("g_cost = {}", -path_len - f_cost);
            println!("frontier_len = {}", frontier.len());
            //println!("checked = {}", checked.len());
            println!("astar_calls = {}", astar_calls.len());
            println!("pruned = {}", pruned);
            //current.print();
        }

        if current.is_goal() {
            // return the first path which is a goal.
            println!("Shortest length is: {}", -path_len);
            println!("{}", i);
            break;
        }

        // Use floodfill algorithm to find currently accessible keys.
        for key in current.find_accessible_keys(&map) {
            // Use A* to find distance to keys.
            let new_node = current.got_key(&mut astar_calls, &map, key);

            // Sort tree search using A* like heuristic function (was doing only djikstra...)
            let g_cost: usize = new_node.path_len;
            let mut points: Vec<Point2D> = new_node.keys.iter().map(|(_ch, p)| *p).collect();
            points.push(new_node.player_pos);
            //let f_cost: usize = util::closest_distance(&points); 
            //let f_cost: usize = util::closest_astar(&mut astar_calls, &map, &new_node.doors, &points);
            let f_cost: usize = util::closest_astar_greedy(&mut astar_calls, &map, &points);

            // Add total g-cost of key_getting_action to frontier, do the lowest 
            // g-cost action each iteration and then generate new subsequent 
            //actions with higher g-costs from that action.
            if !checked.contains(&new_node.state_hash()) {
                frontier.push( new_node, -((g_cost + f_cost) as isize) );
            } else {
                pruned += 1;
            }
        }
        
        i += 1;
    }
    
    println!("Day 18 pt1 done");
}
