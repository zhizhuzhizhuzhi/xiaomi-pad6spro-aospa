#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -eo pipefail
kit=$(cd "$(dirname "$0")/.." && pwd)
tree=$(realpath "${1:?Usage: prepare.sh ANDROID_TREE STOCK_DUMP [input arguments...]}")
dump=$(realpath "${2:?Matching Sheng stock partition dump is required}")
shift 2
python3 "$kit/scripts/apply.py" "$tree"
cd "$tree"
python3 device/xiaomi/sheng/extract-files.py "$dump"
python3 device/xiaomi/sheng/tools/extract-device-inputs.py --tree "$tree" --source "$dump" "$@"
python3 device/xiaomi/sheng/displayfeature/apply-compat-libraries.py
python3 "$kit/scripts/fetch-dependencies.py" "$tree" --verify
