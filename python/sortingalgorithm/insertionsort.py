def insertionSort(array):
    n = len(array)

    if n <= 1:
        return

    for i in range(1, n):
        key = array[i]
        j = i-1
        while j >= 0 and key < array[j]:
            array[j+1] = array[j]
            j -= 1
        array[j+1] = key

array = [3, 6, 8, 4, 5]
insertionSort(array)
print(array)
