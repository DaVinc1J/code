you might need to fuck around with the ruby gems to get it to work because i cant remember what ones i used

ensure you have firefox installed and a haese login with the books you wanna scrape, often schools will have one with a majority of the books and you can find out the login details.

1. use fullbook.rb to scan the entire book:
  - you must keep it focused on the firefox window otherwise it will take too long to load and wont screenshot properly
  - make sure the page is clearly visible in the window, if it is cut off by say the bottom row it will not screenshot properly

2. check to see what pages were not screenshotted properly, this can be done by looking at what files in the /uncroppedScreenshots directory are smaller than the others and looking at that page number

3. use specificpage.rb to scan each specific page you need replaced while using the id system that it directs you with (self explanatory)
  - make sure the dimensions you used with fullbook.rb is the same as specificpage.rb

4. use cropper.rb to trim down the sides and top, this might need adjusting the file as need
  - you can edit the top, bottom, left and right margins inside the file

5. use combiner.rb to get the final .pdf

6. use clearfolders.rb at the end to empty all folders before you do another bookw