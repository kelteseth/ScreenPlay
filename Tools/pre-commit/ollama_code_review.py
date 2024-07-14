# File: Tools/pre-commit/ollama_code_review.py

import sys
import asyncio
import subprocess
import re
from typing import Optional, Sequence, List, Tuple
from ollama import AsyncClient

MODEL_NAME = 'glm4'  # You can change this to any model you prefer
ANALYSIS_TIMEOUT = 60  # Timeout in seconds for each file analysis

def get_git_staged_diff() -> str:
    """Get the git diff of staged changes."""
    try:
        result = subprocess.run(['git', 'diff', '--cached'], capture_output=True, text=True, check=True)
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error getting git diff: {e}")
        sys.exit(1)

def parse_git_diff(diff: str) -> List[Tuple[str, str]]:
    """Parse git diff and return a list of (filename, file_diff) tuples."""
    file_diffs = []
    current_file = None
    current_diff = []

    for line in diff.split('\n'):
        if line.startswith('diff --git'):
            if current_file:
                file_diffs.append((current_file, '\n'.join(current_diff)))
            current_file = re.search(r'b/(.+)$', line).group(1)
            current_diff = [line]
        elif current_file:
            current_diff.append(line)

    if current_file:
        file_diffs.append((current_file, '\n'.join(current_diff)))

    return file_diffs


async def analyze_file_with_ollama(filename: str, file_diff: str) -> None:
    """Send a single file diff to Ollama for analysis and stream the response."""
    prompt = f"""
    Analyze the following git diff for the file '{filename}'. 

    Focus only on:
    1. Copy-paste errors: Look for actual repeated code blocks or patterns that suggest copy-pasting, ignoring the '+' and '-' at the start of lines.
    2. Spelling issues: Identify any misspelled words or typos in comments and strings. Ignore variable names, function names, and other code identifiers.

    Use the following emoji system to indicate the severity of issues:
    🟢 - No issues found
    🟡 - Minor issues (e.g., a few typos or small repeated code segments)
    🔴 - Significant issues (e.g., large blocks of copied code or numerous spelling errors)

    Start your response with one of these emojis to indicate the overall status.
    Then provide a concise summary of findings, if any. If no issues are found, just state that.

    Git diff for {filename}:
    {file_diff}
    """

    try:
        client = AsyncClient()
        print(f"\nAnalyzing {file_diff}:")
        
        full_response = []
        async def process_stream():
            async for response in await client.generate(model=MODEL_NAME, prompt=prompt, stream=True):
                chunk = response['response']
                full_response.append(chunk)
                print(chunk, end='', flush=True)

        await asyncio.wait_for(process_stream(), timeout=ANALYSIS_TIMEOUT)
        print()  # New line after complete response
    except asyncio.TimeoutError:
        print(f"\nAnalysis for {filename} timed out after {ANALYSIS_TIMEOUT} seconds.")
    except Exception as e:
        print(f"\nError communicating with Ollama for {filename}: {e}")

async def check_and_pull_model() -> bool:
    """Check if the model exists, and pull it if it doesn't."""
    client = AsyncClient()
    try:
        # Check if the model exists
        models = await client.list()
        if MODEL_NAME not in [model['name'] for model in models['models']]:
            print(f"Model '{MODEL_NAME}' not found. Pulling it now...")
            await client.pull(model=MODEL_NAME)
            print(f"Model '{MODEL_NAME}' has been pulled successfully.")
        return True
    except Exception as e:
        print(f"Error checking or pulling the model: {e}")
        return False

async def main(argv: Optional[Sequence[str]] = None) -> int:
    # First, check and pull the model if necessary
    if not await check_and_pull_model():
        print("Failed to ensure the model is available. Exiting.")
        return 1

    full_diff = get_git_staged_diff()
    if not full_diff:
        print("No staged changes to analyze.")
        return 0

    file_diffs = parse_git_diff(full_diff)
    
    for filename, file_diff in file_diffs:
        await analyze_file_with_ollama(filename, file_diff)

    # Ask user if they want to proceed with the commit
    while True:
        answer = input("\nDo you want to proceed with the commit? (y/n): ").lower()
        if answer in ['y', 'yes']:
            return 0  # Proceed with commit
        elif answer in ['n', 'no']:
            return 1  # Abort commit
        else:
            print("Please answer 'y' or 'n'.")

if __name__ == "__main__":
    sys.exit(asyncio.run(main()))