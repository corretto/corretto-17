## Corretto JDK

Amazon Corretto is a no-cost, multiplatform,
production-ready distribution of the Open Java Development Kit (OpenJDK).
Corretto is used internally at Amazon for production services.
With Corretto, you can develop and run Java applications
on operating systems such as Linux, Windows, and macOS.

This repository is used to track [OpenJDK upstream tip](https://github.com/openjdk/jdk).
Please look at the branches section for more information on Feature Releases.

The latest binary Corretto 17 release builds can be downloaded from https://github.com/corretto/corretto-17/releases.

Documentation is available at [https://docs.aws.amazon.com/corretto](https://docs.aws.amazon.com/corretto).

### Licenses and Trademarks

Please read these files: "LICENSE", "ADDITIONAL_LICENSE_INFO", "ASSEMBLY_EXCEPTION", "TRADEMARKS.md".

### Branches

_develop_
: The default branch. The branch that consumes development and patches to upstream jdk17u. Corretto builds are generated from this branch.

_upstream-jdk17_
: The branch is similar to master at [openjdk/jdk17](https://github.com/openjdk/jdk17). This branch merges into release branches.

_upstream-jdk17u_
: The branch is similar to master at [openjdk/jdk17u](https://github.com/openjdk/jdk17u). This branch merges into develop.

### OpenJDK Readme
```

Welcome to the JDK!
===================

For build instructions, please see either of these files:

  * doc/building.html   (html version)
  * doc/building.md     (markdown version)

See https://openjdk.java.net/ for more information about
the OpenJDK Community and the JDK.
```
