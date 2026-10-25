#!/usr/bin/env bash
set -uo pipefail

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/src"
DEV_DIR="$(dirname "${BASH_SOURCE[0]}")"
RAW_DIR="$DEV_DIR/raw"
mkdir -p "$RAW_DIR"

export GST_PLUGIN_PATH="$SRC_DIR/build"
export GST_DEBUG_DUMP_DOT_DIR=/tmp
# Включаем debug для меты чтобы видеть transform
export GST_DEBUG="GST_META:5,mymetawriter:4,mymetareader:4"

NUM_BUFFERS=${NUM_BUFFERS:-300}
RUNS=${RUNS:-10}

# Массив связок
declare -a PIPELINES=(
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! queue ! mymetawriter ! queue ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! videoconvert ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! videoscale ! videoconvert ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! identity drop-allocation=true ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! videorate ! mymetareader ! fakesink"
  "videotestsrc num-buffers=$NUM_BUFFERS ! mymetawriter ! jpegenc ! jpegdec ! mymetareader ! fakesink"
)

declare -a NAMES=(
  "base_writer_reader"
  "with_queues"
  "writer_videoconvert_reader"
  "writer_videoscale_videoconvert_reader"
  "identity_dropalloc"
  "writer_videorate_reader"
  "jpegenc_jpegdec"
)

for i in "${!PIPELINES[@]}"; do
  name="${NAMES[$i]}"
  pipe="${PIPELINES[$i]}"
  echo "=== RUNNING $name ===" >&2
  for r in $(seq 1 $RUNS); do
    out="$RAW_DIR/${name}_run${r}.log"
    # gst-launch может падать с ошибкой, но мы всё равно парсим
    timeout 60 gst-launch-1.0 $pipe >"$out" 2>&1 || true
    echo "  $name run$r done" >&2
  done
done

echo "ALL_DONE" >&2
