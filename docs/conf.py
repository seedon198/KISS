# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'KISS Fuzzer'
copyright = '2025, KISS Fuzzer Team'
author = 'KISS Fuzzer Team'
release = '1.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.viewcode',
    'sphinx.ext.napoleon',
    'sphinx.ext.intersphinx',
    'sphinx.ext.todo',
    'sphinx.ext.coverage',
    'sphinx.ext.mathjax',
    'myst_parser',
    'sphinx_copybutton',
    'sphinxcontrib.mermaid',
    'breathe'
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output ------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'furo'
html_static_path = ['_static']

# Theme options
html_theme_options = {
    "sidebar_hide_name": True,
    "light_logo": "kiss-logo.svg",
    "dark_logo": "kiss-logo.svg",
    "source_repository": "https://github.com/your-username/KISS-Fuzzer/",
    "source_branch": "main",
    "source_directory": "docs/",
}

html_title = "KISS Fuzzer Documentation"
html_short_title = "KISS Fuzzer"

# -- Extension configuration -------------------------------------------------

# Napoleon settings
napoleon_google_docstring = True
napoleon_numpy_docstring = True
napoleon_include_init_with_doc = False
napoleon_include_private_with_doc = False
napoleon_include_special_with_doc = True
napoleon_use_admonition_for_examples = False
napoleon_use_admonition_for_notes = False
napoleon_use_admonition_for_references = False
napoleon_use_ivar = False
napoleon_use_param = True
napoleon_use_rtype = True

# MyST settings
myst_enable_extensions = [
    "colon_fence",
    "deflist",
    "dollarmath",
    "fieldlist",
    "html_admonition",
    "html_image",
    "replacements",
    "smartquotes",
    "strikethrough",
    "substitution",
    "tasklist",
]

# Breathe configuration (for C code documentation)
breathe_projects = {"KISS Fuzzer": "_build/doxygen/xml"}
breathe_default_project = "KISS Fuzzer"

# Copy button configuration
copybutton_prompt_text = r">>> |\.\.\. |\$ |In \[\d*\]: | {2,5}\.\.\.: | {5,8}: "
copybutton_prompt_is_regexp = True

# Intersphinx mapping
intersphinx_mapping = {
    'python': ('https://docs.python.org/3/', None),
    'numpy': ('https://numpy.org/doc/stable/', None),
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# Copy the logo to static files
import os
import shutil

def setup(app):
    # Copy logo from res/img to docs/_static
    src_logo = os.path.join(os.path.dirname(os.path.dirname(__file__)), 'res', 'img', 'kiss-logo.svg')
    dst_dir = os.path.join(os.path.dirname(__file__), '_static')
    os.makedirs(dst_dir, exist_ok=True)
    if os.path.exists(src_logo):
        shutil.copy2(src_logo, dst_dir)
