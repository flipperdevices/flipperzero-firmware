#!/usr/bin/env bash

CLANG_FORMAT_BIN="/usr/bin/clang-format-10"
PATH="$HOME/.cargo/bin:${PATH}"

cd /project

echo "RUN C\C++ SYNTAX CHECK"

C_FILES=$(find . -not \( -path ./target_f1/Middlewares -prune \) -not \( -path ./target_f1/Drivers -prune \) -name *.c -o -name *.h -o -name *.cpp)
$CLANG_FORMAT_BIN -style=file -n --Werror --ferror-limit=0 $C_FILES


echo "RUN RUST SYNTAX CHECK"
RUST_FILES=$(find . -name *.rs)
rustup component add rustfmt --toolchain stable-x86_64-unknown-linux-gnu
rustfmt --check $RUST_FILES
