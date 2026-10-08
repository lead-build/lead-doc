extern crate cbindgen;

use std::{env, path::PathBuf};

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    // If CBINDGEN_HEADER_OUTPUT is relative, it should be relative to PWD, not
    // CARGO_MANIFEST_DIR, to match the structure of the ninja build.
    let pwd = PathBuf::from(env::var("PWD").unwrap());
    let header_name = PathBuf::from(env::var("CBINDGEN_HEADER_OUTPUT").unwrap());
    let header_path = pwd.join(&header_name);

    cbindgen::Builder::new()
        .with_crate(&crate_dir)
        .with_config(cbindgen::Config::from_root_or_default(&crate_dir))
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(&header_path);
}
