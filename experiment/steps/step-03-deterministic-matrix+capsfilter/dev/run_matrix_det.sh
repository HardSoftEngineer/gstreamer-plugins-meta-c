#!/usr/bin/env bash
set -uo pipefail

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/src"
DEV_DIR="$(dirname "${BASH_SOURCE[0]}")"
RAW_DIR="$DEV_DIR/raw"
mkdir -p "$RAW_DIR"

export GST_PLUGIN_PATH="$SRC_DIR/build"
export GST_DEBUG="GST_META:5,mymetawriter:4,mymetareader:4"

NUM_BUFFERS=${NUM_BUFFERS:-300}
RUNS=${RUNS:-10}
CAPS="video/x-raw,format=I420,width=320,height=240,framerate=30/1"

declare -a PIPELINES=(
  "videotestsrc num-buffers=$NUM_BUFFERS ! capsfilter caps=$CAPS ! mymetawriter ! capsfilter caps=$CAPS ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! capsfilter caps=$CAPS ! mymetawriter ! videoconvert ! capsfilter caps=$CAPS ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! capsfilter caps=$CAPS ! mymetawriter ! videoscale ! videoconvert ! capsfilter caps=$CAPS ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! capsfilter caps=$CAPS ! mymetawriter ! identity drop-allocation=true ! capsfilter caps=$CAPS ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! capsfilter caps=$CAPS ! mymetawriter ! videorate ! capsfilter caps=$CAPS ! mymetareader ! fakesink"
)

declare -a NAMES=(
  "det_ref"
  "det_videoconvert"
  "det_videoscale_videoconvert"
  "det_identity_dropalloc"
  "det_videorate"
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
