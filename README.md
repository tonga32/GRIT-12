# GRIT-12

**GRIT-12** is an original JUCE-based audio effect inspired by the workflow of classic 12-bit samplers and vintage low-pass filters. It is not affiliated with waveTracing, Akai, or E-mu, and it does not contain SP950 code, presets, graphics, or other proprietary assets.

The project is configured to build a Windows **VST3** and a Standalone app with CMake. GitHub Actions compiles both and publishes them as a workflow artifact.

## Current controls

- **RPM / Detune:** changes the effective decimation rate while keeping the host playback duration unchanged. The readout translates the detune amount to a virtual RPM value.
- **Range -15 / -30:** selects the available detune span.
- **Fine:** changes the detune step from semitones to tenths of a semitone.
- **S950 Filter:** a three-section Butterworth low-pass (6th order). At 99 the filter is bypassed.
- **Bit Depth:** 6 to 16 bits, default 12 bits.
- **Input / Output:** independent gain controls, each from -12 dB to +12 dB.
- **Gain Link:** compensates output gain against input gain to keep the overall level more stable while changing the input drive.
- **Routing:** Stereo, Mono Sum, Mono L, Mono R, Mid, or Side.
- **Wet / Dry:** output blend.

## Build it on GitHub

1. Create a new empty repository on GitHub.
2. Extract this project ZIP and upload its contents to the repository root. The `.github/workflows/build-windows.yml` file must remain in that exact location.
3. Commit the files to the `main` branch (or `master`).
4. Open the repository's **Actions** tab. The workflow **Build GRIT-12 VST3** starts automatically after the push. You can also start it manually with **Run workflow**.
5. When the run finishes, open it and download the artifact named **GRIT-12-Windows-VST3**. It contains the VST3 bundle and Standalone build.

The first run downloads JUCE 9.0.4 during the CMake configure step, so the GitHub runner needs network access.

## Build locally on Windows

Requirements:

- Windows 10/11 64-bit
- Visual Studio 2022 with the **Desktop development with C++** workload
- CMake 3.22 or newer
- Git

From a Developer PowerShell or terminal where CMake and Visual Studio are available:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
```

The VST3 bundle is created under:

```text
build/GRIT12_artefacts/Release/VST3/GRIT-12.vst3
```

## Notes

- The initial DSP is a real-time sample-and-hold decimator followed by quantisation and a 6th-order low-pass. It is an original approximation of the broad sonic workflow described above, not a bit-for-bit model of the SP-1200 or S950 hardware.
- The mono and Mid/Side routing modes are implemented in the wet path; the dry path remains the original input for the Wet/Dry control.
- Test the plugin at moderate levels first. The input gain can drive the quantiser into hard digital clipping by design.
- JUCE is fetched from its official repository at the fixed `9.0.4` tag. JUCE is separately licensed under AGPLv3 or the commercial JUCE licence. If you plan to distribute a closed-source binary, review JUCE's current licence options before release.

## License

The original GRIT-12 source files are provided under the MIT License; see `LICENSE`. This does not change the separate licence terms that apply to JUCE or its bundled dependencies.
