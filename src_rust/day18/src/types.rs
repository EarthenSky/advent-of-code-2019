use std::fs;
use std::process;

use bit_vec::BitVec;

use crate::util;

#[derive(Eq, PartialEq, Hash)]
pub struct Node {
    pub map: CharMatrix,
    pub player_pos: Point2D,
    pub path_len: usize,
}
impl Node {
    pub fn make_root(mut map: CharMatrix) -> Node {
        let mut player_pos = Point2D::new(0, 0);
        for (i, ch) in map.data.iter().enumerate() {
            if *ch == '@' {
                player_pos = Point2D::new(i % map.width, i / map.width);
                break;
            }
        }
        
        map.set(player_pos, '.');

        Node {
            map,
            player_pos,
            path_len: 0,
        }
    }

    // is goal if there are no more keys / doors.
    pub fn is_goal(&self) -> bool {
        for ch in &self.map.data {
            match ch {
                '.' => (),
                '#' => (),
                '@' => (),
                _ => return false,
            };
        }
        true
    }

    // implements the floodfill alg in the background.
    pub fn find_accessible_keys(&self, key_positions: &Vec<Point2D>) -> Vec<char> {
        let mut key_vec: Vec<char> = Vec::new();
        let walkable_spaces = util::get_walkable_spaces(&self.map, self.player_pos);
        
        // check from key positions
        for p in key_positions {
            if walkable_spaces.get(*p).unwrap() == true && self.map.get(*p) != '.' {
                key_vec.push( self.map.get(*p) );
            }
        }

        key_vec
    }

    // returns a copy of the current node, but with a map that has a new player 
    // location & with the key & door cooresponnding to c removed.
    // This function should also use A* to update the path length it took to get to this position.
    pub fn got_key(&self, key_ch: char) -> Node {
        let mut map = self.map.clone();
        let mut player_pos = Point2D::new(0, 0);
        for (i, ch) in self.map.data.iter().enumerate() {
            let point = Point2D::new(i % map.width, i / map.width);
            if *ch == key_ch.to_ascii_uppercase() { // door
                map.set(point, '.');
            } else if *ch == key_ch { // key
                map.set(point, '.');
                player_pos = point;
            }
        }
        
        // Do A* from self.player_pos -> player_pos
        let move_len = util::astar_pathfind(&map, self.player_pos, player_pos).len();

        Node {
            map,
            player_pos,
            path_len: self.path_len + move_len,
        }
    }

    pub fn print(&self) {
        self.map.print();
    }
}

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
                map_width = v - 1;
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
    pub fn get(&self, p: Point2D) -> char {
        self.data[p.y * self.width + p.x]
    }
    pub fn set(&mut self, p: Point2D, val: char) {
        self.data[p.y * self.width + p.x] = val;
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
            Action::NoMove => self.clone(),
        }
    }
}

#[derive(Copy, Clone)]
pub enum Action {
    Up,
    Down,
    Left,
    Right,
    NoMove,
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
                Action::NoMove => (),
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
            Action::NoMove => Action::NoMove,
        }
    }
}
