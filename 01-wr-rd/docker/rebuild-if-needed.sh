#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPO_ROOT="$SCRIPT_DIR"
while [ "$REPO_ROOT" != "/" ] && [ ! -d "$REPO_ROOT/.git" ]; do
  REPO_ROOT=$(dirname "$REPO_ROOT")
done
if [ ! -d "$REPO_ROOT/.git" ]; then
  echo "[rebuild-if-needed] Could not find repo root with .git" >&2
  exit 1
fi

IMAGE_NAME="gst-meta-test-img"
CONTAINER_NAME="gst-meta-test"
DOCKERFILE="$SCRIPT_DIR/Dockerfile"

echo "[rebuild-if-needed] Forcing rebuild due to dependency changes..."
if [[ "$(docker ps -a -q -f name=^/${CONTAINER_NAME}$ 2> /dev/null)" != "" ]]; then
  echo "[rebuild-if-needed] Stopping/removing container..."
  docker stop "$CONTAINER_NAME" >/dev/null 2>&1 || true
  docker rm "$CONTAINER_NAME" >/dev/null 2>&1 || true
fi

docker build -t $IMAGE_NAME -f "$DOCKERFILE" "$SCRIPT_DIR"

echo "[rebuild-if-needed] Creating new container..."
docker run -d \
  --name "$CONTAINER_NAME" \
  -v "$REPO_ROOT:/work:rw" \
  -w /work \
  "$IMAGE_NAME"

echo "[rebuild-if-needed] Done"
