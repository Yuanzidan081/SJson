# SJson
a json parser base on json-tutorial[json-tutorial](https://github.com/miloyip/json-tutorial)(written by C++)。

# project organization
+ src: the source file
  + Json: Store and Get the JsonValue parsed by JsonParser. 
  + JsonException: Exception handing class if there is any error when parse the json string.
  + JsonParser: Parse the json format string to the JsonValue.
  + JsonGenerator: Stringfy the string to the json format.
  + JsonValue: Manage the json parsed object(null, true, false, number, string, array, object)
+ dep: Test Framework: GoogleTest
+ test: unit test using GoogleTest

# class description


# how to start
```
git clone --recursive https://github.com/Yuanzidan081/SJson.git
# update the GoogleTest module as the latest version
git submodule update --remote 
```

I use the VSCode as my code editor, if you also use this, you should define the json files in the folder called .vscode. I use the MSVC as my compiler, if you want to change it, you can add `-G XXX(like MinGW Makefiles)`in the task.json (args).

task.json
```
{
    "tasks": [
        {
            "type": "shell",
            "label": "generate",
            "options": {
                "cwd": "${workspaceFolder}"
            },
            "command": "cmake", 
            "args": [
                "-S",
                ".",
                "-B",
                "build"
            ],
        },
        {
            "label": "complie",
            "options": {
                "cwd": "${workspaceFolder}"
            },
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "command": "cmake",
            "args": [
                "--build",
                "build"
            ],
        },
        {
            "label": "preparation",
            "dependsOn": [
                "generate",
                "complie"
            ]
        }
    ],
    "version": "2.0.0"
}
```

launch.json
```
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "launch SRendererLab",
            "type": "cppvsdbg",
            "request": "launch",
            //"program": "${workspaceFolder}/build/test/Debug/SJsonGTest.exe",
            "program": "${workspaceFolder}/build/test/Debug/SJsonTest.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "console": "externalTerminal",
            "preLaunchTask": "preparation"
        }
    ]
}
```

Press F5, then you can see the test results.
