-- another splitter test
function string:split(sSeparator, nMax, bRegexp)
   assert(sSeparator ~= '')
   assert(nMax == nil or nMax >= 1)

   local aRecord = {}

   if self:len() > 0 then
      local bPlain = not bRegexp
      nMax = nMax or -1

      local nField, nStart = 1, 1
      local nFirst,nLast = self:find(sSeparator, nStart, bPlain)
      while nFirst and nMax ~= 0 do
         aRecord[nField] = self:sub(nStart, nFirst-1)
         nField = nField+1
         nStart = nLast+1
         nFirst,nLast = self:find(sSeparator, nStart, bPlain)
         nMax = nMax-1
      end
      aRecord[nField] = self:sub(nStart)
   end

   return aRecord
end

function read_file(f)
  --local size = 2^8 --512b buffer
  
  local my_f = assert(io.open(f, "rb"))

  -- read the whole file
  local t = my_f:read("*all")
  
  --my_f:seek('set') --reset seek to file start
  my_f:close()
  return t
end


file="./lua_tests/test_data/fasttech/VIPNT_TEST.txt"
data = read_file(file)

the_tbl = string.split(data, '\x1e')

for k,v in pairs(the_tbl) do
  if v == '' then 
    --print("first record shenanigans")
  else
    print("\nRecord Data: \n" .. v)
      
      fields = string.split(v, '\x1d')
      print("Field data:")
      for k,v in ipairs(fields) do
        print("Field Len: ".. #v .. "\tF# " .. k .. " : " .. v)
      end
  end
    
end
