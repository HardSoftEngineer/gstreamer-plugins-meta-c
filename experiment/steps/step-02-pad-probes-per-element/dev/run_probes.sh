#!/usr/bin/env bash
set -uo pipefail

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/src"
DEV_DIR="$(dirname "${BASH_SOURCE[0]}")"
RAW_DIR="$DEV_DIR/raw_probes"
mkdir -p "$RAW_DIR"

export GST_PLUGIN_PATH="$SRC_DIR/build"
export GST_DEBUG="GST_META:5,mymetawriter:4,mymetareader:4"

NUM_BUFFERS=${NUM_BUFFERS:-300}
RUNS=${RUNS:-10}

# Тестируем только проходимость через один элемент между writer и reader
declare -a PIPELINES=(
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! videoconvert ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! videoscale ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! identity drop-allocation=true ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! videorate ! mymetareader ! fakesink"
)

declare -a NAMES=(
  "probe_ref"
  "probe_videoconvert"
  "probe_videoscale"
  "probe_identity_dropalloc"
  "probe_videorate"
)

for i in "${!PIPELINES[@]}"; do
  name="${NAMES[$i]}"
  pipe="${PIPELINES[$i]}"
  echo "=== $name ===" >&2
  for r in $(seq 1 $RUNS); do
    out="$RAW_DIR/${name}_run${r}.log"
    timeout 60 gst-launch-1.0 $pipe >"$out" 2>&1 || true
  done
done
echo "ALL_DONE" >&2
