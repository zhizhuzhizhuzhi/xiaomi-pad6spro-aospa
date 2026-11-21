#!/usr/bin/env bash
set -eo pipefail
tree=$(realpath "${1:?Usage: prepare.sh ANDROID_TREE PREPARED_INPUTS}")
inputs=$(realpath "${2:?Prepared input directory is required}")
python3 "$tree/vendor/xiaomi/sm8550-integration/tools/prepare-source.py" "$tree" --device sheng
python3 "$tree/vendor/xiaomi/sm8550-integration/tools/prepare-inputs.py" "$tree" --device sheng --source "$inputs" --replace-stock-inputs
