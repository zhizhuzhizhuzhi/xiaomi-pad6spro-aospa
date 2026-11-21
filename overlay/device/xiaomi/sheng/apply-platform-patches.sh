#!/usr/bin/env bash
set -euo pipefail
script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
source_root=$(cd -- "$script_dir/../../.." && pwd)
cd "$source_root"
for patch_file in "$script_dir"/patches/*.patch; do
    if git apply --reverse --check "$patch_file" >/dev/null 2>&1; then
        continue
    fi
    git apply --check "$patch_file"
    git apply "$patch_file"
done

python3 "$script_dir/displayfeature/apply-compat-libraries.py"
