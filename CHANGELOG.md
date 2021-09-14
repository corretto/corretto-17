# Change Log for Amazon Corretto 17

The following sections describe the changes for each release of Amazon Corretto 17.

Release Date: 09-15-2021

**Target Platforms**

+  macOS 10.13 and later, x86_64, aarch64

The following issues are addressed in 17.0.0.35.2.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Corretto-17#11 Regression related to JDK-8246094: [macos] Sound Recording and playback is not working | Macos | Use Info.plist from upstream | [Corretto-17#11](https://github.com/corretto/corretto-17/issues/11)

Release Date: 08-23-2021

**Target Platforms**

+  RPM-based Linux using glibc 2.12 or later, x86, x86_64
+  Debian-based Linux using glibc 2.12 or later, x86, x86_64
+  RPM-based Linux using glibc 2.17 or later, aarch64
+  Debian-based Linux using glibc 2.17 or later, aarch64
+  Alpine-based Linux, x86_64
+  Windows 7 or later, x86_64
+  macOS 10.13 and later, x86_64, aarch64

The following issues are addressed in 17.0.0.35.1.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Import jdk-17+35 | All | Updates Corretto baseline to OpenJDK 17+35 | [jdk-17+35](https://github.com/openjdk/jdk17/releases/tag/jdk-17%2B35)
| 8270317: Large Allocation in CipherSuite | All | Reduce allocation and improve performance in CipherSuite class | [JDK-8270317](https://bugs.openjdk.java.net/browse/JDK-8270317)
| 8268427: Improve AlgorithmConstraints:checkAlgorithm performance | All | Use Efficient method to check if algorithm is disabled while establishing TLS connections | [JDK-8268427](https://bugs.openjdk.java.net/browse/JDK-8268427)
