#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2025 Thomas@chriesibaum.dev

VENV_DIR="/tmp/cb_jtag_probe_testing/.venv"

if ! command -v uv >/dev/null 2>&1; then
    echo "uv is required but not installed. Install it from https://docs.astral.sh/uv/ and re-run this script."
    exit 1
fi

if [ -d "$VENV_DIR" ]; then
    echo "Activating virtual environment..."
    source "$VENV_DIR/bin/activate"
else
    echo "Virtual environment not found. Let's create it first."
    uv venv "$VENV_DIR"

    source "$VENV_DIR/bin/activate"

    uv pip install --upgrade pip
    uv pip install -r requirements.txt
    echo "Virtual environment setup complete and ready to use."
fi


