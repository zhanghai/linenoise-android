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

package me.zhanghai.android.linenoise.sample;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Looper;

import java.io.File;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.Collections;
import java.util.Objects;

import androidx.annotation.NonNull;
import me.zhanghai.android.linenoise.Hint;
import me.zhanghai.android.linenoise.Linenoise;

public class Main {
    @SuppressLint({ "PrivateApi", "UnsafeDynamicallyLoadedCode" })
    public static void main(@NonNull String[] args) throws Exception {
        Thread.setDefaultUncaughtExceptionHandler((_1, e) -> e.printStackTrace());

        final Class<?> ddmHandleAppNameClass = Class.forName("android.ddm.DdmHandleAppName");
        final Method ddmHandleAppNameSetAppName = ddmHandleAppNameClass.getMethod("setAppName",
                String.class, int.class);
        ddmHandleAppNameSetAppName.invoke(null, BuildConfig.APPLICATION_ID + ":shell", 0);

        Looper.prepareMainLooper();
        final Class<?> activityThreadClass = Class.forName("android.app.ActivityThread");
        final Method activityThreadSystemMainMethod = activityThreadClass.getMethod("systemMain");
        final Object activityThread = activityThreadSystemMainMethod.invoke(null);
        final Method activityThreadGetSystemContextMethod = activityThreadClass.getMethod(
                "getSystemContext");
        final Context systemContext = (Context) activityThreadGetSystemContextMethod.invoke(
                activityThread);

        final String libraryDirectory = systemContext.getPackageManager().getApplicationInfo(
                BuildConfig.APPLICATION_ID, 0).nativeLibraryDir;
        final String libraryPath = new File(libraryDirectory, "lib" + Linenoise.getLibraryName()
                + ".so").getPath();
        System.load(libraryPath);

        Linenoise.setMultiLine(true);
        Linenoise.setCompleter(input -> input.startsWith("h") ?
                Arrays.asList("hello", "hello there") : Collections.emptyList());
        Linenoise.setHinter(input -> Objects.equals(input, "git remote add") ?
                new Hint(" <name> <url>", 35, false) : null);
        String line;
        while ((line = Linenoise.readLine("linenoise> ")) != null) {
            System.out.println(line);
            Linenoise.addHistory(line);
        }
    }
}
