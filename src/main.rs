extern crate rand;

use rand::Rng;

struct Vec2 {
    x: f32,
    y: f32,
}

const POINTS_LEN: usize = 100_000;
const THREAD_NUM: usize = 1;

fn main() {
    let mut points: Vec<Vec2> = Vec::new();

    for _ in 0..POINTS_LEN {
        points.push(Vec2 {x: 0.0, y: 0.0});
    }

    'running: loop {
        let start_frame = std::time::Instant::now();

        let _ = crossbeam::scope(|scope| {
            for slice in points.chunks_mut(POINTS_LEN/THREAD_NUM) {
                scope.spawn(move |_| {
                    let mut rng = rand::thread_rng();
                    for point in slice {
                        point.x += if rng.gen::<bool>() { rng.gen::<f32>() } else { -rng.gen::<f32>() };
                        point.y += if rng.gen::<bool>() { rng.gen::<f32>() } else { -rng.gen::<f32>() };
                    }
                });
            }
        });

        let frame_time = std::time::Instant::now().duration_since(start_frame);
        println!["FPS: {}", 1_000_000_000/frame_time.subsec_nanos()];
    }
}
