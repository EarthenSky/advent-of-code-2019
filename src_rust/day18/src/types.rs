use std::collections::HashMap;
type AstarCache = HashMap<(Vec<(char, Point2D)>, Point2D, Point2D), usize>;

use std::fs;
use std::process;
use std::hash::Hash;

use bit_vec::BitVec;

use crate::util;

#[derive(Clone, Eq, PartialEq, Hash)]
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
}
/*
impl Hash for Node {
    // keys & doors are interconnected & path_len is unique for each node, so it  
    // should not be considered. Player position should also not be considered, 
    // because it is a unique factor for each node.
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.keys.hash(state);
        //self.player_pos.hash(state);
    }
}
impl PartialEq for Node {
    // Same reasoning as with hash. -> only keys are important for uniqueness
    // due to the order we are traversing the collection.
    fn eq(&self, other: &Self) -> bool {
        self.keys == other.keys
    }
}
impl Eq for Node {}
*/
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
