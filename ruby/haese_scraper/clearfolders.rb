require 'fileutils'

directories = [
  File.join(Dir.pwd, 'uncroppedScreenshots'),
  File.join(Dir.pwd, 'screenshots'),
  File.join(Dir.pwd, 'customScreenshots')
]

directories.each do |dir|
  if Dir.exist?(dir)
    puts "Emptying directory: #{dir}"
    FileUtils.rm_rf(Dir.glob("#{dir}/*"))
    puts "Directory emptied: #{dir}"
  else
    puts "Directory does not exist: #{dir}"
  end
end

puts 'All specified directories have been processed.'
