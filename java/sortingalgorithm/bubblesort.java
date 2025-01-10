public class bubblesort {
	public static void main(String args[])
	{
		int[] array = {3, 6, 8, 1, 2, 4};


		for (int n = array.length - 2; n > 0; n--) {
			for (int i = 0; i < n; i++) {
				if (array[n] > array[n+1]) {
					int temp = array[i];
					array[i] = array[i+1];
					array[i+1] = temp;
				}
			}
		}

		for (int i : array) {
			System.out.print(i + " ");
		}

	}
}
