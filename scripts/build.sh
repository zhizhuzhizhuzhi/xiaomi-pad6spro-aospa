#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
set -eo pipefail
kit=$(cd "$(dirname "$0")/.." && pwd)
tree=$(realpath "${1:?Usage: build.sh ANDROID_TREE [userdebug|user] [jobs]}")
variant=${2:-userdebug}; jobs=${3:-8}
case "$variant" in user|userdebug) ;; *) echo 'Unsupported variant' >&2; exit 1 ;; esac
[[ "$jobs" =~ ^[1-9][0-9]*$ ]] || { echo 'jobs must be a positive integer' >&2; exit 1; }
python3 "$kit/scripts/apply.py" "$tree" --verify-applied
python3 "$kit/scripts/fetch-dependencies.py" "$tree" --verify
cd "$tree"
exec bash build-device.sh sheng "$variant" -j"$jobs" target-files-package otatools
