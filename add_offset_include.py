import os
import re

def find_files_with_get_offset(root_dir):
    """Find all files containing GET_OFFSET"""
    files = []
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.endswith(('.cpp', '.hpp', '.h')):
                filepath = os.path.join(dirpath, filename)
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                        if 'GET_OFFSET' in content:
                            files.append(filepath)
                except:
                    pass
    return files

def has_include(filepath, include_str):
    """Check if file has the specific include"""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            return include_str in content
    except:
        return False

def add_include(filepath, include_str):
    """Add include to file if not present"""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()

        # Find the position to insert (after last #include or at top)
        insert_pos = 0
        for i, line in enumerate(lines):
            if line.strip().startswith('#include'):
                insert_pos = i + 1

        # Insert the include
        lines.insert(insert_pos, f'{include_str}\n')

        with open(filepath, 'w', encoding='utf-8') as f:
            f.writelines(lines)

        return True
    except Exception as e:
        print(f"Error adding include to {filepath}: {e}")
        return False

def main():
    root_dir = r'D:\Documents\New Github\dll\src'
    include_str = '#include "Utils/Memory/Game/SignatureAndOffsetManager.hpp"'

    # Find all files with GET_OFFSET
    files = find_files_with_get_offset(root_dir)

    print(f"Found {len(files)} files with GET_OFFSET\n")

    files_without_include = []
    files_with_include = []

    for filepath in files:
        if has_include(filepath, include_str):
            files_with_include.append(filepath)
        else:
            files_without_include.append(filepath)

    print(f"Files WITH include: {len(files_with_include)}")
    print(f"Files WITHOUT include: {len(files_without_include)}\n")

    if files_without_include:
        print("Files missing the include:")
        for filepath in files_without_include:
            print(f"  - {filepath}")

        print("\nAdding include to files...")
        for filepath in files_without_include:
            if add_include(filepath, include_str):
                print(f"  [+] Added to {os.path.basename(filepath)}")
            else:
                print(f"  [x] Failed to add to {os.path.basename(filepath)}")

        print(f"\nDone! Added include to {len(files_without_include)} files.")
    else:
        print("All files already have the include!")

if __name__ == "__main__":
    main()
