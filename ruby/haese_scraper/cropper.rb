require 'fileutils'

uncroppedScreenshotDir = File.join(Dir.pwd, 'uncroppedScreenshots')
screenshotDir = File.join(Dir.pwd, 'screenshots')
FileUtils.mkdir_p(screenshotDir)

unless Dir.exist?(uncroppedScreenshotDir)
  puts "Directory '#{uncroppedScreenshotDir}' does not exist."
  exit
end

puts 'Found directory'

uncroppedScreenshotFiles = Dir.entries(uncroppedScreenshotDir).select do |file|
  file.match(/screenshot_\d+\.png/)
end

uncroppedScreenshotFiles.sort_by! do |filename|
  filename.match(/screenshot_(\d+)\.png/)[1].to_i
end

removeLeft = 233
removeRight = 233
removeTop = 16
removeBottom = 134

page = 1
total_pages = uncroppedScreenshotFiles.size

uncroppedScreenshotFiles.each do |screenshot|
  uncroppedPath = File.join(uncroppedScreenshotDir, screenshot)
  croppedPath = File.join(screenshotDir, screenshot)
  FileUtils.cp(uncroppedPath, croppedPath)

  info = `magick identify -format "%wx%h" #{uncroppedPath}`
  originalWidth, originalHeight = info.split('x').map(&:to_i)

  newWidth = originalWidth - removeLeft - removeRight
  newHeight = originalHeight - removeTop - removeBottom
  system("magick #{croppedPath} -crop #{newWidth}x#{newHeight}+#{removeLeft}+#{removeTop} #{croppedPath}")

  print "\rCropped page #{page}/#{total_pages}"
  page += 1
end

puts "\nCropping complete."
