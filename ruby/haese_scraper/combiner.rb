require 'combine_pdf'
require 'fileutils'

screenshotDir = File.join(Dir.pwd, 'screenshots')
outputDir = File.join(Dir.pwd, 'output')

unless Dir.exist?(screenshotDir)
  puts "Directory '#{screenshotDir}' does not exist."
  exit
end

FileUtils.mkdir_p(outputDir)

screenshotFiles = Dir.entries(screenshotDir).select { |file| file.match(/screenshot_\d+\.png/) }

screenshotFiles.sort_by! do |filename|
  filename.match(/screenshot_(\d+)\.png/)[1].to_i
end

pdf = CombinePDF.new

page = 1
total_pages = screenshotFiles.size

screenshotFiles.each do |screenshot|
  print "\rCollating page #{page}/#{total_pages}"
  screenshotPath = File.join(screenshotDir, screenshot)

  tempPDFpath = screenshotPath.gsub('.png', '.pdf')

  system("magick #{screenshotPath} #{tempPDFpath}")

  pdf << CombinePDF.load(tempPDFpath)

  File.delete(tempPDFpath) if File.exist?(tempPDFpath)

  page += 1
end

puts "\nCollation complete."

outputBaseName = 'output'
outputIndex = 1
outputPDFpath = File.join(outputDir, "#{outputBaseName}_#{outputIndex}.pdf")

while File.exist?(outputPDFpath)
  outputIndex += 1
  outputPDFpath = File.join(outputDir, "#{outputBaseName}_#{outputIndex}.pdf")
end

pdf.save(outputPDFpath)

puts "PDF compiled and saved as #{outputPDFpath}."
