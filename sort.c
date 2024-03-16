#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int extraMemoryAllocated;

void *Alloc(size_t sz)
{
	extraMemoryAllocated += sz;
	size_t* ret = malloc(sizeof(size_t) + sz);
	*ret = sz;
	printf("Extra memory allocated, size: %ld\n", sz);
	return &ret[1];
}

void DeAlloc(void* ptr)
{
	size_t* pSz = (size_t*)ptr - 1;
	extraMemoryAllocated -= *pSz;
	printf("Extra memory deallocated, size: %ld\n", *pSz);
	free((size_t*)ptr - 1);
}

size_t Size(void* ptr)
{
	return ((size_t*)ptr)[-1];
}

// implement merge sort
// extraMemoryAllocated counts bytes of extra memory allocated
void mergeSort(int pData[], int l, int r)
{
	// Base Case: Return if the array or subarray is sized for only a single element.
	// Recursive Case: Subdivide the array into halves until the base case is reached, then sort those subarrays,
	// backtracking until the entire array is sorted.
	if (l < r)
    {
		// Compute the middlemost index position of the array.
    	int m = (l + r) / 2;

        // Recursively call mergeSort to sort first the left, then the right halves of the array.
        mergeSort(pData, l, m);
        mergeSort(pData, m + 1, r);
        
        // Merge the sorted left and right halves into a sorted, whole array.
    	// Array Index Variables
    	int i, j, k;
    	// Compute the size of temporary subarray, L. Store as an int, left.
    	int left = m - l + 1;
    	// Size of temporary subarray, R. Store as an int, right.
    	int right = r - m;

    	// Allocate memory for the temporary subarrays, L and R.
    	int *L = Alloc(sizeof(int)*left);
    	int *R = Alloc(sizeof(int)*right);

    	// Copy the corresponding data from the original array over to the subarrays.
    	for (i = 0; i < left; i++)
    	{
			// Fill each i-th entry of the left-half subarray, L, with the data at the "l + i"-th position of the original array.
        	L[i] = pData[l + i];
    	}
    	for (j = 0; j < right; j++)
    	{
			// Fill each j-th entry of the right-half subarray, R, with the data at the "m + 1 + j"-th position of the original array.
        	R[j] = pData[m + 1 + j];
    	}

    	// Fill the original array with the entries from the temp arrays in sorted order
    	// Initialize array index variables
    	i = 0; j = 0; k = l;

    	// While BOTH the left- and right-half subarray have not been COMPLETELY iterated over...
		while (i < left && j < right)
    	{
			// If the i-th entry of L is less than or equal to the j-th entry of R...
        	if (L[i] <= R[j])
        	{
				// The k-th entry of the original array takes on the data stored in the i-th entry of L.
            	pData[k] = L[i];
				// Move along to the next element in the left subarray, loop ends if no next element exists.
            	i++;
        	}

			// Else, the i-th entry of L is greater than the j-th entry of R, so...
        	else
        	{
				// The k-th entry of the original array takes on the data stored in the j-th entry of R.
            	pData[k] = R[j];
				// Move along to the next element in the right subarray, loop ends if no next element exists.
            	j++;
        	}
			// Move along to the next element of the original array.
        	k++;
    	}

    	// It is possible that one subarray was larger in size than the other, so to address these leftover entries...
		// While the left subarray has not been completely iterated over...
    	while (i < left)
    	{
			// Assign the i-th entry of L to the k-th entry of the original array.
        	pData[k] = L[i];
			// Move along to the next elements of the left subarray and original array.
        	i++;
        	k++;
    	}
		// While the right subarray has not been completely iterated over...
    	while (j < right) {
			// Assign the j-th entry of R to the k-th entry of the original array.
        	pData[k] = R[j];
			// Move along to the next elements of the right subarray and original array.
        	j++;
        	k++;
    	}

    	// Deallocate the memory reserved for the temporary subarrays now that they are no longer needed.
    	DeAlloc(L);
    	DeAlloc(R);
    }
}

// parses input file to an integer array
int parseData(char *inputFileName, int **ppData)
{
	FILE* inFile = fopen(inputFileName,"r");
	int dataSz = 0;
	int i, n, *data;
	*ppData = NULL;
	
	if (inFile)
	{
		fscanf(inFile,"%d\n",&dataSz);
		*ppData = (int *)malloc(sizeof(int) * dataSz);
		// Implement parse data block
		if (*ppData == NULL)
		{
			printf("Cannot allocate memory\n");
			exit(-1);
		}
		for (i=0;i<dataSz;++i)
		{
			fscanf(inFile, "%d ",&n);
			data = *ppData + i;
			*data = n;
		}

		fclose(inFile);
	}
	
	return dataSz;
}

// prints first and last 100 items in the data array
void printArray(int pData[], int dataSz)
{
	int i, sz;
	// Compute the range of indices to iterate over
	// If the array contains less than or exactly 100 items, print all those items.
	if (dataSz <= 100) {
		sz = dataSz - 1;
	}

	// Else, print the first 100 items in the array.
	else {
		sz = 100;
	}

	printf("\tData:\n\t");
	for (i=0;i<sz;++i)
	{
		printf("%d ",pData[i]);
	}
	printf("\n\t");
	
	// If the array contains more than 100 items, now print the last 100 items in the array.
	if (dataSz >= 100) {
		for (i=dataSz - 100 - 1;i<dataSz;++i)
		{
			printf("%d ",pData[i]);
		}
	}
	printf("\n\n");
}

int main(void)
{
	clock_t start, end;
	int i;
    double cpu_time_used;
	char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt", "input4.txt" };
	
	for (i=0;i<4;++i)
	{
		int *pDataSrc, *pDataCopy;
		int dataSz = parseData(fileNames[i], &pDataSrc);
		
		if (dataSz <= 0)
			continue;
		
		pDataCopy = (int *)malloc(sizeof(int)*dataSz);
	
		printf("---------------------------\n");
		printf("Dataset Size : %d\n",dataSz);
		printf("---------------------------\n");
		
		printf("Merge Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz*sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		mergeSort(pDataCopy, 0, dataSz - 1);
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n",cpu_time_used);
		printf("\textra memory allocated\t: %d\n",extraMemoryAllocated);
		printArray(pDataCopy, dataSz);
		
		free(pDataCopy);
		free(pDataSrc);
	}
	
}