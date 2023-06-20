# Generating documentation

## Requirements

- [Python 3.6+](https://www.python.org/downloads/)
- [Sphinx](https://www.sphinx-doc.org/en/master/usage/installation.html)
- [Sphinx RTD theme](https://sphinx-rtd-theme.readthedocs.io/en/stable/installing.html)
- [Doxygen](https://www.doxygen.nl/download.html)
- [Doxysphinx](https://pypi.org/project/doxysphinx/)
- Sphinx-multiversion - install our [fork](https://github.com/flipperdevices/sphinx-multiversion/tree/hleb/submodules) from the `hleb/submodules` branch, otherwise it won't work with submodules.
- [Myst-parser](https://pypi.org/project/myst-parser/)

## Building documentation

First, make sure to have all the submodules checked out:

```bash
git submodule update --init --recursive
```

Then, run the following command to build the Doxygen part of the documentation:

```bash
# Assuming you are in the root of the repository
cd flipperzero-documentation
doxygen Doxyfile-awesome.cfg
```

This will generate the Doxygen documentation in the `doxygen` folder.

Then, run the following command to convert the Doxygen files to reStructuredText:

```bash
# Assuming you are in the flipperzero-documentation folder
doxysphinx build doxygen/html sphinx_out/ Doxyfile-awesome.cfg
```

If there are any changes to the existing .rst files, you'll need to commit them for sphinx-multiversion to find them. It will not find uncommitted changes.

Finally, run the following command to build the HTML documentation:

```bash
# Assuming you are in the flipperzero-documentation folder
cd ../ # Go back to the root of the repository
sphinx-multiversion flipperzero-documentation flipperzero-documentation/sphinx_out/
```

The HTML documentation will be generated in the `flipperzero-documentation/sphinx_out/<BRANCH_NAME>` folder. To make the Doxygen documentation prettier, you can run doxysphinx once again over the results, it'll generate additional CSS files that will make the documentation look better, for example:

```bash
# Assuming you are in the flipperzero-documentation folder
doxysphinx build doxygen/html sphinx_out/<BRANCH_NAME>/doxygen/html/ Doxyfile-awesome.cfg
```

This needs to be done for every branch, as the CSS files are generated inside the branch folder.

## Adding new pages

To add a new page, create a new .md file in the `flipperzero-documentation/docs` folder. Then, add the filename to the `flipperzero-documentation/docs/index.rst` file, so that it appears in the sidebar. You can look at the existing files for examples.
