# eSpeak Text-to-Speech for Android™

[![eSpeak for Android on Google Play](https://developer.android.com/images/brand/en_app_rgb_wo_45.png "eSpeak for Android on Google Play")](https://play.google.com/store/apps/details?id=com.reecedunn.espeak)

- [Build Dependencies](#build-dependencies)
  - [Debian](#debian)
- [Building](#building)
  - [Building with Gradle](#building-with-gradle)
  - [Building with Eclipse](#building-with-eclipse)
  - [Signing the APK](#signing-the-apk)
- [Installing](#installing)
  - [Enabling eSpeak on the Device](#enabling-espeak-on-the-device)
- [Bugs](#bugs)
- [License Information](#license-information)

----------

This branch contains the files needed to build espeak on the android platform.
It is derived from the eyes-free project.

## Build Dependencies

In order to build eSpeak, you need:

1.  a functional autotools system (`make`, `autoconf`, `automake`, `libtool`
    and `pkg-config`);
2.  a functional c++ compiler;
3.  wxWidgets development libraries (needed to build and run espeakedit
    to compile the phoneme data).

In order to build the APK, you need:

1.  the [Android SDK](http://developer.android.com/sdk/index.html);
2.  the [Android NDK](http://developer.android.com/tools/sdk/ndk/index.html);
3.  Gradle 1.10.

In order to use Android Studio, you will also need:

1.  [Android Studio](http://developer.android.com/sdk/installing/studio.html).

In order to use Eclipse, you will also need:

1.  Eclipse;
2.  the [Android Developer Tools](http://developer.android.com/tools/sdk/eclipse-adt.html) (ADT) for Eclipse.

### Debian

| Dependency    | Install                                                          |
|---------------|------------------------------------------------------------------|
| autotools     | `sudo apt-get install make autoconf automake libtool pkg-config` |
| c++ compiler  | `sudo apt-get install gcc g++`                                   |
| wxWidgets     | `sudo apt-get install libwxgtk2.8-dev`                           |
| gradle        | [gradle-1.10_1.0-0ubuntu1_all.deb](https://launchpad.net/~cwchien/+archive/gradle/+files/gradle-1.10_1.0-0ubuntu1_all.deb) |

## Building

### Building with Gradle

1.  Set the location of the Android SDK:

2.  Build the project:

        $ ./autogen.sh
        $ ./configure
        $ make

This will create an `android/build/apk/espeak-release-unsigned.apk` file.

### Building with Eclipse

1.  Build the JNI binding and espeak data file by running:

        $ ./autogen.sh
        $ ./configure
        $ make jni espeakdata
2.  Open Eclipse.
3.  Create a new workspace.
4.  Import the espeak folder as an exising Android project.
5.  Build the espeak apk within Eclipse.

The generated `eSpeakActivity.apk` can be installed like any other apk build
via eclipse, such as by using the `Run` menu option.

### Signing the APK

In order to install the built APK (e.g. `bin/eSpeakActivity-release-unsigned.apk`)
you need to self-sign the package. You can do this by:

1.  Creating a certificate, if you do not already have one:

        $ keytool -genkey -keystore [YOUR_CERTIFICATE] -alias [ALIAS]
2. Sign the package using your certificate:

        $ jarsigner -sigalg MD5withRSA -digestalg SHA1 \
          -keystore [YOUR_CERTIFICATE] \
          bin/eSpeakActivity-release-unsigned.apk [ALIAS]
3. Align the apk using the zipalign tool.

        $ zipalign 4 bin/eSpeakActivity-release-unsigned.apk \
          bin/eSpeakActivity-release-signed.apk

## Installing

Now, you can install the APK using the `adb` tool:

After running, `eSpeakActivity` will extract the `espeakdata.zip` file into its
own data directory to set up the available voices.

To enable eSpeak, you need to:

1.  go into the Android `Text-to-Speech settings` UI;
2.  enable `eSpeak TTS` in the `Engines` section;
3.  select `eSpeak TTS` as the default engine;
4.  use the `Listen to an example` option to check if everything is working.

## Bugs

Report bugs to the [espeak issues](https://github.com/rhdunn/espeak/issues)
page on GitHub.

## License Information

eSpeak Text-to-Speech is released under the GPL version 3 or later license.

The eSpeak Android APK code in the `android` folder is released under the
Apache 2.0 license.

Android and Google Play are registered trademarks of Google Inc.

All trademarks are property of their respective owners.