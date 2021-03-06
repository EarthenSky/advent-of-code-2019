use std::collections::{HashSet, HashMap};
type AstarCache = HashMap<(Vec<(char, Point2D)>, Point2D, Point2D), usize>;

use std::fs::File;

use priority_queue::PriorityQueue;

mod util;
mod types;
use types::{CharMatrix, Node, Point2D, QuadNode};

// Notes: 
// Input is assumed to be well formed.

fn main() {
    //part1();
    part2();
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
            println!("frontier_len = {}", frontier.len());
            println!("astar_calls = {}", astar_calls.len());
            println!("pruned = {}", pruned);
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

fn part2() {
    println!("Day 18 pt2 start");

    // Load map as vector.
    let mut map = CharMatrix::from_file("input_2");
    map.print();
    map.propagate_walls(true);
    map.print();

    // stores all key & door positions & cleans out the map. Extracts player 
    // positions as well
    let mut player_positions: [Point2D; 4] = [Point2D::new(0, 0); 4];
    let mut index = 0;
    let mut key_quads: [Vec<(char, Point2D)>; 4] = [Vec::new(), Vec::new(), Vec::new(), Vec::new()];
    let mut door_quads: [Vec<(char, Point2D)>; 4] = [Vec::new(), Vec::new(), Vec::new(), Vec::new()];
    //let mut key_positions: Vec<(char, Point2D)> = Vec::new();
    //let mut door_positions: Vec<(char, Point2D)> = Vec::new();
    for (i, ch) in map.data.clone().iter().enumerate() {
        if i % map.width >= map.width / 2 && i >= map.data.len() / 2 {
            index = 0;
        } else if i % map.width < map.width / 2 && i >= map.data.len() / 2 {
            index = 1;
        } else if i % map.width >= map.width / 2 && i < map.data.len() / 2 {
            index = 2;
        } else if i % map.width < map.width / 2 && i < map.data.len() / 2 {
            index = 3;
        }

        if *ch == '@' {
            player_positions[index] = Point2D::new(i % map.width, i / map.width);
            map.set(player_positions[index], '.');
            index += 1;
        } else if ch.is_ascii_lowercase() {
            let p = Point2D::new(i % map.width, i / map.width);
            key_quads[index].push( (*ch, p) );
            map.set(p, '.');
        } else if ch.is_ascii_uppercase() {
            let p = Point2D::new(i % map.width, i / map.width);
            door_quads[index].push( (*ch, p) );
            map.set(p, '.');
        }
    }

    // to avoid duplicate checking. This works because nodes are accessed in order.
    let mut checked: HashSet<u64> = HashSet::new();
    let mut astar_calls: AstarCache = HashMap::new();
    let mut pruned = 0;

    // store all possible nodes
    let mut frontier = PriorityQueue::new();
    frontier.push( QuadNode::make_root(key_quads, door_quads, player_positions), 0_isize );

    let mut i = 0;
    while !frontier.is_empty() {
        let (current, path_len) = frontier.pop().unwrap();
        checked.insert(current.state_hash());

        if i % 5000 == 0 {
            println!("------------------------- {}", i);
            let g_cost: usize = current.path_len;
            let quad_points: Vec<Vec<Point2D>> = current.key_quads.iter().map(|vec| 
                vec.iter().map(|(_ch, p)| *p).collect::<Vec<Point2D>>()
            ).collect();
            let f_cost: usize = util::quad_closest_astar_greedy(&mut astar_calls, &map, quad_points, current.player_positions);
            println!("g_cost = {}", g_cost);
            println!("f_cost = {}", f_cost);
            println!("path_len = {}", -path_len);
            println!("frontier_len = {}", frontier.len());
            println!("astar_calls = {}", astar_calls.len());
            println!("pruned = {}", pruned);
        }

        if current.is_goal() {
            // return the first path which is a goal.
            println!("Shortest length is: {}", -path_len);
            println!("{}", i);
            break;
        }

        // Use floodfill algorithm to find currently accessible keys.
        for (key, index) in current.find_accessible_keys(&map) {
            // Use A* to find distance to keys.
            let new_node = current.got_key(&mut astar_calls, &map, key, index);

            // Sort tree search using A* heuristic function
            let g_cost: usize = new_node.path_len;
            let quad_points: Vec<Vec<Point2D>> = new_node.key_quads.iter().map(|vec| 
                vec.iter().map(|(_ch, p)| *p).collect::<Vec<Point2D>>()
            ).collect();
            let f_cost: usize = util::quad_closest_astar_greedy(&mut astar_calls, &map, quad_points, new_node.player_positions);

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

    println!("Day 18 pt2 done");
}
