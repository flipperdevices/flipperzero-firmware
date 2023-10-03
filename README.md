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
**Apps made by the Xtreme team are just kept as code in the main branch. Others instead come from elsewhere and need tweaks applied.**
Due to how Git works we can't just make edits in a submodule and publish it as a signle repo like that.
Our changes need to be committed and tracked somewhere, but we didn't want to have a fork repo for each app.
Instead we use **fork *branches*:tm:** where the **code for the apps is in branches** of this same repo and the **main branch has submodules to other branches** this same repo.
As for pulling updates from the upstream repositories, remotes are not kept when pushing and cloning, so we use a modular system:
- set it up by running `git config --local include.path ../.gitremotes` (this makes your git config import the file `.gitremotes`)
- each app branch has a `.gitremotes` file which contains configuration for its `upstream` remote
- this also has an alias for merging upstream updates with the correct ref by `git merge-upstream`
