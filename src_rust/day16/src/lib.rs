use std::fs;
use std::env;
use std::path::Path;


// ************************************************************************* //


fn read_signal(filename: &str) -> Vec<u32> {
    // construct proper path to file.
    let current_directory = env::current_dir().unwrap();
    let path = Path::new(&current_directory);
    let path = path.join(filename);

    // read path to string
    let content_string = fs::read_to_string(&path)
        .expect("File read failed.");

    // convert string to numbers
    let mut signal_list: Vec<u32> = Vec::new();
    for c in content_string.chars() {
        signal_list.push( c.to_digit(10).unwrap() );
    }

    signal_list
}

// process the signal according to the message offset
fn process_signal(filename: &str) -> Vec<u32> {
    let REP_COUNT: u32 = 10_000;
    
    let signal_base = read_signal(filename);
    let mut message_offset: usize = 0;
    for (i, num) in signal_base[0..7].iter().enumerate() {
        message_offset += (*num as usize) * 10_usize.pow(6 - (i as u32));
    }

    // double check that the method will work
    assert!(message_offset > (signal_base.len() * REP_COUNT as usize / 2));

    // create signal
    let mut signal = Vec::new();
    for _ in 0..REP_COUNT {
        signal.extend(signal_base.clone());
    }

    signal.drain(0..message_offset);
    
    signal
}


// ************************************************************************* //


// returns a wave pattern as a vector of size length
// period cannot be 0.
fn get_pattern(period: u32, length: usize) -> Vec<i32> {
    assert!(period != 0);

    let mut repetitions = period - 1;  // repetitions represents how far through the current value it is.
    let mut wave_state: (i32, i32) = (0, 1);  // the first value is position of the wave, the second value is direction.

    let mut pattern = Vec::new(); 
    for _ in 0..length {
        if repetitions == 0 {
            repetitions = period;  // refresh repetitions to full.
            if wave_state.0 == 0 {
                wave_state = (wave_state.0 + wave_state.1, wave_state.1);
            } else {
                wave_state = (0, -wave_state.1); // change direction of wave.
            }
        }

        pattern.push(wave_state.0);
        repetitions -= 1;
    }

    pattern
}

fn apply_fft(phases: u32, list: &Vec<u32>) -> Vec<u32> {
    let mut input = list.clone();
   
    // do n phases
    for _ in 0..phases {
        let mut output = Vec::new();

        // do one phase & fill output.
        for i in 0..list.len() as u32 {
            let mut cur_out_val: i32 = 0;
            let pattern = get_pattern(i + 1, list.len());
            // sum patterm combined values.
            //for j in (i as usize)..list.len() {
            //    cur_out_val += (input[j] as i32) * pattern[j];
            //}
            for (j, val) in input.iter().enumerate() {
                cur_out_val += (*val as i32) * pattern[j];
            }
            
            output.push((cur_out_val.abs() % 10) as u32); 
        }

        input = output;  // update the input
    }

    input
}

fn apply_fft_improved(phases: u32, list: &Vec<u32>) -> Vec<u32> {
    let mut input = list.clone();
   
    // generate pattern map beforehand & leave in memory
    let mut pattern_map: Vec<Vec<i32>> = Vec::new();
    for i in 0..list.len() {
        pattern_map.push( get_pattern((i + 1) as u32, list.len()) );
    }

    // do n phases
    for _ in 0..phases {
        let mut output = Vec::new();

        // do one phase & fill output.
        for i in 0..list.len() {
            let mut cur_out_val: i32 = 0;
            
            // sum patterm combined values.
            for j in i..list.len() {
                cur_out_val += (input[j] as i32) * pattern_map[i][j];
            }
            
            output.push((cur_out_val.abs() % 10) as u32); 
        }

        input = output;  // update the input
    }

    input
}

// $list must be the end half of a signal
fn apply_fft_pruned(phases: u32, list: &Vec<u32>) -> Vec<u32> {
    let mut input = list.clone();

    println!("len -> {}", list.len());

    // do n phases
    for p in 0..phases {
        let mut output = Vec::new();

        // do one phase & fill output.
        for i in 0..list.len() {
            let mut cur_out_val: i32 = 0;
            
            // sum patterm combined values.
            for j in i..list.len() {
                cur_out_val += input[j] as i32;
            }
            
            output.push((cur_out_val.abs() % 10) as u32); 
        }

        println!("phase {} done", p);

        input = output;  // update the input
    }

    input
}

// optimized to run in time O(n), rather than O(n^2)
fn apply_fft_pruned_improved(phases: u32, list: &Vec<u32>) -> Vec<u32> {
    let mut input = list.clone();

    println!("len -> {}", list.len());

    // do n phases
    for p in 0..phases {
        let mut output = Vec::new();
        let mut cur_out_val: u32 = input.iter().sum();

        // remove leftmost value (b/c turned to zero) each reptition
        for i in 0..input.len() {
            output.push(cur_out_val % 10); 
            cur_out_val -= input[i];
        }

        println!("phase {} done", p);

        input = output;  // update the input
    }

    input
}


// ************************************************************************* //


pub fn part1() {
    println!("//** Part 1 **//");

    // read input from file & fill vector with numbers
    let signal_list = read_signal("input.txt");
    println!("initial list -> {:?}", signal_list);

    let signal_list_output = apply_fft_improved(100, &signal_list);
    println!("final output list after 100 phases -> {:?}", signal_list_output);
    
    // puzzle solution -> 63483758
}

pub fn part1_quiet() {
    let signal_list = read_signal("input.txt");
    let signal_list_output = apply_fft_improved(100, &signal_list);
    println!("{:?}", signal_list_output.len());
}

pub fn part2() {
    println!("//** Part 2 **//");

    let pruned_signal = process_signal("input.txt");
    let output_signal = apply_fft_pruned_improved(100, &pruned_signal);
    println!("final output list after 100 phases -> {:?}", &output_signal[..8]);
}