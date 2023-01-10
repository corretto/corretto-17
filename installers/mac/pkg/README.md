### Instruction for generating Amazon Corretto installer

#### Prerequisites

The generation of the Corretto installer depends on an open-source utility tool [Packages](http://s.sudre.free.fr/Software/Packages/about.html).
The application must be installed before running the installer generation task.

There are two options to generate a Corretto installer.

#### Option 1: Build Corretto
* Under the root directory of the repository, run
```
./gradlew :installers:mac:pkg:generateInstaller
```

#### Option 2: Use pre-built Corretto artifacts
* Set the environment variable "CORRETTO_ARTIFACTS_PATH" to the path of the pre-built Corretto artifacts.
```
export CORRETTO_ARTIFACTS_PATH=.../path/to/amazon-corretto-<version>.jdk
```
* Under the root directory of the repository, run
```
./gradlew :installers:mac:pkg:generateInstaller
```

The installer will be generated under "<repo-root>/installers/mac/pkg/corretto-build/distributions".
