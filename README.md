# linenoise-android

[![Android CI status](https://github.com/zhanghai/linenoise-android/workflows/Android%20CI/badge.svg)](https://github.com/zhanghai/linenoise-android/actions)

[`Linenoise`](https://github.com/antirez/linenoise) built with Android NDK, packaged as an Android library with Java binding.

This is not an officially supported Google product.

## Why linenoise-android?

Because `TCSAFLUSH` is broken on Android 8.0 - 11 and this broke all the line editing libraries I looked into. This library patched `Linenoise` to use `TCSADRAIN` instead, which is also what the Android bulit-in `mksh` is using, so that it can run on all Android versions. For more details about this issue, you may refer to [my blog post](https://blog.zhanghai.me/fixing-line-editing-on-android-8-0/).

## Integration

Gradle:

```gradle
implementation 'me.zhanghai.android.linenoise:library:1.0.2'
```

## Usage

See [`Linenoise.java`](library/src/main/java/me/zhanghai/android/linenoise/Linenoise.java).

## License

    Copyright 2020 Google LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
