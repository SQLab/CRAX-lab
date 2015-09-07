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
host & guest 的帳密都是 `ais3 / crax`
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
 - `./sample`
3. host 上的 **CRAX** 會開始生 exploit, exploit 會在 `~/crax/script/result/s2e-last` 底下  
 - exploit 可能會不只一個, address 較低的 exploit 比較容易成功  
 - `cd s2e-last && ls exploit*`
 - _exploit-bfffe818.bin  exploit-bffff819.bin_
4. 再次啟動 guest, 這次用一般的 qemu 啟動, 並將 exploit 傳進 guest  
 - `raw_net.sh sample.raw`
 - `scp -P2222 exploit-xxxxxxxx localhost:~/sample.exp` 
5. 在 guest 測試 payload 是不是能成功拿到 shell  
 - `cd sample && ./sample $(cat ../sample.exp)`

## lab 1-2
**lab 1-2** 將介紹在沒辦法取得原始碼的情況下進行 symbolic execition  
[sample2.c](http://123.tw) 與 sample.c 基本上一模一樣, 除了少 `s2e_make_concolic` 那一行  
本實驗將透過 symbolic wrapper 對受測程式做 symbolic  


1. 開啟 vm (host) 後, 在 `~/crax/script/result` 底下啟動 **CRAX**  
 - `crax.sh crax.s2e demo` (host)
2. 啟動後會用 qemu 開啟一個 debian 的 vm (guest), 在 `~/sample/` 底下執行跑 `run2.sh`  
 - `cd sample && ./run2.sh` (guest)
3. 與 **lab 1-1** 的差別是, 這次透過 `~/wrapper` 底下的 `symarg.c` make symbolic 間接執行程式   
4. 再次啟動 guest, 這次可以直接在 snapshot 測試, 透過 s2eget 指令取得 exploit  
 - `cd s2e-last && cp exploit-xxxxxxxx ~/crax/share/sample2.exp` (host)
 - `s2eget sample2.exp`
5. 在 guest 測試 payload 是不是能成功拿到 shell  
 - `cd sample && ./sample2 $(cat ../sample2.exp)`
