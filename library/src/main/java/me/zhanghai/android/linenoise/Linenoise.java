/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package me.zhanghai.android.linenoise;

import android.os.Process;
import android.system.Os;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class Linenoise {
    private static final String LIBRARY_NAME = "linenoise-jni";

    static {
        switch (Os.getuid()) {
            case Process.ROOT_UID:
            case Process.SHELL_UID:
                // Processes running as root or shell cannot automatically find native libraries.
                break;
            default:
                System.loadLibrary(LIBRARY_NAME);
        }
    }

    private Linenoise() {}

    @NonNull
    public static String getLibraryName() {
        return LIBRARY_NAME;
    }

    @Nullable
    public static native String readLine(@NonNull String prompt);

    public static native void setMultiLine(boolean multiLine);

    public static native boolean addHistory(@NonNull String line);

    public static native boolean setHistoryMaxLength(int length);

    public static native boolean saveHistory(@NonNull String path);

    public static native boolean loadHistory(@NonNull String path);

    public static native void setMaskModeEnabled(boolean enabled);

    public static native void setCompleter(@Nullable Completer completer);

    public static native void setHinter(@Nullable Hinter hinter);

    public static native void clearScreen();
}
