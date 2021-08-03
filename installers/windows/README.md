## Instruction of Building Corretto on Windows

Corretto on Windows includes one subproject.

The `:zip` project builds and packages Corretto archive as zip file. Before executing
this project, you need to provide the following parameters:

```$xslt
bootjdk_dir         # Path of bootstrap JDK. To build Corretto, a JDK of the same or previous version is required.

ucrt_dll_dir        # Path of ucrt library directory.

jtreg_dir           # Path of Jtreg.

vcruntime_dir       # Path of the latest vcruntime140.dll
```

To execute this, run: `./gradlew :installers:windows:zip:build` at `corretto-<version>` root directory.
The zip archive is located at `/installers/windows/zip/corretto-build/distributions`

```$xslt
➜ ./gradlew :installers:windows:zip:build \
            -Pbootjdk_dir=... \
            -Pucrt_dll_dir=... \
            -Pjtreg_dir=... \
            -Pvcruntime_dir=...
➜ tree installers/windows/zip/corretto-build/distributions
   installers/windows/zip/corretto-build/distributions
   └── unsigned-jdk-image.zip
```
