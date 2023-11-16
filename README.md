# Xtreme-Apps
Bundle of external apps tweaked for Xtreme

### Why?
Apps we use for Xtreme are all modified (some lots more than others). This includes:
- Removing duplicate icons that are part of firmware to **support our Asset Packs system**
- Removing duplicate keyboard implementations to **use our extended system keyboard**
- With our system keyboard also **support our CLI command `input keyboard`** to type with PC keyboard
- **Moving location of save files** or changing how they are saved
- **Changing application display names** to fit our scheme
- **Changing how some menus work/look** or adding **new exclusive menus and features**
- **Improving or extending functionality** and better integrating with Xtreme firmware
- **Updating and fixing apps** that were abandoned by the original developers

### How?
**Apps made by the Xtreme team are kept as plain code here. For all others we use git subtrees to pull updates from elsewhere but also keep our own tweaks.**
We didn't want to have fork repos for each single app since it would get out of hand very quick. Instead, we opted for subtrees.

Subtrees work in a very peculiar way, where they pull and compare commit history from a remote repo and apply it to a subdirectory of this repo.
That's why the commit history for our repo is so huge, it contains all the commits for all the apps, plus our edits.

To make updating more manageable, we have added another layer on top of subtrees:
- set it up by running `git config --local include.path ../.utils/gitconfig`
- add a new app with `git add-subtree <path> <repo url> <branch> [subdir]`, this will pull the history and create `path/.gitsubtree` to remember the url, branch and subdir
- run `git update-subtree` to pull updates based on the current subtree directory
- or run `git bulk-update-subtrees` to do it for all subtrees
