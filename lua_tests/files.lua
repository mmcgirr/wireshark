local cc = require("config")

local function read_file(file)
  local size = 2^8 --512b buffer
  
  local myfile = assert(io.open(file, "rb"))

  -- read the whole file
  local t = myfile:read("*all")
  
  myfile:seek('set') --reset seek to file start

  myfile:close()
  return t
end

local data = read_file("./test_data/fasttech/VIPNT_TEST.txt")
--print(data)

--[[
for k,v in string.gmatch(data, cc.controls['rs']) do
  print('found rs @ ' .. r )
end

for k,_ in pairs(cc.r_sep) do 
  local count = 0
  
  for yes in string.gmatch(data, cc.r_sep[k]) do
    count = count + 1
  end
  
  if count > 0 then
    print("Found " .. count ..' ' .. k .. ' items in data')
  end
  
end
]]

-- basic - only gives you the substring
-- doesn't tell you where it starts or ends
local function basic(sep)
  print("Basic example - no indexing")
  for substring in data:gmatch(cc.r_sep[sep]) do
    print(substring)
  end
end


local function more_advanced(sep)
  print("More complex example - first and last index of sub within string")
  -- one method, prints where substring starts and ends
  local first, last = 0, 0
  while true do
    first, last = data:find(cc.r_sep[sep], first+1)
    if not first then break end
    print(data:sub(first, last), first, last)
  end
end

local function advanced_recurse(sep)
  print("Recursive example of above")
  -- same as above, but as a recursive function
  -- that takes a callback and calls it on the result so 
  -- that it can be reused
  local function find(str, substr, callback, init)
    init = init or 1
    local first, last = str:find(substr, init)
  
    if first then
      --callback(str, first, last)
      callback("\tFound at byte #: " .. first)
      return find(str, substr, callback, last+1)
    end
  end
  
  find(data, cc.r_sep[sep], print)
end

for k, _ in pairs(cc.r_sep) do
  print("Next record separator ")
  print("Searching for ... " .. k)
  basic(k)
  more_advanced(k)
  advanced_recurse(k)
  print("-----")

end