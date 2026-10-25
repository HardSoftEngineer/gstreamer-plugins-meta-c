#!/usr/bin/env bash
set -euo pipefail
CONTAINER_NAME="gst-meta-test"
if [[ $# -eq 0 ]]; then
  echo "Usage: $0 <command> [args...]" >&2
  exit 1
fi
docker exec -i "$CONTAINER_NAME" "$@"
