# Change Log for Amazon Corretto 17

The following sections describe the changes for each release of Amazon Corretto 17.

## Corretto version: 17.0.3.6.1

 Release Date: April 17, 2022

 **Target Platforms**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Windows 7 or later, x86_64
+ macos 10.13 and later, x86_64
+ macos 11.0 and later, aarch64

 The following issues are addressed in 17.0.3.6.1

 | Issue Name | Platform | Description | Link |
 | --- | --- | --- | --- |
 | Import jdk-17.0.3+5 | All | Updates Corretto patch set to OpenJDK 17.0.3+6 | [jdk-17.0.3+6](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.3%2B6)
 | Fix alternatives for devel package | Amazon Linux 2 | Fix alternatives for devel package | [PR #59](https://github.com/corretto/corretto-17/pull/59) |
 | Fix missing libLingeredApp.so failing jtregs | All | Fix jtreg tier 1 test failures caused by UnsatisfiedLinkError | [PR #50](https://github.com/corretto/corretto-17/pull/50) |

 The following CVEs are addressed in 17.0.3.6.1

 | CVE | CVSS | Component |
 | --- | --- | --- |
 | CVE-2022-21449 | 7.5 | security-libs/java.security |
 | CVE-2022-21496 | 5.3 | core-libs/javax.naming |
 | CVE-2022-21434 | 5.3 | core-libs/java.lang |
 | CVE-2022-21426 | 5.3 | xml/jaxp |
 | CVE-2022-21443 | 3.7 | security-libs/java.security |

## Corretto version: 17.0.2.8.1

 Release Date: January 18, 2022

 **Target Platforms**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Windows 7 or later, x86_64
+ macos 10.13 and later, x86_64
+ macos 11.0 and later, aarch64

 The following issues are addressed in 17.0.2.8.1

 | Issue Name | Platform | Description | Link |
 | --- | --- | --- | --- |
 | Import jdk-17.0.2+8 | All | Updates Corretto patch set to OpenJDK 17.0.2+8 | [jdk-17.0.2+8](https://github.com/openjdk/jdk17/releases/tag/jdk-17.0.2%2B8)
 | Use SHA256 hashes in RPM File Digest | RPM-based Linux | Update plugins to support FIPS compliant file digests in generated RPMs | [#344](https://github.com/corretto/corretto-8/issues/344) |
 | Sign Mac bundle | Macos | The Macos tgz contained signed files, but was not signed as a bundle. | |
 | Update Amazon CACerts | All | Pull in the latest CA Certificates from Amazon Linux | |

 The following CVEs are addressed in 17.0.2.8.1

 | CVE | CVSS | Component |
 | --- | --- | --- |
 | CVE-2022-21341 | 5.3 | core-libs/java.io:serialization |
 | CVE-2022-21365 | 5.3 | client-libs/javax.imageio |
 | CVE-2022-21282 | 5.3 | xml/jaxp |
 | CVE-2022-21291 | 5.3 | hotspot/runtime |
 | CVE-2022-21277 | 5.3 | client-libs/javax.imageio |
 | CVE-2022-21305 | 5.3 | hotspot/compiler |
 | CVE-2022-21299 | 5.3 | xml/jaxp |
 | CVE-2022-21296 | 5.3 | xml/jaxp |
 | CVE-2022-21283 | 5.3 | core-libs/java.util |
 | CVE-2022-21340 | 5.3 | security-libs/java.security |
 | CVE-2022-21293 | 5.3 | core-libs/java.lang |
 | CVE-2022-21294 | 5.3 | core-libs/java.util |
 | CVE-2022-21360 | 5.3 | client-libs/javax.imageio |
 | CVE-2022-21366 | 5.3 | client-libs/javax.imageio |
 | CVE-2022-21248 | 3.7 | core-libs/java.io:serialization |

## Corretto version: 17.0.1.12.2

 Release Date: November 19, 2021

 **Target Platforms**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64

 The following issues are addressed in 17.0.1.12.2

 | Issue Name | Platform | Description | Link |
 | --- | --- | --- | --- |
 | Corretto-17#19 | AmazonLinux2 | Adding RPMs for Amazon Linux 2. | [Correto-17#19](https://github.com/corretto/corretto-17/issues/19)|


## Corretto version: 17.0.1.12.1

 Release Date: October 19, 2021

 **Target Platforms**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Windows 7 or later, x86_64
+ macos 10.13 and later, x86_64
+ macos 11.0 and later, aarch64

 The following issues are addressed in 17.0.1.12.1

 | Issue Name | Platform | Description | Link |
 | --- | --- | --- | --- |
 | Debug symbols | All | Separate tar for zipped debug symbols in release build. | N/A |
 | Debug symbols | All | Separate lib debug symbols into separate tar/zip. | N/A |
 | 8273902: Memory leak in OopStorage due to bug in OopHandle::release() | All | Fix the memory leak in OopStorage introduced in the JDK 16 | [JDK-8273902](https://bugs.openjdk.java.net/browse/JDK-8273902) |

 The following CVEs are addressed in 17.0.1.12.1

 | CVE | CVSS | Component |
 | --- | --- | --- |
 | CVE-2021-35567  | 6.8 | security-libs/java.security |
 | CVE-2021-35550  | 5.9 | security-libs/javax.net.ssl |
 | CVE-2021-35586  | 5.3 | client-libs/javax.imageio |
 | CVE-2021-35564  | 5.3 | security-libs/java.security |
 | CVE-2021-35561  | 5.3 | core-libs/java.util |
 | CVE-2021-35565  | 5.3 | core-libs/java.net |
 | CVE-2021-35559  | 5.3 | client-libs/javax.swing |
 | CVE-2021-35578  | 5.3 | security-libs/javax.net.ssl |
 | CVE-2021-35556  | 5.3 | client-libs/javax.swing |
 | CVE-2021-35603  | 3.7 | security-libs/javax.net.ssl |
 | CVE-2021-35588  | 3.1 | hotspot/runtime |

## Corretto version: 17.0.0.35.2

Release Date: 09-15-2021

**Target Platforms**

+  macOS 10.13 and later, x86_64, aarch64

The following issues are addressed in 17.0.0.35.2.

| Issue Name | Platform | Description | Link |
| --- | --- | --- | --- |
| Corretto-17#11 Regression related to JDK-8246094: [macos] Sound Recording and playback is not working | Macos | Use Info.plist from upstream | [Corretto-17#11](https://github.com/corretto/corretto-17/issues/11)

## Corretto version: 17.0.0.35.1

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
