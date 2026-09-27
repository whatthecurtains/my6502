#!/usr/bin/env python3

import json
import re
import sys
from pathlib import Path


def fix_duplicate_inst_action(text: str) -> str:
    pattern = r'("inst_action"\s*:\s*"(?:[^"\\]|\\.)*")\s*"inst_action"\s*:\s*"(?:[^"\\]|\\.)*"'
    fixed = re.sub(pattern, r"\1", text)
    return fixed


def strip_illegal_actions(source_path: Path, dest_path: Path) -> None:
    text = source_path.read_text(encoding="utf-8")

    # op2.json contains a malformed duplicate "inst_action" entry in one object.
    # Remove the duplicate key before parsing JSON.
    cleaned = fix_duplicate_inst_action(text)

    try:
        data = json.loads(cleaned)
    except json.JSONDecodeError as exc:
        raise ValueError(f"Could not parse JSON in {source_path}: {exc}") from exc

    opcodes = data.get("OPCODES", [])
    for entry in opcodes:
        if isinstance(entry, dict) and entry.get("illegal") is True:
            entry["inst_action"] = ""

    with dest_path.open("w", encoding="utf-8") as dst:
        json.dump(data, dst, indent=2)
        dst.write("\n")


if __name__ == "__main__":
    base_dir = Path(__file__).resolve().parent
    source_path = Path(sys.argv[1]) if len(sys.argv) > 1 else base_dir / "op2.json"
    dest_path = Path(sys.argv[2]) if len(sys.argv) > 2 else base_dir / "op3.json"
    print(f"Source path: {source_path}")
    print(f"Destination path: {dest_path}")
    strip_illegal_actions(source_path, dest_path)
