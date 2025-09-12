#!/usr/bin/env python3
"""
CI Release utility script for GitLab CI/CD.
Generates release information based on git tags.
"""

import sys
import os
import argparse
from pathlib import Path


def is_prerelease_tag(tag):
    """
    Determine if a git tag represents a pre-release version.
    Returns True for tags containing: alpha, beta, rc
    
    Examples:
    - v1.0.0-alpha4 -> True
    - v1.0.0-beta2 -> True
    - v1.0.0-rc1 -> True
    - v1.0.0 -> False
    """
    if not tag:
        return False
    
    # Convert to lowercase for case-insensitive matching
    tag_lower = tag.lower()
    
    # Check for alpha, beta, or rc
    return 'alpha' in tag_lower or 'beta' in tag_lower or 'rc' in tag_lower


def get_release_info(tag):
    """
    Generate release name and description based on tag.
    
    Args:
        tag: Git tag string (e.g., v1.0.0-alpha4, v1.0.0, etc.)
    
    Returns:
        tuple: (release_name, release_description)
    """
    is_prerelease = is_prerelease_tag(tag)
    
    if is_prerelease:
        release_name = f"🚧 ScreenPlay {tag}"
        
        # Set description based on release type
        tag_lower = tag.lower()
        if "alpha" in tag_lower:
            release_description = "🚧 Alpha snapshot for early testing and feature evaluation while ScreenPlay is still being worked on."
        elif "beta" in tag_lower:
            release_description = "🚧 Beta snapshot for public testing to catch bugs before the stable release."
        elif "rc" in tag_lower:
            release_description = "🚧 Release candidate focusing on finalizing the release and fixing remaining bugs."
        else:
            release_description = "🚧 Pre-release version for testing purposes."
    else:
        release_name = f"ScreenPlay {tag} Released!"
        release_description = "🎉 A Wild ScreenPlay Release Appeared!"
    
    return release_name, release_description


def main():
    parser = argparse.ArgumentParser(description="Generate release information for GitLab CI/CD")
    parser.add_argument("--name", action="store_true", help="Output release name")
    parser.add_argument("--description", action="store_true", help="Output release description")
    parser.add_argument("--is-pre-release", action="store_true", help="Output true/false for prerelease")
    
    args = parser.parse_args()
    
    # Get the tag from environment variable (set by GitLab CI)
    tag = os.environ.get('CI_COMMIT_TAG')
    
    if not tag:
        print("Error: CI_COMMIT_TAG environment variable not found", file=sys.stderr)
        sys.exit(1)
    
    # Generate release information
    release_name, release_description = get_release_info(tag)
    is_prerelease = is_prerelease_tag(tag)
    
    # Output based on requested flag
    if args.name:
        print(release_name)
    elif args.description:
        print(release_description)
    elif args.is_pre_release:
        print("true" if is_prerelease else "false")
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == "__main__":
    main()