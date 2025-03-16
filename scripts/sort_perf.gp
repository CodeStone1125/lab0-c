set title "Sorting Methods Comparison"
set xlabel "Length"
set ylabel "Time (ms)"
set grid
set key right top

# 設定 y 軸範圍（延遲時間範圍）
set yrange [500:2000]

# 設定輸出為 PNG 圖片
set terminal png
set output 'sort_comparison.png'

# 讀取檔案，並根據列數畫出每個排序方法的折線
plot "sort_result.csv" using 1:2 with lines title "Tree Sort", \
     "sort_result.csv" using 1:3 with lines title "Sediment Sort", \
     "sort_result.csv" using 1:4 with lines title "List Sort"
