#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPO_ROOT=$(cd "$SCRIPT_DIR/.." && pwd)

IMAGE_NAME="gst-meta-test-img"
CONTAINER_NAME="gst-meta-test"
DOCKERFILE="$SCRIPT_DIR/Dockerfile"

# Build image only if missing or if Dockerfile changed? 
# "только при координальной смене зависимостей среды" - по умолчанию не ребилдим
if [[ "$(docker images -q $IMAGE_NAME 2> /dev/null)" == "" ]]; then
  echo "[up.sh] Image not found, building..."
  docker build -t $IMAGE_NAME -f "$DOCKERFILE" "$SCRIPT_DIR"
else
  echo "[up.sh] Image $IMAGE_NAME exists, skipping build (rebuild-if-needed.sh if deps changed)"
fi

# Create container if missing, else start if stopped
if [[ "$(docker ps -a -q -f name=^/${CONTAINER_NAME}$ 2> /dev/null)" == "" ]]; then
  echo "[up.sh] Creating long-lived container..."
  docker run -d \
    --name "$CONTAINER_NAME" \
    -v "$REPO_ROOT:/work:rw" \
    -w /work \
    "$IMAGE_NAME"
  echo "[up.sh] Container created"
else
  if [[ "$(docker ps -q -f name=^/${CONTAINER_NAME}$ 2> /dev/null)" == "" ]]; then
    echo "[up.sh] Starting existing container..."
    docker start "$CONTAINER_NAME"
  else
    echo "[up.sh] Container already running"
  fi
fi

echo "[up.sh] Done. Use docker/exec.sh or docker/shell.sh"
