#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <mpi.h>
#include <time.h>

#define ARRAYSIZE 20

using namespace std;

int BinarySearch(int start, int end, int searchNumber, int *data)
{
	int mid;
	int low = start;
	int high = end;

	while (low <= high)
	{
		mid = (low + high) / 2;
		if (data[mid] == searchNumber)
			return mid; // found, return the index

		if (data[mid] < searchNumber)
			low = mid + 1;
		else if (data[mid] > searchNumber)
			high = mid - 1;
	}

	// not found
	return -1;
}

void printResult(int id, int processors, int index, int searchNumber, int &notfound)
{
	if (index == -1) { // not found
		notfound++;
		cout << "id " << id << " didn't found number " << searchNumber << endl;
	}
	else // found
		cout << "id " << id << " found number " << searchNumber << " at index: " << index << endl;
}

int main(int argc, char* argv[])
{
	int i, data[ARRAYSIZE];
	int id, processors;

	// Start Parallel
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &processors);
	int blockSize = ARRAYSIZE / processors; // membagi size array menjadi beberapa bagian untuk dicari setiap processor

	if (id == 0) // root initialize data and send it to other processors
	{
		// Initialize data with increasing/sorted random number
		srand(time(0));
		cout << endl << "Data Array:" << endl;
		cout << "[index] = value" << endl;

		data[0] = rand() % 100;
		cout << "[0] = " << data[0] << endl;
		for (i = 1; i < ARRAYSIZE; i++) {
			data[i] = data[i - 1] + (rand() % 100) + 1;
			cout << "[" << i << "] = " << data[i] << endl;
		}

		// Send the data to each processors
		for (i = 1; i < processors; i++) {
			MPI_Send(&data, ARRAYSIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
	else { // other processor receive the data
		MPI_Recv(&data, ARRAYSIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	while (1) // loop untuk mencari angka berkali-kali
	{
		int searchNumber;
		int result = -1;
		int notfound = 0;
		if (id == 0) {
			// root meminta input angka yang ingin dicari
			cout << endl << "Input Search Number: ";
			cin >> searchNumber;

			// lalu dikirimkan ke setiap processor
			for (i = 1; i < processors; i++) {
				MPI_Send(&searchNumber, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}

			if (searchNumber < 0) // handling root untuk keluar dari loop
				break;

			// root menjalankan function binary search dari index 0 hingga index blocksize - 1
			result = BinarySearch(id * blockSize, (id * blockSize) + blockSize - 1, searchNumber, data);
			printResult(id, processors, result, searchNumber, notfound); // lalu print hasilnya

			// root menerima hasil dari setiap processor dan print hasilnya
			for (i = 1; i < processors; i++) {
				MPI_Recv(&result, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printResult(i, processors, result, searchNumber, notfound);
			}

			// jika setiap processor tidak menemukan angka yang dicari
			if (notfound == processors)
				cout << "Number " << searchNumber << " not found." << endl;
		}
		else { 
			// processor lainnya menerima angka yang ingin dicari
			MPI_Recv(&searchNumber, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			if (searchNumber < 0) // handling processor lain untuk keluar dari loop
				break;

			// processor lainnya melakukan proses binary search sesuai block nya masing-masing
			
			// if disini untuk handling jika pembagian block masih tersisa
			// misal: arraysize = 20, tapi hanya pakai 3 processor (20%3 = 2), berarti tersisa 2 index terakhir tidak ada yg menelusuri
			// jadi, khusus processor terakhir, mencari hingga index terakhir array nya
			if(id == processors-1) 
				result = BinarySearch(id * blockSize, ARRAYSIZE - 1, searchNumber, data);
			else
				result = BinarySearch(id * blockSize, (id * blockSize) + blockSize - 1, searchNumber, data);
			
			
			MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // lalu mengirimkan hasilnya ke root processor
		}
	}

	// End parallel
	MPI_Finalize();

	return 0;
}
