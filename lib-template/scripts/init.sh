#!/usr/bin/env bash
# Rename the template library. Run once, right after creating a repo from the template:
#     ./scripts/init.sh lmstdlib
# Replaces mylib / MYLIB everywhere and moves include/mylib/ to include/<name>/.
set -euo pipefail

new="${1:-}"
if [[ ! "$new" =~ ^[a-z][a-z0-9_]*$ ]]; then
    echo "usage: $0 <name>   (lowercase letters, digits, underscores; starts with a letter)" >&2
    exit 1
fi
upper="$(tr '[:lower:]' '[:upper:]' <<< "$new")"

cd "$(dirname "$0")/.."
[[ -d include/mylib ]] || { echo "include/mylib not found: already renamed?" >&2; exit 1; }

mv include/mylib "include/$new"

# File names first (include/<new>/mylib.hpp, tests/test_mylib.cpp, ...), then contents.
find . -path ./.git -prune -o -path ./build -prune -o -type f -name '*mylib*' -print |
while IFS= read -r f; do
    mv "$f" "$(dirname "$f")/$(basename "$f" | sed "s/mylib/$new/g")"
done

grep -rlI --exclude-dir=.git --exclude-dir=build --exclude=init.sh -e mylib -e MYLIB . |
while IFS= read -r f; do
    sed -i.bak -e "s/mylib/$new/g" -e "s/MYLIB/$upper/g" "$f"
    rm -f "$f.bak"
done

echo "Renamed to '$new'. Next: edit project(... VERSION ...) in CMakeLists.txt, then 'make check'."
echo "This script is single-use; delete scripts/init.sh whenever you like."
