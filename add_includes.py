import os
import re
from pathlib import Path

# Directories to scan
SCAN_DIRS = [
    "src/Client/Module/Modules",
    "src/Client/Command",
    "src/Client/Hook",
    "src/Client/Events"
]

# Patterns to detect usage
PATTERNS = {
    'Assets/Assets.hpp': [
        r'\bIDR_\w+\b',  # Any IDR_ constant
    ],
    'Events/EventManager.hpp': [
        r'\bListen\s*\(',
        r'\bListenOrdered\s*\(',
        r'\bDeafen\s*\(',
        r'\beventMgr\b',
    ],
    'Events/Events.hpp': [
        r'\b\w+Event\b',  # Any class/type ending with Event
    ],
    'Utils/Memory/Game/SignatureAndOffsetManager.hpp': [
        r'\bGET_OFFSET\s*\(',
        r'\bGET_SIG_ADDRESS\s*\(',
        r'\bGET_SIG\s*\(',
    ]
}

def find_last_include_line(lines):
    """Find the line number after the last #include directive"""
    last_include_idx = -1
    for i, line in enumerate(lines):
        if line.strip().startswith('#include'):
            last_include_idx = i
    return last_include_idx

def has_include(lines, include_path):
    """Check if file already has the include"""
    include_pattern = re.escape(include_path)
    for line in lines:
        if re.search(rf'#include\s+[<"].*{include_pattern}[>"]', line):
            return True
    return False

def needs_include(content, patterns):
    """Check if content uses any of the patterns"""
    for pattern in patterns:
        if re.search(pattern, content):
            return True
    return False

def get_header_includes(cpp_filepath):
    """Get includes from corresponding header file if it exists"""
    header_path = cpp_filepath.replace('.cpp', '.hpp')
    if not os.path.exists(header_path):
        return []

    try:
        with open(header_path, 'r', encoding='utf-8', errors='ignore') as f:
            return f.read().split('\n')
    except:
        return []

def process_file(filepath):
    """Process a single file and add necessary includes"""
    try:
        # Skip files with "event" in the name
        if 'event' in os.path.basename(filepath).lower():
            return False

        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            lines = content.split('\n')

        # Skip if file is empty or has no includes
        if not content.strip():
            return False

        includes_to_add = []

        # For .cpp files, check if header already has the includes
        header_lines = []
        if filepath.endswith('.cpp'):
            header_lines = get_header_includes(filepath)

        # Check each include type
        for include_path, patterns in PATTERNS.items():
            if needs_include(content, patterns) and not has_include(lines, include_path):
                # If this is a .cpp file and the header has the include, skip it
                if filepath.endswith('.cpp') and header_lines and has_include(header_lines, include_path):
                    continue
                includes_to_add.append(include_path)

        if not includes_to_add:
            return False

        # Find where to insert includes
        last_include_line = find_last_include_line(lines)

        if last_include_line == -1:
            # No includes found, add at the beginning after #pragma once or header guard
            insert_pos = 0
            for i, line in enumerate(lines):
                if '#pragma once' in line or '#ifndef' in line:
                    insert_pos = i + 1
                    break
        else:
            insert_pos = last_include_line + 1

        # Insert the includes
        for include_path in reversed(includes_to_add):
            lines.insert(insert_pos, f'#include "{include_path}"')

        # Write back to file
        with open(filepath, 'w', encoding='utf-8', newline='\n') as f:
            f.write('\n'.join(lines))

        print(f"Added {', '.join(includes_to_add)} to {filepath}")
        return True

    except Exception as e:
        print(f"Error processing {filepath}: {e}")
        return False

def main():
    """Main function to scan and process files"""
    modified_count = 0
    processed_count = 0

    # Process .hpp files first, then .cpp files
    # This ensures headers have includes before we check them for .cpp files
    for ext in ['*.hpp', '*.cpp']:
        for scan_dir in SCAN_DIRS:
            scan_path = Path(scan_dir)
            if not scan_path.exists():
                print(f"Warning: {scan_dir} does not exist, skipping...")
                continue

            for filepath in scan_path.rglob(ext):
                processed_count += 1
                if process_file(str(filepath)):
                    modified_count += 1

    print(f"\nProcessed {processed_count} files, modified {modified_count} files")

if __name__ == "__main__":
    main()
