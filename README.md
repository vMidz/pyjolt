### WIP

# Installation Guide


To install the package, run:

```bash
cd <project-directory>
pip install .
```

During development, you may want incremental builds. Otherwise, pip would rebuild the entire package from scratch on each invocation.

Alternatively:
```bash
pip install --no-build-isolation -ve .
```

If you use vscode you can create a task to start the build process:
```json
{
    "version": "2.0.0",
    "tasks": [
      {
        "label": "build",
        "type": "shell",
        "command": "cmd",
        "args": [
          "/c",
          "build.bat && pip install --no-build-isolation -ve ."
        ],
        "options": {
          "env": {
            "VENV_PATH": "C:\\path_to_venv\\.venv"
          }
        },
        "group": {
          "kind": "build",
          "isDefault": true
        },
        "problemMatcher": []
      },
    ]
}
```

To run the samples app run:
```bash
pip install -r requirements.txt
cd tests/samples/main.py
```


