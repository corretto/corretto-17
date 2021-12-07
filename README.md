## Corretto JDK

Amazon Corretto is a no-cost, multiplatform,
production-ready distribution of the Open Java Development Kit (OpenJDK).
Corretto is used internally at Amazon for production services.
With Corretto, you can develop and run Java applications
on operating systems such as Linux, Windows, and macOS.

Documentation is available at [https://docs.aws.amazon.com/corretto](https://docs.aws.amazon.com/corretto).

### Licenses and Trademarks

Please read these files: "LICENSE", "ADDITIONAL_LICENSE_INFO", "ASSEMBLY_EXCEPTION", "TRADEMARKS.md".

### Branches
:warning: _generational-shenandoah_ :warning:

This branch adds a young generation to the Shenandoah garbage collector. Although this branch is _experimental_, we are making these nightly builds available for expanded testing and additional feedback. Please don't put these builds into production (yet).

| Platform | Download Link | Checksum | Sig File |
| :--------|:--------------|:---------|:---------|
| Linux x86\_64 | [amazon-corretto-17.0.1.12.2-684a794107-linux-x64.tar.gz](https://corretto.aws/downloads/resources/17.0.1.12.2/amazon-corretto-17.0.1.12.2-684a794107-linux-x64.tar.gz) | c50b25f7b70494ebceaf1252d58e388d | [Download](https://corretto.aws/downloads/resources/17.0.1.12.2/amazon-corretto-17.0.1.12.2-684a794107-linux-x64.tar.gz.sig) |
| Linux aarch64 | [amazon-corretto-17.0.1.12.2-684a794107-linux-aarch64.tar.gz](https://corretto.aws/downloads/resources/17.0.1.12.2/amazon-corretto-17.0.1.12.2-684a794107-linux-aarch64.tar.gz) | 23a7f13bbd50cd8018a576c4d15259d5 | [Download](https://corretto.aws/downloads/resources/17.0.1.12.2/amazon-corretto-17.0.1.12.2-684a794107-linux-aarch64.tar.gz.sig) |

To activate the young generation, you must change the gc mode with: `-XX:+UseShenandoahGC -XX:ShenandoahGCMode=generational`.


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
