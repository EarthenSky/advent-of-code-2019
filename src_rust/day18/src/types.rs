use std::collections::hash_map::DefaultHasher;
use std::collections::HashMap;
type AstarCache = HashMap<(Vec<(char, Point2D)>, Point2D, Point2D), usize>;

use std::fs;
use std::process;
use std::hash::{Hash, Hasher};

use bit_vec::BitVec;

use crate::util;

#[derive(Clone)]  
pub struct Node {
    pub keys: Vec<(char, Point2D)>,
    pub doors: Vec<(char, Point2D)>,
    pub player_pos: Point2D,
    pub path_len: usize,
}
impl Node {
    pub fn make_root(keys: Vec<(char, Point2D)>, doors: Vec<(char, Point2D)>, player_pos: Point2D) -> Node {
        Node {
            keys, 
            doors,
            player_pos,
            path_len: 0,
        }
    }

    // is goal if there are no more keys & doors. keys -> doors.
    pub fn is_goal(&self) -> bool {
        self.keys.len() == 0
    }

    // implements the floodfill alg in the background.
    pub fn find_accessible_keys(&self, map: &CharMatrix) -> Vec<char> {
        let mut key_vec: Vec<char> = Vec::new();
        let walkable_spaces = util::get_walkable_spaces(&map, &self.doors, self.player_pos);
        
        // check from key positions
        for (ch, p) in &self.keys {
            if walkable_spaces.get(*p).unwrap() == true {
                key_vec.push( *ch );
            }
        }

        //println!("key_vec len: {}", key_vec.len());

        key_vec
    }

    // returns a copy of the current node, but with a map that has a new player 
    // location & with the key & door cooresponnding to c removed.
    // This function should also use A* to update the path length it took to get to this position.
    pub fn got_key(&self, astar_calls: &mut AstarCache, map: &CharMatrix, key_ch: char) -> Node {
        let mut player_pos = Point2D::new(0, 0);
        
        // make copies of keys and doors.
        let mut keys = Vec::new();
        let mut doors = Vec::new();
        for (ch, p) in &self.keys {
            if *ch != key_ch {
                keys.push( (*ch, *p) );
            } else {
                player_pos = *p;
            }
        }
        for (ch, p) in &self.doors {
            if *ch != key_ch.to_ascii_uppercase() {
                doors.push( (*ch, *p) );
            }
        }
        
        // Do A* from self.player_pos -> player_pos (using newly updated information)
        // Also memoise same calls.
        let move_len = match astar_calls.get( &(doors.clone(), self.player_pos, player_pos) ) {
            Some(val) => *val,
            None => {
                let val = util::astar_pathfind(&map, &doors, self.player_pos, player_pos).len();
                astar_calls.insert((doors.clone(), self.player_pos, player_pos), val);
                val
            },
        };

        //println!("keys: {}", keys.len());

        Node {
            keys,
            doors,
            player_pos,
            path_len: self.path_len + move_len,
        }
    }

    // This function computes a hash which disregards the length of the path to the node.
    // Now, nodes which are hash identical, but have a longer or equal path length can safely be pruned because
    // the shorter nodes are ultimately better. (or exactly the same if equal)
    pub fn state_hash(&self) -> u64 {
        let mut s = DefaultHasher::new();
        self.keys.hash(&mut s);
        self.player_pos.hash(&mut s);
        s.finish()
    }
}
impl Hash for Node {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.keys.hash(state);
        self.player_pos.hash(state);
        self.path_len.hash(state);
    }
}
impl PartialEq for Node {
    fn eq(&self, other: &Self) -> bool {
        self.keys == other.keys && self.player_pos == other.player_pos && self.path_len == other.path_len
    }
}
impl Eq for Node {}

#[derive(Clone)]  
pub struct QuadNode {
    pub key_quads: [Vec<(char, Point2D)>; 4],
    pub door_quads: [Vec<(char, Point2D)>; 4],
    pub player_positions: [Point2D; 4],
    pub path_len: usize,
}
impl QuadNode {
    pub fn make_root(key_quads: [Vec<(char, Point2D)>; 4], 
                     door_quads: [Vec<(char, Point2D)>; 4], 
                     player_positions: [Point2D; 4]) -> QuadNode {
        QuadNode {
            key_quads, 
            door_quads,
            player_positions,
            path_len: 0,
        }
    }

    // is goal if there are no more keys.
    pub fn is_goal(&self) -> bool {
        self.key_quads[0].len() == 0 && self.key_quads[1].len() == 0 && 
        self.key_quads[2].len() == 0 && self.key_quads[3].len() == 0
    }

    // implements the floodfill alg in the background.
    pub fn find_accessible_keys(&self, map: &CharMatrix) -> Vec<(char, usize)> {
        let mut key_vec: Vec<(char, usize)> = Vec::new();
        for i in 0..4 {
            let walkable_spaces = util::get_walkable_spaces(&map, &self.door_quads[i], self.player_positions[i]);
        
            // check from key positions
            for (ch, p) in &self.key_quads[i] {
                if walkable_spaces.get(*p).unwrap() == true {
                    key_vec.push( (*ch, i) );
                }
            }
        }

        key_vec
    }

    // returns a copy of the current node, but with a map that has an updated 
    // player location & with the key & door cooresponnding to c removed.
    pub fn got_key(&self, astar_calls: &mut AstarCache, map: &CharMatrix, key_ch: char, index: usize) -> QuadNode {
        let mut player_positions = [
            self.player_positions[0], self.player_positions[1], 
            self.player_positions[2], self.player_positions[3]
        ];
        
        // make copies of keys and doors.
        let mut key_quads = [Vec::new(), Vec::new(), Vec::new(), Vec::new()];
        let mut door_quads = [Vec::new(), Vec::new(), Vec::new(), Vec::new()];
        for i in 0..4 {
            for (ch, p) in &self.key_quads[i] {
                if *ch != key_ch {
                    key_quads[i].push( (*ch, *p) );
                } else {
                    player_positions[index] = *p;  // only update the selected robot
                }
            }
            for (ch, p) in &self.door_quads[i] {
                if *ch != key_ch.to_ascii_uppercase() {
                    door_quads[i].push( (*ch, *p) );
                }
            }
        }
        
        // Do A* from self.player_pos -> player_pos (using newly updated information)
        let (old_p, new_p) = (self.player_positions[index], player_positions[index]);
        // Also memoise same calls.
        let move_len = match astar_calls.get( &(door_quads[index].clone(), old_p, new_p) ) {
            Some(val) => *val,
            None => {
                let val = util::astar_pathfind(&map, &door_quads[index], old_p, new_p).len();
                let key = (door_quads[index].clone(), old_p, new_p);
                astar_calls.insert(key, val);
                val
            },
        };

        QuadNode {
            key_quads,
            door_quads,
            player_positions,
            path_len: self.path_len + move_len,
        }
    }

    // This function computes a hash which disregards the length of the path to the node.
    // Now, nodes which are hash identical, but have a longer or equal path length can safely be pruned because
    // the shorter nodes are ultimately better. (or exactly the same if equal)
    pub fn state_hash(&self) -> u64 {
        let mut s = DefaultHasher::new();
        self.key_quads.hash(&mut s);
        self.player_positions.hash(&mut s);
        s.finish()
    }
}
impl Hash for QuadNode {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.key_quads.hash(state);
        self.player_positions.hash(state);
        self.path_len.hash(state);
    }
}
impl PartialEq for QuadNode {
    fn eq(&self, other: &Self) -> bool {
        self.key_quads == other.key_quads && 
        self.player_positions == other.player_positions && 
        self.path_len == other.path_len
    }
}
impl Eq for QuadNode {}

#[derive(Clone, Eq, PartialEq, Hash)]
pub struct CharMatrix {
    pub width: usize,
    pub data: Vec<char>,
}
impl CharMatrix {
    pub fn from_file(filepath: &str) -> CharMatrix {
        let map_string = match fs::read_to_string(filepath) {
            Ok(string) => string,
            Err(_) => {
                println!("Error: Input path does not exist, or cannot be opened.");
                process::exit(1);
            },
        };
    
        // find width.
        let mut map_width: usize = 0;
        match map_string[..].find('\n') {
            Some(v) => {
                map_width = v;
            },
            None => {
                println!("Error: Must contain newline.");
                process::exit(1);
            },
        };
    
        // map string to Tile enum.
        let mut char_vec: Vec<char> = Vec::new();
        for ch in map_string.chars() {
            match ch { // skip newline characters
                '\n' => {},
                '\r' => {},
                c => char_vec.push(c),
            };
        }
    
        CharMatrix {
            width: map_width, 
            data: char_vec,
        }
    }
    // This function turns spaces which never need to be accessed (don't lead to keys)
    // into walls so that A* & walkable space calls are faster.
    // I didn't optimize this function because it is a pre-processing step.
    pub fn propagate_walls(&mut self, include_walls: bool) {
        let mut check_again = true;
        while check_again {
            check_again = false;
            for (i, ch) in self.data.clone().iter().enumerate() {
                let p = Point2D::new(i % self.width, i / self.width);
                if !self.is_edge(p) && (self.get(p) == '.' || (include_walls && self.get(p).is_ascii_uppercase())) {
                    let adj = vec![
                        p.from(Action::Up), p.from(Action::Down), 
                        p.from(Action::Left), p.from(Action::Right)
                    ];
                    let wall_count: usize = adj.iter().map(|p| 
                        if self.get(*p) == '#' { 1 } else { 0 } 
                    ).sum();
                    // case: this position is a boring ass corner. Also, corners can make more corners.
                    if wall_count >= 3 { 
                        self.set(p, '#');
                        check_again = true;
                    }
                }
            }
        }
    }
    // This function takes multiple keys in the same corridor (no junctions between them) and turns them into
    // the same key. Assumes that the structure of the maze is that of a tree (only one way in & out).
    pub fn propagate_keys(&mut self) {
        // TODO: this
    }
    pub fn get(&self, p: Point2D) -> char {
        self.data[p.y * self.width + p.x]
    }
    pub fn set(&mut self, p: Point2D, val: char) {
        self.data[p.y * self.width + p.x] = val;
    }
    pub fn is_edge(&self, p: Point2D) -> bool {
        p.x == 0 || p.x == self.width || p.y == 0 || p.y == self.data.len() / self.width
    }
    pub fn print(&self) {
        for (i, tile) in self.data.iter().enumerate() {
            if i % self.width == 0 {
                print!("\n");
            }
            print!("{}", tile);
        }
        print!("\n");
    }
}

#[derive(Clone)]
pub struct BitMatrix {
    pub width: usize,
    pub bv: BitVec,
} 
impl BitMatrix {
    pub fn new(width: usize, len: usize) -> BitMatrix {
        BitMatrix {
            width, bv: BitVec::from_elem(len, false)
        }
    }
    pub fn get(&self, p: Point2D) -> Option<bool> {
        self.bv.get(p.y * self.width + p.x)
    }
    pub fn set(&mut self, p: Point2D, val: bool) {
        self.bv.set(p.y * self.width + p.x, val);
    }
}

// Simple point struct
#[derive(Copy, Clone, Hash, PartialEq, Eq)]
pub struct Point2D {
    pub x: usize,
    pub y: usize,
}
impl Point2D {
    pub fn new(x: usize, y: usize) -> Point2D {
        Point2D { x, y }
    }
    pub fn from(&self, action: Action) -> Point2D {
        match action {
            Action::Up => Point2D::new(self.x, self.y - 1),
            Action::Down => Point2D::new(self.x, self.y + 1),
            Action::Left => Point2D::new(self.x - 1, self.y),
            Action::Right => Point2D::new(self.x + 1, self.y),
        }
    }
}

#[derive(Copy, Clone)]
pub enum Action {
    Up,
    Down,
    Left,
    Right,
}
impl Action {
    pub fn to_string(actions: &Vec<Action>) -> String {
        let mut s = String::with_capacity(actions.len());
        for item in actions {
            match item {
                Action::Up => s.push('u'),
                Action::Down => s.push('d'),
                Action::Left => s.push('l'),
                Action::Right => s.push('r'),
            }
        }
        s
    }

    // inverse of no move is no move.
    pub fn inverse(&self) -> Action {
        match self {
            Action::Up => Action::Down,
            Action::Down => Action::Up,
            Action::Left => Action::Right,
            Action::Right => Action::Left,
        }
    }
}
