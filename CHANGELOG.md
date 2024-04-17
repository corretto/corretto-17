# Change Log for Amazon Corretto 17

The following sections describe the changes for each release of Amazon Corretto 17.

## Corretto version: 17.0.11.9.1
Release Date: April 16, 2024
**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Alpine-based Linux, aarch64
+ Windows 10 or later, x86_64
+ macos 12.0 and later, x86_64
+ macos 12.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.11.9.1:

| Issue Name           | Platform | Description                                                                          | Link                                                                  |
|----------------------|----------|--------------------------------------------------------------------------------------|-----------------------------------------------------------------------|
| Import jdk-17.0.11+9 | All      | Update Corretto baseline to OpenJDK 17.0.11+9 | [17.0.11+9](https://github.com/openjdk/jdk17u/releases/tag/17.0.11+9)  |
| Fallback option for POST-only OCSP requests | All | Add option to fallback to old OCSP behavior to not unconditionally use GET requests for small requests | [JDK-8328638](https://bugs.openjdk.org/browse/JDK-8328638) |
| Shenandoah: SIGSEGV crash in unnecessary_acquire due to LoadStore split through phi | All | Fix SIGSEGV crash when using Shenandoah garbage collector | [JDK-8325372](https://bugs.openjdk.org/browse/JDK-8325372) |

The following CVEs are addressed in 17.0.11.9.1:

| CVE            | CVSS | Component                      |
|----------------|------|--------------------------------|
| CVE-2024-21012 | 3.7  | core-libs/java.net             |
| CVE-2024-21011 | 3.7  | hotspot/runtime                |
| CVE-2024-21068 | 3.7  | hotspot/compiler               |
| CVE-2024-21094 | 3.7  | hotspot/compiler               |


## Corretto version: 17.0.10.8.1
Release Date: February 8, 2024
**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Alpine-based Linux, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.10.8.1:

| Issue Name           | Platform | Description                                                                          | Link                                                                  |
|----------------------|----------|--------------------------------------------------------------------------------------|-----------------------------------------------------------------------|
| [AArch64] C1 compilation fails because monitor offset in OSR buffer is too large for immediate | All Linux      |Fixes regression from [JDK-8293671](https://bugs.openjdk.org/browse/JDK-8293671) | [JDK-8310844](https://bugs.openjdk.org/browse/JDK-8310844)  |
| AArch64: crypto pmull based CRC32/CRC32C intrinsics clobber V8-V15 registers | All Linux     |Fixes register clobber on aarch64 architetures| [JDK-8324874](https://bugs.openjdk.org/browse/JDK-8324874)                                         |

No CVEs are addressed in 17.0.10.8.1


## Corretto version: 17.0.10.7.1
Release Date: January 16, 2024
**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Alpine-based Linux, aarch64
+ Windows 10 or later, x86_64
+ macos 12.0 and later, x86_64
+ macos 12.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.10.7.1:

| Issue Name           | Platform | Description                                                                          | Link                                                                  |
|----------------------|----------|--------------------------------------------------------------------------------------|-----------------------------------------------------------------------|
| Import jdk-17.0.10+7 | All | Updates Corretto baseline to OpenJDK 17.0.10+7   | [jdk-17.0.10+7](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.10+7)                                 |
| Data loss in AVX3 Base64 decoding | All | Base64 appears to give different (wrong) results in some rare cases when AVX3 is enabled.| [JDK-8321599](https://bugs.openjdk.org/browse/JDK-8321599)|
| (tz) Update Timezone Data to 2023d | All | Update Timezone Data to 2023d |  [JDK-8322725](https://bugs.openjdk.org/browse/JDK-8322725)                        |
| NPE in PKCS7.parseOldSignedData    | All | Fixes exception PKCS7.parseOldSignedDat | [JDK-8315042](https://bugs.openjdk.org/browse/JDK-8315042)                                              |
|Enable Neoverse N1 optimizations for Neoverse V2| All      |Enable Neoverse N1 optimizations for Neoverse V2| [JDK-8321025](https://bugs.openjdk.org/browse/JDK-8321025)                      |
|Enable UseCryptoPmullForCRC32 for Neoverse V2| All      |Enable UseCryptoPmullForCRC32| [JDK-8321105](https://bugs.openjdk.org/browse/JDK-8321105)                                         |

The following CVEs are addressed in 17.0.10.7.1:

| CVE            | CVSS | Component                      |
|----------------|------|--------------------------------|
| CVE-2024-20932 | 7.5  | security-libs/java.security    |
| CVE-2024-20918 | 7.4  | hotspot/compiler               |
| CVE-2024-20952 | 7.4  | security-libs/java.security    |
| CVE-2024-20919 | 5.9  | hotspot/runtime                |
| CVE-2024-20921 | 5.9  | hotspot/compiler               |
| CVE-2024-20945 | 4.7  | security-libs/javax.xml.crypto |


## Corretto version: 17.0.9.8.1
Release Date: October 17, 2023

**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Alpine-based Linux, aarch64
+ Windows 10 or later, x86_64
+ macos 11.0 and later, x86_64
+ macos 11.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.9.8.1:

| Issue Name                          | Platform | Description                                                                          | Link                                                                        |
|-------------------------------------|----------|--------------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| Import jdk-17.0.9+8                | All    | Updates Corretto baseline to OpenJDK 17.0.9+8   | [jdk-17.0.9+8](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.9+8) |

The following CVEs are addressed in 17.0.9.8.1:

| CVE            | CVSS | Component                   |
|----------------|------|-----------------------------|
| CVE-2023-22081 | 5.3  | security-libs/javax.net.ssl |
| CVE-2023-22025 | 3.7  | hotspot/compiler            |

## Corretto version: 17.0.8.8.1
Release Date: August 22, 2023

**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Alpine-based Linux, aarch64
+ Windows 10 or later, x86_64
+ macos 11.0 and later, x86_64
+ macos 11.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.8.8.1:

|                             Issue Name                            | Platform |                                    Description                                     |                            Link                            |
|-------------------------------------------------------------------|----------|------------------------------------------------------------------------------------|------------------------------------------------------------|
| 8313765: Invalid CEN header (invalid zip64 extra data field size) | All      | Fix ZipException that may be encountered when opening select APK, ZIP or JAR files | [JDK-8313765](https://bugs.openjdk.org/browse/JDK-8313765) |


## Corretto version: 17.0.8.7.1
Release Date: July 18, 2023

**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Alpine-based Linux, aarch64
+ Windows 10 or later, x86_64
+ macos 11.0 and later, x86_64
+ macos 11.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.8.7.1:

| Issue Name                          | Platform | Description                                                                          | Link                                                                        |
|-------------------------------------|----------|--------------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| Import jdk-17.0.8+7                | All    | Updates Corretto baseline to OpenJDK 17.0.8+7   | [jdk-17.0.8+7](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.8+7) |
| Dynamic linking zlib               | All      | Dynamic linking zlib         | [#125](https://github.com/corretto/corretto-17/pull/125)                    |
| 8302483: Enhance ZIP performance          | All      | This release of OpenJDK includes stronger checks on the Zip64 fields of zip files. In the event that these checks cause failures on trusted zip files, the checks can be disabled by setting the new system property 'jdk.util.zip.disableZip64ExtraFieldValidation' to 'true'. |                                                                             |
| 8300596: Enhance Jar Signature validation | All      | A System property "jdk.jar.maxSignatureFileSize" is introduced to configure the maximum number of bytes allowed for the signature-related files in a JAR file during verification. The default value is 8000000 bytes (8 MB). |                                                                             |

The following CVEs are addressed in 17.0.8.7.1:

| CVE            | CVSS | Component                   |
|----------------|------|-----------------------------|
| CVE-2023-22041 | 5.1  | hotspot/compiler |
| CVE-2023-25193 | 3.7  | client-libs/2d |
| CVE-2023-22044 | 3.7  | hotspot/compiler |
| CVE-2023-22045 | 3.7  | hotspot/compiler |
| CVE-2023-22049 | 3.7  | core-libs/java.io |
| CVE-2023-22036 | 3.7  | core-libs/java.util |
| CVE-2023-22006 | 3.1  | core-libs/java.net |

## Corretto version: 17.0.7.7.1
Release Date: April 18, 2023

**Target Platforms <sup>1</sup>**

+ RPM-based Linux using glibc 2.12 or later, x86, x86_64
+ Debian-based Linux using glibc 2.12 or later, x86, x86_64
+ RPM-based Linux using glibc 2.17 or later, aarch64
+ Debian-based Linux using glibc 2.17 or later, aarch64
+ Alpine-based Linux, x86_64
+ Alpine-based Linux, aarch64
+ Windows 10 or later, x86_64
+ macos 11.0 and later, x86_64
+ macos 11.0 and later, aarch64

**1.** This is the platform targeted by the build. See [Using Amazon Corretto](https://aws.amazon.com/corretto/faqs/#Using_Amazon_Corretto)
in the Amazon Corretto FAQ for supported platforms

The following issues are addressed in 17.0.7.7.1:

| Issue Name                          | Platform | Description                                                                          | Link                                                                          |
|-------------------------------------|----------|--------------------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| Import jdk-17.0.7+7                | All    | Updates Corretto baseline to OpenJDK 17.0.7+7   | [jdk-17.0.7+7](https://github.com/openjdk/jdk17u/releases/tag/jdk-17.0.7+7)                                                                                                 |
| Tz data                            | All    | All tzdata updates up to 2023c                  | [#117](https://github.com/corretto/corretto-17/pull/117)                                                                                                                    |
| Update amazon cacerts              | All    | Update amazon cacerts file from amazonlinux     | [#113](https://github.com/corretto/corretto-17/pull/113) [#115](https://github.com/corretto/corretto-17/pull/115) [#116](https://github.com/corretto/corretto-17/pull/116)  |

The following CVEs are addressed in 17.0.7.7.1:

| CVE            | CVSS | Component                   |
|----------------|------|-----------------------------|
| CVE-2023-21930 | 7.4  | security-libs/javax.net.ssl |
| CVE-2023-21954 | 5.9  | hotspot/gc                  |
| CVE-2023-21967 | 5.9  | security-libs/javax.net.ssl |
| CVE-2023-21939 | 5.3  | client-libs/javax.swing     |
| CVE-2023-21938 | 3.7  | core-libs/java.lang         |
| CVE-2023-21937 | 3.7  | core-libs/java.net          |
| CVE-2023-21968 | 3.7  | core-libs/java.nio          |


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
