# Parallel_Computing_Binary_Search_Algorithm

Implementasi algoritma binary search pada parallel computing menggunakan C++
untuk project kampus mata kuliah "Pemrosesan Data Terdistribusi"

Penggunaan:
Debug [ParallelBinarySearch.cpp](https://github.com/lieahau/Parallel_Computing_Binary_Search/blob/master/ParallelBinarySearch.cpp) untuk menghasilkan file ParallelBinarySearch.exe

lalu dengan CMD:
mpiexec -n jumlahProcessorYangInginDipakai path_to_exe/ParallelBinarySearch.exe

contoh:
mpiexec -n 5 ParallelBinarySearch.exe

