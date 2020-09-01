use std::collections::HashMap;
use priority_queue::PriorityQueue;
use std::cmp::Reverse;

use crate::types::{Action, CharMatrix, Point2D, BitMatrix};

// This module is for utility algorithms like floodfill, manhattan_dis, and a*.

// ************************************************************************** //

// This is just a different kind of interface to the same recursive alg.
pub fn get_walkable_spaces(map: &CharMatrix, current: Point2D) -> BitMatrix {
    let mut walk_map = BitMatrix::new(map.width, map.data.len());
    walk_map.set(current, true);
    find_walkable_spaces(map, current, &mut walk_map);
    walk_map
}

// effectively a recursive floodfill algorithm.
pub fn find_walkable_spaces(map: &CharMatrix, current: Point2D, walk_map: &mut BitMatrix) {
    let adjacent: Vec<Point2D> = vec![ 
        current.from(Action::Left),
        current.from(Action::Right),
        current.from(Action::Up),
        current.from(Action::Down),
    ];
    for point in adjacent {
        if walk_map.get(point).unwrap() == false {
            match map.get(point) {
                '.' => {
                    walk_map.set(point, true);
                    find_walkable_spaces(map, point, walk_map);
                },
                '#' => walk_map.set(point, false),
                c => {
                    if c.is_ascii_lowercase() {
                        walk_map.set(point, true);
                        find_walkable_spaces(map, point, walk_map);
                    }
                },
            }
        }
    }
}

// ************************************************************************** //

pub fn manhattan_distance(p1: Point2D, p2: Point2D) -> usize {
    let mut val: usize = 0;
    if p1.x < p2.x {
        val += p2.x - p1.x;
    } else {
        val += p1.x - p2.x;
    }
    if p1.y < p2.y {
        val += p2.y - p1.y;
    } else {
        val += p1.y - p2.y;
    }
    val
}

// ************************************************************************** //

// the heuristic function used is simple manhattan distance
pub fn astar_pathfind(map: &CharMatrix, start_point: Point2D, goal_point: Point2D) -> Vec<Action> {
    let mut path = a_star(map, start_point, goal_point, manhattan_distance);
    path
}

fn reconstruct_path(came_from: &HashMap<Point2D, Action>, goal: Point2D) -> Vec<Action> {
    // this with-capacity is just a random-sucky estimate.
    let mut total_path: Vec<Action> = Vec::with_capacity(came_from.len()/4);
    let mut current = goal;
    while came_from.contains_key(&current) {
        let action = came_from[&current];
        total_path.push(action);
        current = current.from(action.inverse());
    }
    total_path.reverse();
    total_path
}

// A* finds a path from start to goal.
// h is the heuristic function. h(n) estimates the cost to reach goal from node n.
fn a_star(map: &CharMatrix, start: Point2D, goal: Point2D, h: fn(Point2D, Point2D) -> usize) -> Vec<Action> {
    // The set of discovered nodes that may need to be (re-)expanded.
    // Initially, only the start node is known.
    // This is usually implemented as a min-heap or priority queue rather than a hash-set.
    let mut open_queue = PriorityQueue::new();
    open_queue.push( start, Reverse( 0 + h(start, goal) ) );

    // For node n, came_from[n] is the node immediately preceding it on the cheapest path from start
    // to n currently known.
    let mut came_from: HashMap<Point2D, Action> = HashMap::new();

    // For node n, g_score[n] is the cost of the cheapest path from start to n currently known.
    let mut g_score: HashMap<Point2D, usize> = HashMap::new(); // map with default value of Infinity
    g_score.insert(start, 0);

    while !open_queue.is_empty() {
        let (current, _priority) = open_queue.pop().unwrap();
        if current == goal {
            return reconstruct_path(&came_from, goal);
        }

        let neighbors = vec![
            Action::Up, Action::Down, Action::Left, Action::Right
        ];
        for action in neighbors {
            let neighbor_point = current.from(action);

            // prune nodes which are not walkable. -> player nodes are considered walkable.
            match map.get(neighbor_point) {
                '#' => continue,
                '.' => (),
                c => {
                    if c.is_ascii_uppercase() {
                        continue;  // is wall
                    }
                },
            }

            // tentative_g_score is the distance from start to the neighbor through current
            let tentative_g_score = g_score[&current] + 1;
            if let Some(g) = g_score.get(&neighbor_point) {
                if tentative_g_score >= *g {
                    continue;
                }
            }

            // This path to neighbor is better than any previous one. (or none exist) Record it!
            came_from.insert(neighbor_point, action);
            g_score.insert(neighbor_point, tentative_g_score);
            let f_score = g_score.get(&neighbor_point).unwrap() + h(neighbor_point, goal);
            if open_queue.change_priority( &neighbor_point, Reverse(f_score) ).is_none() {
                open_queue.push( neighbor_point, Reverse(f_score) );
            }
        }
    }
    // We should never get here because of the validity of the flood fill algorithm
    return Vec::new(); 
}

// ************************************************************************** //
