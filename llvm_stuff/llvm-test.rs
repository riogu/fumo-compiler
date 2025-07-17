use std::io;
pub fn square(x: i32) -> i32 {
  x * x
}
fn main() -> i32 {
    let mut buffer = String::new();
    io::stdin().read_line(&mut buffer).expect("input not an int lmaoooooo.");
    let var = buffer.trim().parse().expect("holy shit stop rust");
    return square(var);
}
