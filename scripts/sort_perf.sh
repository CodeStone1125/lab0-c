#!/bin/bash

# 只測量 CPU 執行時間（latency）
event="-e task-clock"

# 定義要測試的排序方法
sort_methods=("-t" "-s" "-m")

# 輸出結果的檔案
output_file="sort_result.csv"

# 清空舊的結果檔案，準備寫入新的資料
echo -e "# Length TreeSort SedimentSort LXSort" > $output_file

# 遍歷 100000 到 200000，每次增加 10000
for length in $(seq 100000 1000 200000); do
    # 記錄每個長度的結果
    result_line="$length"
    
    # 測試每個排序方法並記錄結果
    for method in "${sort_methods[@]}"; do
        # 執行 perf 測量並抓取 task-clock，將命令寫入 ./qtest 的 cmd> 中
        task_clock=$(perf stat -e task-clock ./qtest <<EOF |& grep -a "task-clock" | awk '{print $1}' | sed 's/,//g'
new
ih RAND $length
c_sort $method
EOF
)

        # 將結果附加到該長度的行
        result_line="$result_line $task_clock"
    done

    # 將該長度的測試結果寫入文件
    echo -e "$result_line" >> $output_file
done

echo "Data written to $output_file"
