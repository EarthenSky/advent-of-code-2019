use criterion::{criterion_group, criterion_main, Criterion};
use day16::part1_silent;

pub fn criterion_benchmark(c: &mut Criterion) {
    c.bench_function( "silent_p1_test", |b| b.iter(|| part1_silent()) );
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);