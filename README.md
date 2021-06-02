# RA_fuzzing
fuzzing algorithm, C with fuzzing book<br>
- 20-1 Research Internship
- Personal Project with [Professor](https://github.com/shinhong)

## Summary
I performed the task of changing the **python code** of the **[fuzzing book](<https://www.fuzzingbook.org/>)** to the **C language**.
This process allows me to learn the fuzzing algorithm.
The code of the fuzzing book is implemented as an object-oriented algorithm, so I overcomed the limitations of the c language using the structure of the c language.
After completing this process, I conducted a fuzzing test using [Google/fuzzer-test-suite](https://github.com/google/fuzzer-test-suite) with [Anora](https://github.com/AngoraFuzzer/Angora), [AFL](https://github.com/google/AFL), and [LibFuzzer](https://llvm.org/docs/LibFuzzer.html) and performed a comparative analysis of the differences and performance of each fuzzer.
The comparison data will be uploaded later.

## Comparison(draft)🔧
### Target Program
- guetzli
- libarchive
- lcms
- libjpeg-turbo
- Harfbuzz
#### Reason
- Google/fuzzer-test-suite : 24
- Condition
  - Seeds - 12
  - Not too big to angora dry run - 11
  - the libfuzzing engine not too tightly attached to the target program – 8
  - Can't attach Gcov -5
### Compare Graph (draft, some results per module)
This project has not been terminated. Consequently, results and information cannot be fully disclosed.<br>
- Orenge: Angora
- Blue: AFL
<p align="center"><img width="514" alt="image" src="https://user-images.githubusercontent.com/28642467/120481152-c9c7a500-c3ea-11eb-81bf-9b169e6c77be.png"></p>
<p align="center">Figure 1. Guetzli jpeg_data_reader</p>

<p align="center"><img width="514" alt="image" src="https://user-images.githubusercontent.com/28642467/120481437-11e6c780-c3eb-11eb-948a-300b5f5dbbf3.png"></p>
<p align="center">Figure 2. harfbuzz hp-op-shape</p>

<p align="center"><img width="514" alt="image" src="https://user-images.githubusercontent.com/28642467/120481529-27f48800-c3eb-11eb-9565-efce157859bc.png"></p>
<p align="center">Figure 3. Lcms cmsio0</p>

<p align="center"><img width="514" alt="image" src="https://user-images.githubusercontent.com/28642467/120481619-3e9adf00-c3eb-11eb-9b5b-324a29e4f90b.png"></p>
<p align="center">Figure 4. Libarchive archive_read</p>

<p align="center"><img width="514" alt="image" src="https://user-images.githubusercontent.com/28642467/120481702-57a39000-c3eb-11eb-8498-c59c62f00067.png"></p>
<p align="center">Figure 5. Libjpeg jdmarker </p>
