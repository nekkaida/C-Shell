#!/usr/bin/env python3
"""
Code analyzer for C Shell project.

This script analyzes the codebase for potential issues, coding style
violations, and provides metrics about the code.
"""

import os
import re
import sys
import subprocess
from collections import defaultdict, Counter

# ANSI colors
class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_header(text):
    """Print a formatted header."""
    print(f"\n{Colors.HEADER}{Colors.BOLD}{text}{Colors.ENDC}")

def print_issue(severity, message, file=None, line=None):
    """Print a formatted issue message."""
    color = Colors.RED if severity == "ERROR" else Colors.YELLOW if severity == "WARNING" else Colors.BLUE
    location = f"{file}:{line}: " if file and line else ""
    print(f"{color}{severity}{Colors.ENDC}: {location}{message}")

def find_c_files(directory):
    """Find all C source and header files in the directory."""
    c_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.c', '.h')):
                c_files.append(os.path.join(root, file))
    return c_files

def check_file_length(file_path):
    """Check if a file is too long."""
    with open(file_path, 'r') as f:
        lines = f.readlines()
    
    if len(lines) > 500:
        print_issue("WARNING", f"File is too long ({len(lines)} lines)", file_path)
    
    return len(lines)

def check_function_length(file_path):
    """Check for functions that are too long."""
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Simple regex to find function definitions
    function_pattern = r'(\w+\s+)+(\w+)\s*\([^)]*\)\s*\{'
    
    functions = []
    current_pos = 0
    
    for match in re.finditer(function_pattern, content):
        func_name = match.group(2)
        start_pos = match.end()
        
        # Find the end of the function (matching curly brace)
        brace_count = 1
        end_pos = start_pos
        
        while brace_count > 0 and end_pos < len(content):
            if content[end_pos] == '{':
                brace_count += 1
            elif content[end_pos] == '}':
                brace_count -= 1
            end_pos += 1
        
        func_content = content[start_pos:end_pos]
        func_lines = func_content.count('\n') + 1
        
        functions.append((func_name, func_lines))
        
        if func_lines > 50:
            # Find approximate line number
            line_num = content[:start_pos].count('\n') + 1
            print_issue("WARNING", f"Function '{func_name}' is too long ({func_lines} lines)", file_path, line_num)
    
    return functions

def check_long_lines(file_path):
    """Check for lines longer than 80 characters."""
    long_lines = []
    with open(file_path, 'r') as f:
        for i, line in enumerate(f, 1):
            if len(line.rstrip('\n')) > 80:
                long_lines.append(i)
                print_issue("WARNING", f"Line exceeds 80 characters ({len(line.rstrip())})", file_path, i)
    
    return long_lines

def check_todo_comments(file_path):
    """Find TODO comments."""
    todos = []
    with open(file_path, 'r') as f:
        for i, line in enumerate(f, 1):
            if re.search(r'(TODO|FIXME|XXX)', line, re.IGNORECASE):
                todos.append((i, line.strip()))
                print_issue("INFO", f"Found TODO comment: '{line.strip()}'", file_path, i)
    
    return todos

def check_naming_conventions(file_path):
    """Check for naming convention violations."""
    issues = []
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Check for function names (should be snake_case)
    function_pattern = r'(\w+)\s+(\w+)\s*\([^)]*\)\s*\{'
    for match in re.finditer(function_pattern, content):
        func_name = match.group(2)
        if not re.match(r'^[a-z][a-z0-9_]*$', func_name) and func_name != 'main':
            line_num = content[:match.start()].count('\n') + 1
            print_issue("WARNING", f"Function '{func_name}' should use snake_case", file_path, line_num)
            issues.append((line_num, func_name))
    
    # Check for constants (should be UPPER_CASE)
    define_pattern = r'#define\s+([A-Za-z0-9_]+)'
    for match in re.finditer(define_pattern, content):
        constant_name = match.group(1)
        if not re.match(r'^[A-Z][A-Z0-9_]*$', constant_name):
            line_num = content[:match.start()].count('\n') + 1
            print_issue("WARNING", f"Constant '{constant_name}' should use UPPER_CASE", file_path, line_num)
            issues.append((line_num, constant_name))
    
    return issues

def check_memory_management(file_path):
    """Check for potential memory management issues."""
    issues = []
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Check for malloc without free
    malloc_pattern = r'(\w+)\s*=\s*malloc\('
    malloc_vars = []
    for match in re.finditer(malloc_pattern, content):
        var_name = match.group(1)
        malloc_vars.append(var_name)
    
    free_pattern = r'free\s*\(\s*(\w+)\s*\)'
    free_vars = []
    for match in re.finditer(free_pattern, content):
        var_name = match.group(1)
        free_vars.append(var_name)
    
    # Simple check for variables that might not be freed
    for var in malloc_vars:
        if var not in free_vars and not (var + '_' in content or '_' + var in content):
            print_issue("WARNING", f"Potential memory leak: '{var}' allocated but might not be freed", file_path)
            issues.append(var)
    
    # Check for unchecked malloc
    unchecked_malloc_pattern = r'(\w+)\s*=\s*malloc\([^;]*;(?!\s*if\s*\(\s*!\s*\1|\s*if\s*\(\s*\1\s*==\s*NULL)'
    for match in re.finditer(unchecked_malloc_pattern, content):
        var_name = match.group(1)
        line_num = content[:match.start()].count('\n') + 1
        print_issue("ERROR", f"Unchecked malloc of '{var_name}'", file_path, line_num)
        issues.append((line_num, var_name))
    
    return issues

def check_error_handling(file_path):
    """Check for potential issues with error handling."""
    issues = []
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Check for system calls without error checking
    system_calls = ['open', 'close', 'read', 'write', 'fork', 'exec', 'dup', 'dup2']
    for call in system_calls:
        pattern = r'(\w+)\s*=\s*{}[^;]*;(?!\s*if\s*\(\s*\1)'.format(call)
        for match in re.finditer(pattern, content):
            var_name = match.group(1)
            line_num = content[:match.start()].count('\n') + 1
            print_issue("WARNING", f"Unchecked system call: '{call}' result stored in '{var_name}'", file_path, line_num)
            issues.append((line_num, call, var_name))
    
    return issues

def run_cppcheck(file_path):
    """Run cppcheck on the file if available."""
    try:
        result = subprocess.run(['cppcheck', '--enable=all', '--suppress=missingIncludeSystem', file_path], 
                               capture_output=True, text=True)
        if result.stderr:
            print_header(f"Cppcheck results for {file_path}")
            for line in result.stderr.splitlines():
                if ': error:' in line:
                    print_issue("ERROR", line.split(': error:')[1], file_path)
                elif ': warning:' in line:
                    print_issue("WARNING", line.split(': warning:')[1], file_path)
                elif ': style:' in line:
                    print_issue("INFO", line.split(': style:')[1], file_path)
    except FileNotFoundError:
        print("cppcheck not found, skipping static analysis")

def collect_metrics(file_path):
    """Collect various metrics about the code."""
    with open(file_path, 'r') as f:
        content = f.read()
    
    metrics = {}
    
    # Count lines
    metrics['total_lines'] = content.count('\n') + 1
    
    # Count comment lines (simplistic approach)
    comment_lines = len(re.findall(r'^\s*//.*$|^\s*/\*.*?\*/\s*$|^\s*\*.*$', content, re.MULTILINE))
    metrics['comment_lines'] = comment_lines
    
    # Count blank lines
    blank_lines = len(re.findall(r'^\s*$', content, re.MULTILINE))
    metrics['blank_lines'] = blank_lines
    
    # Count code lines
    metrics['code_lines'] = metrics['total_lines'] - metrics['comment_lines'] - metrics['blank_lines']
    
    # Count functions
    function_pattern = r'(\w+\s+)+(\w+)\s*\([^)]*\)\s*\{'
    functions = re.findall(function_pattern, content)
    metrics['function_count'] = len(functions)
    
    # Count variables
    var_pattern = r'(int|char|float|double|long|short|unsigned|void|bool|size_t)\s+([a-zA-Z_][a-zA-Z0-9_]*)[^(]'
    variables = re.findall(var_pattern, content)
    metrics['variable_count'] = len(variables)
    
    return metrics

def analyze_file(file_path):
    """Analyze a single file for issues."""
    print_header(f"Analyzing {file_path}")
    
    # Run various checks
    check_file_length(file_path)
    check_function_length(file_path)
    check_long_lines(file_path)
    check_todo_comments(file_path)
    check_naming_conventions(file_path)
    check_memory_management(file_path)
    check_error_handling(file_path)
    
    # Run external tools if available
    run_cppcheck(file_path)
    
    # Collect metrics
    metrics = collect_metrics(file_path)
    
    print(f"\n{Colors.GREEN}Metrics for {file_path}:{Colors.ENDC}")
    print(f"  Total lines: {metrics['total_lines']}")
    print(f"  Code lines: {metrics['code_lines']}")
    print(f"  Comment lines: {metrics['comment_lines']} ({metrics['comment_lines']/max(1, metrics['total_lines'])*100:.1f}%)")
    print(f"  Blank lines: {metrics['blank_lines']}")
    print(f"  Functions: {metrics['function_count']}")
    print(f"  Variables: {metrics['variable_count']}")
    
    return metrics

def analyze_project(directory):
    """Analyze the entire project."""
    c_files = find_c_files(directory)
    
    if not c_files:
        print(f"{Colors.RED}No C files found in {directory}{Colors.ENDC}")
        return
    
    print(f"{Colors.GREEN}Found {len(c_files)} C files to analyze{Colors.ENDC}")
    
    total_metrics = defaultdict(int)
    file_metrics = {}
    
    for file_path in c_files:
        metrics = analyze_file(file_path)
        file_metrics[file_path] = metrics
        
        for key, value in metrics.items():
            total_metrics[key] += value
    
    print_header("Project Summary")
    print(f"Total files: {len(c_files)}")
    print(f"Total lines of code: {total_metrics['code_lines']}")
    print(f"Total functions: {total_metrics['function_count']}")
    print(f"Average code lines per file: {total_metrics['code_lines'] / len(c_files):.1f}")
    print(f"Average functions per file: {total_metrics['function_count'] / len(c_files):.1f}")
    
    # Find files with the most code
    print_header("Files with the most code")
    sorted_files = sorted(file_metrics.items(), key=lambda x: x[1]['code_lines'], reverse=True)
    for file_path, metrics in sorted_files[:5]:
        print(f"{file_path}: {metrics['code_lines']} lines of code, {metrics['function_count']} functions")

def main():
    """Main entry point."""
    if len(sys.argv) > 1:
        directory = sys.argv[1]
    else:
        directory = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'src')
    
    print(f"{Colors.HEADER}{Colors.BOLD}C Shell Code Analyzer{Colors.ENDC}")
    print(f"Analyzing directory: {directory}")
    
    analyze_project(directory)

if __name__ == "__main__":
    main()