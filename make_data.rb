if ARGV.size == 0
  puts "specify data size."
  exit 1
end

length = ARGV[0].to_i
if length <= 0
  puts "specify positive data size."
  exit 1
end

puts length
length.times do
  puts rand(-length..length)
end
