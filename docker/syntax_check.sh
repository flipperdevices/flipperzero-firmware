#!/usr/bin/env bash

CLANG_FORMAT_BIN="/usr/bin/clang-format-10"
PATH="$HOME/.cargo/bin:${PATH}"

cd /project

echo "RUN C\C++ SYNTAX CHECK"
C_FILES=$(find . \
    -not \( -path ./target_f1/Middlewares -prune \) \
    -not \( -path ./target_f1/Drivers -prune \) \
    -name *.c -o -name *.h -o -name *.cpp)
$CLANG_FORMAT_BIN --verbose -style=file -n --Werror --ferror-limit=0 $C_FILES
c_syntax_rc=$?

echo "RUN RUST SYNTAX CHECK"
RUST_FILES=$(find . -name *.rs)
rustfmt --check $RUST_FILES
rust_syntax_rc=$?

if [[ $rust_syntax_rc -eq 0 ]] && [[ $c_syntax_rc -eq 0 ]]; then
    echo "Code looks fine for me!"
    exit 1
fi

read -p "Do you want fix syntax? (y/n): " confirm && [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]] || exit 1

rustfmt $RUST_FILES

# We use root in container and clang-format rewriting files. We'll need change owner to original
local_user=$(stat -c '%u' .clang-format)
$CLANG_FORMAT_BIN -style=file -i $C_FILES
chown $local_user $C_FILES