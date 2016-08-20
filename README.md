# AIS3 CRAX lab

[CRAX](https://github.com/SQLab/CRAX) 是 SQLab 所發展的 Automatic Exploit Generator (AEG)，利用 symbolic execution 為基礎，對受測程式進行動態分析。

## 原理
在 symbolic execution 執行中 判斷 eip 是否可由 input 控制，當偵測到 eip 可控( eip tainted )，則將目前收集到的 path constraint(路徑限制式) 加上 shellcode constraint 以 SMT solver 嘗試去解可能的 input。

由於 symbolic execution 會探索程式執行的所有可能路徑，時間複雜度成指數成長，當受測程式規模大時會使執行效率較差。

**CRAX** 採用的 symbolic execution engine 是基於 KLEE 更改來的 **S2E**。

**S2E** 是 select symbolic execution 的縮寫，只對特定的程式區間做 symbolic execution，即 *concolic execution*，以一組隨機的輸入作為初始值開始執行。

## CRAX 架構

Host OS(S2E) 執行 symbolic execution，使用 QEMU 建置 Guest OS 以測試位於各種不同環境上的程式。
S2E 以 API 或允許使用者自訂的 op code(由 S2E 基於它自己的 QEMU 所定義的 machine code) 在 Host OS 和 Guest OS 之間溝通。

## CRAX lab 介紹
---
Host OS 與 Guest OS 的帳密皆為 `ais3 / crax`

---

### Host OS (Ubuntu 12.04 64-bit)
`ais3@ubuntu:`  
`~/crax/build` **CRAX** 的環境建置  
`~/crax/s2e`   由 [CRAX](https://github.com/SQLab/CRAX) 中下載的 source code，詳細編譯方式參考：https://s2e.epfl.ch/  
`~/crax/img`  
1. `crax.raw`：Guest OS image，已放置受測程式。  
2. `crax.s2e`：S2E 需以 .s2e 為副檔名的 image 做 symbolic execution，其與 crax.raw 內容相同。  
3. `crax.s2e.demo`：crax.s2e 的 snapshot(snapshot 取名為 demo 即會產生以 .demo 結尾的檔案)。S2E QEMU 開啟此 snapshot 並開始執行 symbolic execution。  
(參考：[The S2E VM Image Format](https://github.com/dslab-epfl/s2e/blob/master/docs/ImageInstallation.rst#id2))
4. `crax.qcow2`：用來將 Host OS symbolic execution 後產生的 exploit 檔案傳入 Guest OS 進行 exploit 的 image。(由於 crax.raw 更動過後 crax.s2e 會毀壞，因此不能將 exploit 傳入 crax.raw)  

`~/crax/script`  
1. `start.sh`：以 S2E mode 的 QEMU 開啟 Guest OS。  
2. `exploit.sh`：symbolic execution 結束後將 exploit 檔案傳入 Guest OS。以 non-S2E mode 的 QEMU 開啟 Guest OS，並將 Host OS ssh 的 22 port 導至 Guest OS 的 2222 port。  

`~/crax/result`：存放 symbolic execution 結束後的過程紀錄與結果。每結束一次 symbolic execution 就會將最新的紀錄都存放於 `s2e-last` 中。  

### Guest OS (Debian 32-bit)  
`ais@debian:`  
`~/sample`：lab1、2 受測程式與執行的 script。  
`~/easy_pwn`：lab3 受測程式與執行的 script。  
`~/wrapper`：在無法取得原始碼的情況下，以 wrapper 控制 input 為 symbolic variable。  

## CRAX lab 實作  
請開 AIS3_CRAX 資料夾先用 VMware 裝好 vm (只能用 VMware 開唷~)  

### lab 1  
**lab 1** 將介紹利用 S2E 提供的 API 對程式 make symbolic 的流程  
[sample.c](https://github.com/SQLab/CRAX-lab/blob/master/sample/sample.c) 是一個很簡單的 buffer overflow 的小程式  
其中第 12 行 `s2e_make_concolic` 將對某一段 buffer make symbolic  
當 Guest OS 執行到第 12 行就會開始進行 symbolic execution，直到程式結束  
本實驗將展示透過 CRAX 自動生成 exploit  
1. 開啟 Host 後， 在 `ais@ubuntu:~/crax/result` 底下啟動 Guest  
	* (Host) `start.sh crax.s2e` -- 以 S2E mode 的 QEMU 開啟 crax.s2e 的 demo snapshot。  
2. 開啟 Guest 後，在 `ais3@debian:~/sample` 底下執行 `run.sh`   
	* (Guest) `./run.sh` -- 編譯受測程式，輸入 0x1000 個 a 作為 concrete input 執行程式。  
3. Host 上的 **CRAX** 會開始進行 symbolic execution 嘗試解出 exploit，exploit 會在 `~/crax/result/s2e-last` 底下  
	* exploit 可能會不只一個，width 較長的 exploit 比較容易成功。  
	* (Host) `cd s2e-last && ls exploit`  
	* _exploit-bfffe818.bin_ & _exploit-bffff819.bin_  
4. 啟動 Guest，並將 exploit 傳進 Guest  
	* (Host) `exploit.sh crax.qcow2`  
	* (Host 用另一個 terminal)   
		* `~/crax/result/s2e-last$ scp -P2222 exploit-xxxxxxxx localhost:~/sample.exp`  
5. 測試 payload 是不是能成功拿到 shell  
	* (Guest) `sample/sample $(cat sample.exp)`  

### lab 2  
**lab 2** 將介紹在沒辦法取得原始碼的情況下進行 symbolic execition  
[sample2.c](https://github.com/SQLab/CRAX-lab/blob/master/sample/sample2.c) 與 sample.c 基本上一模一樣，除了少 s2e_make_concolic 那一行  
本實驗將透過 symbolic wrapper 對受測程式做 symbolic  
1. 開啟 Host 後， 在 `ais@ubuntu:~/crax/result` 底下啟動 Guest  
	* (Host) `start.sh crax.s2e` -- 以 S2E mode 的 QEMU 開啟 crax.s2e 的 demo snapshot。  
2. 開啟 Guest 後，在 `ais3@debian:~/sample` 底下執行 `run2.sh`   
	* (Guest) `./run2.sh` -- 以 wrapper 執行程式。  
3. Host 上的 **CRAX** 會開始進行 symbolic execution 嘗試解出 exploit，exploit 會在 `~/crax/result/s2e-last` 底下  
	* (Host) `cd s2e-last && ls exploit`  
	* _exploit-bfffe818.bin_ & _exploit-bffff819.bin_  
4. 啟動 Guest，並將 exploit 傳進 Guest  
	* (Host) `exploit.sh crax.qcow2`  
	* (Host 用另一個 terminal)   
		* `~/crax/result/s2e-last$ scp -P2222 exploit-xxxxxxxx localhost:~/sample2.exp`  
5. 測試 payload 是不是能成功拿到 shell  
	* (Guest) `sample/sample2 $(cat sample2.exp)`  

### lab 3  
[stage2](https://github.com/SQLab/CRAX-lab/blob/master/sample/stage2) 是 Defcon quals circa 2004 的題目  
本實驗將使用 CRAX 自動生成 stage2 的 payload  
1. 開啟 Host 後， 在 `ais@ubuntu:~/crax/result` 底下啟動 Guest  
	* (Host) `start.sh crax.s2e` -- 以 S2E mode 的 QEMU 開啟 crax.s2e 的 demo snapshot。  
2. 開啟 Guest 後，在 `ais3@debian:~/easy_pwn` 底下執行 `run.sh`   
	* (Guest) `./run.sh` -- 以 wrapper 執行程式。  
3. Host 上的 **CRAX** 會開始進行 symbolic execution 嘗試解出 exploit，exploit 會在 `~/crax/result/s2e-last` 底下  
	* exploit 可能會不只一個，address 較低的 exploit 比較容易成功。  
	* (Host) `cd s2e-last && ls exploit`  
	* _exploit-bfffede4.bin_ & _exploit-bffffde5.bin_  
4. 啟動 Guest，並將 exploit 傳進 Guest  
	* (Host) `exploit.sh crax.qcow2`  
	* (Host 用另一個 terminal)   
		* `~/crax/result/s2e-last$ scp -P2222 exploit-xxxxxxxx localhost:~/stage2.exp`  
5. 測試 payload 是不是能成功拿到 shell  
	* (Guest) `easy_pwn/stage2 $(cat stage2.exp)`  
