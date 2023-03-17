# Change Log for Amazon Corretto 17

The following sections describe the changes for each release of Amazon Corretto 17.

## Corretto version: 17.0.6.10.1
Release Date: January 17, 2023

**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Windows 10 or later, x86_64
+ macos 10.15 and later, x86_64
+ macos 11.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.6.10.1:

| Issue Name                          | Platform | Description                                                                          | Link                                                                          |
|-------------------------------------|----------|--------------------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| Import jdk-17.0.6+10                | All      | Updates Corretto baseline to OpenJDK 17.0.6+10                                       | [jdk-17.0.6+10](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.6+10) |
| Fix provides for public shared libs | AL       | All tzdata updates up to 2022e                                                       | [#108](https://github.com/corretto/corretto-17/pull/108)                      |
| Fix java_home alternative           | Linux    | Alternative dir without architecture should be created on headless package           | [#106](https://github.com/corretto/corretto-17/pull/106)                      |                                                                                                                 |
| Add "corretto.gtest" property       | All      | The new "corretto.gtest" property will be used to pass the path to the gtest library | [#104](https://github.com/corretto/corretto-17/pull/104)                      |                                                                                                                 |
| Include commitId.txt in archives    | All      | Include commitId.txt in archives                                                     | [#103](https://github.com/corretto/corretto-17/pull/103)                      |
| Update amazon cacerts               | All      | Update amazon cacerts file from amazonlinux                                          |                                                                               |
| Relax VerifyCACerts                 | All      | Relax VerifyCACerts expiry condition                                                 | [#101](https://github.com/corretto/corretto-17/pull/101)                      |                                                                                                                 |

The following CVEs are addressed in 17.0.6.10.1:

| CVE            | CVSS | Component                   |
|----------------|------|-----------------------------|
| CVE-2023-21835 | 5.3  | security-libs/javax.net.ssl |
| CVE-2023-21843 | 3.7  | client-libs/javax.sound     |


## Corretto version: 17.0.5.8.1
Release Date: October 18, 2022

**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Windows 7 or later, x86_64
+ macos 10.15 and later, x86_64
+ macos 11.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.5.8.1:

| Issue Name                                       | Platform    | Description                                                                          | Link                                                                          |
|--------------------------------------------------|-------------|--------------------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| Import jdk-17.0.5+8                              | All         | Updates Corretto baseline to OpenJDK 17.0.5+8                                        | [jdk-17.0.5+8](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.5%2B8) |
| Update Timezone Data to 2022e                    | All         | All tzdata updates up to 2022e                                                       | [#97](https://github.com/corretto/corretto-17/pull/97) [#99](https://github.com/corretto/corretto-17/pull/99) |
| Add jpeg, alsa and fonts as headless dependencies| Amazon Linux| Add libraries that could be used in headless mode to RPM dependecies                 | [#94](https://github.com/corretto/corretto-17/pull/94) |
| Update amazon cacerts                            | All         | Update amazon cacerts file from amazonlinux                                          | |

The following CVEs are addressed in 17.0.5.8.1:

| CVE            | CVSS | Component                   |
|----------------|------|-----------------------------|
| CVE-2022-21618 | 5.3  | security-libs/org.ietf.jgss |
| CVE-2022-21628 | 5.3  | core-libs/java.net          |
| CVE-2022-39399 | 3.7  | core-libs/java.net          |
| CVE-2022-21619 | 3.7  | security-libs/java.security |
| CVE-2022-21624 | 3.7  | core-libs/javax.naming      |


## Corretto version: 17.0.4.9.1
Release Date: August 15, 2022


**Target Platforms**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Windows 7 or later, x86_64
+ macos 10.15 and later, x86_64
+ macos 11.0 and later, aarch64

The following issues are addressed in 17.0.4.9.1

| Issue Name                                       | Platform    | Description                                                                          | Link                                                                          |
|--------------------------------------------------|-------------|--------------------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| Import jdk-17.0.4.1+0                              | All         | Updates Corretto baseline to OpenJDK 17.0.4.1+0                                        | [jdk-17.0.4.1+0](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.4.1%2B0) |
| Resolve C2 compiler crash                              | All         | [JDK-8279219](https://bugs.openjdk.org/browse/JDK-8279219) caused regressions in the OpenJDK 11.0.16 and OpenJDK 17.0.4 releases and we are backing it out. See [JDK-8291665](https://bugs.openjdk.org/browse/JDK-8291665).                                       | [JDK-8292260](https://bugs.openjdk.org/browse/JDK-8292260) |


## Corretto version: 17.0.4.8.2
Release Date: July 25, 2022

**Target Platforms**

+ Amazon Linux 2022, x86_64
+ Amazon Linux 2022, aarch64

The following issues are addressed in 17.0.4.8.2

| Issue Name                                       | Platform    | Description                                                     | Link |
|--------------------------------------------------|-------------|-----------------------------------------------------------------|------|
| Add provides java-devel                          | AL2/AL2022  | Adding provides java-devel as required by 3p packages           |      |



## Corretto version: 17.0.4.8.1
Release Date: July 19, 2022

**Target Platforms**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Windows 7 or later, x86_64
+ macos 10.15 and later, x86_64
+ macos 11.0 and later, aarch64

The following issues are addressed in 17.0.4.8.1

| Issue Name                                       | Platform    | Description                                                                          | Link                                                                          |
|--------------------------------------------------|-------------|--------------------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| Import jdk-17.0.4+8                              | All         | Updates Corretto baseline to OpenJDK 17.0.4+8                                        | [jdk-17.0.4+8](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.4%2B8) |
| Fix src.rpm name that some tools depend on       | Linux       | Updates src.rpm name                                                                 | [#67](https://github.com/corretto/corretto-17/pull/67) [#72](https://github.com/corretto/corretto-17/pull/72) |
| Migrate pkg builds to productbuild from packages | macOS       | Updates to macos packaging                                                           | [#68](https://github.com/corretto/corretto-17/pull/68) |
| Only require log4j-cve-mitigations on AL2        | AL2         | Updates log4j-cve-mitigations to AL2 only                                            | [#71](https://github.com/corretto/corretto-17/pull/71) |
| The documentation update                         | All         | Updates to code of conduct and contributing documentation                            | [#73](https://github.com/corretto/corretto-17/pull/73) |
| AL2022 updates                                   | AL2, AL2022 | Updates to support Corretto in Amazon Linux 2022                                     | [#75](https://github.com/corretto/corretto-17/pull/75) [#79](https://github.com/corretto/corretto-17/pull/79) [#80](https://github.com/corretto/corretto-17/pull/80) [#81](https://github.com/corretto/corretto-17/pull/81) [#82](https://github.com/corretto/corretto-17/pull/82) |
| Enable bundled zlib library via Gradle           | macOS       | Updates to use bundled (not the system) version of the zlib library on macOS aarch64 | [#76](https://github.com/corretto/corretto-17/pull/76) [#78](https://github.com/corretto/corretto-17/pull/78) |
| Update amazon cacerts                            | All         | Update amazon cacerts file from amazonlinux                                          | [#77](https://github.com/corretto/corretto-17/pull/77) |


 The following CVEs are addressed in 17.0.4.8.1

| CVE            | CVSS | Component           |
|----------------|------|---------------------|
| CVE-2022-34169 | 7.5  | xml/jaxp            |
| CVE-2022-21541 | 5.9  | hotspot/runtime     |
| CVE-2022-21549 | 5.3  | core-libs/java.util |
| CVE-2022-21540 | 5.3  | hotspot/compiler    |

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
 | Import jdk-17.0.3+6 | All | Updates Corretto patch set to OpenJDK 17.0.3+6 | [jdk-17.0.3+6](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.3%2B6)
 | Fix alternatives for devel package | Amazon Linux 2 | Fix alternatives for devel package | [PR #59](https://github.com/corretto/corretto-17/pull/59) |
 | Fix missing libLingeredApp.so failing jtregs | All | Fix jtreg tier 1 test failures caused by UnsatisfiedLinkError | [PR #50](https://github.com/corretto/corretto-17/pull/50) |
 | Update CACerts | All | Pull in the latest CA Certificates from Amazon Linux | [PR #64](https://github.com/corretto/corretto-17/pull/64) |
 | Update zlib | All | CVE-2018-25032: based upon our analysis, OpenJDK/Corretto is not affected by CVE-2018-25032, because the zlib “memLevel” parameter is not settable and is fixed at 8, and the usage of the Z_FIXED strategy is prevented. With these settings there is no way to invoke the issue described in the CVE and we only include this fix out of an abundance of caution. | |

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
