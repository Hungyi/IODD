IODD
====

source code
主程式是chsong.c而它有自動include lookdir.h和stsong.h，所以最後只要編譯主程式即可
 掃描目錄: lookdir.h
 鏈結串列(Songstruct)的結構和insert節點的函式為 stsong.h

用法: 只要在主程式呼叫   lookdir(要搜尋目錄的路徑名) 即能傳回一個鏈結串列的指標，而該串列的每個節點都存該目錄下的檔案名


問題:  只需focus在主程式chsong.c中的get_pthread(void *arg),程式有註解，能成功做出鏈結串列(每個node的song_name欄位儲存檔案的內容)，而在fgets之後 node->song_name就消失了
只需解這個問題，其他都沒問題
