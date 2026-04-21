#!/usr/bin/env bash

set -euo pipefail

ROOT="${1:-.}"

find "$ROOT" -type f -name "AGENTS.md" | while read -r agent_file; do
    dir="$(dirname "$agent_file")"

    for name in GEMINI.md CLAUDE.md; do
        target="$dir/$name"

        if [ -e "$target" ] || [ -L "$target" ]; then
            echo "Skipping $target (already exists)"
        else
            ln -s "AGENTS.md" "$target"
            echo "Created $target -> AGENTS.md"
        fi
    done
done
