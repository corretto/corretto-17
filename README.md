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
| Linux x86\_64 | [amazon-corretto-17.0.1.12.1-06f6f03f6a-linux-x64.tar.gz](https://corretto.aws/downloads/resources/17.0.1.12.1/amazon-corretto-17.0.1.12.1-06f6f03f6a-linux-x64.tar.gz) | 9ea4b240f22cd8d94c1d7d77f2200606 | [Download](https://corretto.aws/downloads/resources/17.0.1.12.1/amazon-corretto-17.0.1.12.1-06f6f03f6a-linux-x64.tar.gz.sig) |
| Linux aarch64 | [amazon-corretto-17.0.1.12.1-06f6f03f6a-linux-aarch64.tar.gz](https://corretto.aws/downloads/resources/17.0.1.12.1/amazon-corretto-17.0.1.12.1-06f6f03f6a-linux-aarch64.tar.gz) | 770ec251da2f79e36d70d41bb68c188b | [Download](https://corretto.aws/downloads/resources/17.0.1.12.1/amazon-corretto-17.0.1.12.1-06f6f03f6a-linux-aarch64.tar.gz.sig) |

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
