def bubbleSort(array):
    
    length = len(array)

    for i in range(length - 1, 0, -1):
        for n in range(i):
            if array[n] > array[n+1]:
                array[n], array[n+1] = array[n+1], array[n]
                print(array)
            else:
                return

array = [4, 3, 2, 1]
print("Pre-run list is: ", array)
bubbleSort(array)
print("Post-run list is: ", array)


