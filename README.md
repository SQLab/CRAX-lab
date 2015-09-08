# CRAX lab
**CRAX** 是 SQLab 所發展的 Automatic Exploit Generator (AEG)  
**CRAX** 的原理是透過 symbolic execution 判斷 eip 是否能被 input tainted  
如果一旦偵測到被 tainted 到  
會將目前收集到的 path constraint 加上 shellcode constraint  
以 SMT solver 嘗試去解可能的 input  

**CRAX** 採用的 symbolic execution engine 是基於 KLEE 更改來的 **s2e**  
**s2e** 是 **select symbolic execution** 的縮寫  
只對特定的程式區間做 symbolic execution  
改進 symbolic exection 指數成長的缺點  
可以支援 x86, x64, arm 平台  
詳細可以參考: [https://s2e.epfl.ch/](https://s2e.epfl.ch/)  

* * *
host & guest 的帳密皆為 `ais3 / crax`
* * *

## lab 1-1
**lab 1-1** 將介紹利用 **s2e** 提供的 api 對程式 make symbolic 的流程  
[sample.c](http://123.tw) 是一個很簡單的 buffer overflow 的小程式  
其中第 12 行 `s2e_make_concolic` 將對某一段 buffer make symbolic  
當 guest 執行到第 12 行就會開始進行 symbolic execution, 直到程式結束  
本實驗將展示透過 **CRAX** 自動去生成 shellcode  

1. 開啟 vm (host) 後, 在 `~/crax/script/result` 底下啟動 **CRAX**  
 - `crax.sh crax.s2e demo` (host)
2. 啟動後會用 qemu 開啟一個 debian 的 vm (guest), 在 `~/sample/` 底下執行跑 `run.sh`  
 - `cd sample && ./run.sh` (guest)
3. host 上的 **CRAX** 會開始生 exploit, exploit 會在 `~/crax/script/result/s2e-last` 底下  
 - exploit 可能會不只一個, width 較長的 exploit 比較容易成功  
 - `cd s2e-last && ls exploit*` (host)
 - _exploit-bfffe818.bin  exploit-bffff819.bin_
4. 用一般的 qemu 再次啟動 guest, 並將 exploit 傳進 guest  
 - `raw_net.sh sample.raw` (host)
 - `scp -P2222 exploit-xxxxxxxx localhost:~/sample.exp` (host)
5. 測試 payload 是不是能成功拿到 shell  
 - `sample/sample $(cat sample.exp)` (guest)

## lab 1-2
**lab 1-2** 將介紹在沒辦法取得原始碼的情況下進行 symbolic execition  
[sample2.c](http://123.tw) 與 sample.c 基本上一模一樣, 除了少 `s2e_make_concolic` 那一行  
本實驗將透過 symbolic wrapper 對受測程式做 symbolic  

1. 開啟 vm (host) 後, 在 `~/crax/script/result` 底下啟動 **CRAX**  
 - `crax.sh crax.s2e demo` (host)
2. 啟動後會用 qemu 開啟一個 debian 的 vm (guest), 在 `~/sample/` 底下執行跑 `run2.sh`, 與 **lab 1-1** 的差別是, 這次透過 `~/wrapper` 底下的 `symarg` make symbolic 間接執行程式   
 - `cd sample && ./run2.sh` (guest)
3. host 上的 **CRAX** 會開始生 exploit, exploit 會在 `~/crax/script/result/s2e-last` 底下  
 - `cd s2e-last && ls exploit*` (host)
 - _exploit-bfffe818.bin  exploit-bffff819.bin_
4. 再次啟動 **CRAX**, 透過 s2eget 指令取得 exploit  
 - `crax.sh crax.s2e demo` (host)
 - `cp exploit-xxxxxxxx ~/crax/share/sample2.exp` (host)
 - `s2eget sample2.exp` (guest)
5. 測試 payload 是不是能成功拿到 shell  
 - `sample/sample2 $(cat sample2.exp)` (guest)

## lab 1-3
*lab 1-3* 將使用 **CRAX** 測試 AIS3 Day 2 的練習題 **easy pwn**  
[stage2](http://123.tw) 是 **Defcon quals circa 2004** 的題目  
本實驗將使用 **CRAX** 自動生出 **easy pwn** 的 payload  

1. 開啟 vm (host) 後, 在 `~/crax/script/result` 底下啟動 **CRAX**  
 - `crax.sh crax.s2e demo` (host)
2. 啟動後會用 qemu 開啟一個 debian 的 vm (guest), 在 `~/easy_pwn/` 底下執行跑 `run.sh`  
 - `cd easy_pwn && ./run.sh` (guest)
3. host 上的 **CRAX** 會開始生 exploit, exploit 會在 `~/crax/script/result/s2e-last` 底下  
 - exploit 可能會不只一個, address 較低的 exploit 比較容易成功  
 - `cd s2e-last && ls exploit*` (host)
 - _exploit-bfffede4.bin  exploit-bffffde5.bin_
4. 再次啟動 **CRAX**, 透過 s2eget 指令取得 exploit  
 - `crax.sh crax.s2e demo` (host)
 - `cp exploit-xxxxxxxx ~/crax/share/stage2.exp` (host)
 - `s2eget stage2.exp` (guest)
5. 測試 payload 是不是能成功拿到 shell  
 - `easy_pwn/stage2 $(cat stage2.exp)` (guest)
6. 將 payload 上傳到題目環境
 - `scp exploit-xxxxxxxx ctf@52.27.26.147:/tmp/your_dir/stage2.exp` (host)
7. 在題目環境測試 payload 是否成功  
 - `ssh ctf@52.27.26.147` (host)
 - `/home/stage2/stage2 $(cat /tmp/your_dir/stage2.exp)` (remote)

## Appendix
前面的 lab 為了節省時間, 因此直接在已經建好 snapshot 的狀況下進行  
這邊補上完整的 s2e 操作流程:

0. 從 github 上下載 s2e (or CRAX) source, 到 `repo/qemu/` 底下編譯  
1. 建立 qemu img, 並安裝 guest os  
2. 用 s2e 編譯過的 qemu 開啟 guest, 並將受測程式上傳到 guest 並建置測試環境  
 - 建議加上 -net 參數方便傳檔及安裝軟體
3. 將剛才的 image 複製一份並以 `.s2e` 做結尾, 再次用 qemu 開啟 guest, 用 qemu 建立 snapshot
 - **此步驟不能加上 -net 參數**  
4. 用 s2e 模式開啟剛剛的 snapshot, 執行受測程式或 wrapper, 測試結果會在 `s2e-out-n` 底下  
5. 如果受測程式本身有 make symbolic, 需用 qemu 啟動 guest 並測試
6. 如果 image 被更動, 須重新建立 snapshot
