extern crate rand;

use rand::Rng;

struct Vec2 {
    x: usize,
    y: usize,
}

const POINTS_LEN: usize = 100_00000;
const THREAD_NUM: usize = 4;

fn xorshift(state: &mut usize) -> usize {
    let mut x = *state;
    x^= x << 13;
    x^= x >> 7;
    x^= x << 17;
    *state = x;
    return x;
}

fn main() {
    let mut points: Vec<Vec2> = Vec::new();

    for i in 0..POINTS_LEN {
        points.push(Vec2 {x: 0, y: 0});
    }

    'running: loop {
        let start_frame = std::time::Instant::now();

        let _ = crossbeam::scope(|scope| {
            for (i, slice) in points.chunks_mut(POINTS_LEN/THREAD_NUM).enumerate() {
                let mut s = 111*(i+90);
                scope.spawn(move |_| {
                    let mut rng = rand::thread_rng();
                    for point in slice {
                        point.x = xorshift(&mut s);
                        point.y = xorshift(&mut s);
                    }
                });
            }
        });

        let frame_time = std::time::Instant::now().duration_since(start_frame);
        println!["FPS: {}", 1_000_000_000/frame_time.subsec_nanos()];
    }
}
