import subprocess
import sys

def run_command(command):
    try:
        subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT)
        return True
    except subprocess.CalledProcessError:
        return False

def get_commits(branch):
    try:
        # Get commits that are in branch but not in dev
        output = subprocess.check_output(f"git log --reverse --pretty=format:'%H' dev..{branch}", shell=True).decode('utf-8')
        return [c.strip() for c in output.splitlines() if c.strip()]
    except subprocess.CalledProcessError:
        return []

def main():
    try:
        output = subprocess.check_output("git branch -r | grep upstream-pr", shell=True).decode('utf-8')
    except subprocess.CalledProcessError:
        print("No upstream-pr branches found.")
        return

    branches = [b.strip() for b in output.splitlines()]

    success_branch_count = 0
    fail_branch_count = 0

    for branch in branches:
        print(f"Processing {branch}...")
        commits = get_commits(branch)
        if not commits:
            print(f"  No unique commits found for {branch} (already merged or invalid base?)")
            continue

        print(f"  Found {len(commits)} commits to cherry-pick.")
        branch_success = True

        for commit in commits:
            # Cherry-pick with 'theirs' strategy to resolve simple conflicts favor of the PR
            # But standard cherry-pick doesn't have -X theirs easily without conflict?
            # actually -X theirs works with -m 1 if it's a merge, but these are likely normal commits.
            # We use --strategy-option=theirs
            if run_command(f"git cherry-pick --strategy-option=theirs {commit}"):
                print(f"    Picked {commit[:8]}")
            else:
                print(f"    Failed to pick {commit[:8]}, aborting branch processing...")
                run_command("git cherry-pick --abort")
                branch_success = False
                break

        if branch_success:
            print(f"  Successfully processed {branch}")
            success_branch_count += 1
        else:
            print(f"  Failed to process {branch}")
            fail_branch_count += 1

    print(f"Finished. Success: {success_branch_count}, Failed: {fail_branch_count}")

if __name__ == "__main__":
    main()
