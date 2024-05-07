-- load some data

-- separate the data on some criteria
-- "record separator", "field separator"
-- store separated data in table as rows


---[[
data = "2P0000000010      05002          ***************************ECRWSH**C-022OXKK01 1 1 002 GOR2 FZAQB KETTERING-PECKOR CURRENT RESIDENT42 BLACKHAWK CLUB CTDANVILLE CA 94506-451304714800HUMAN READABLE DO NOT PRINT - 1027110025006979455994506451342FFDFFTFTATADDTTDTAAAAFDAFTADDTDAAATAFFAAFTAFDFDTTADFFATTDAADAADFT94622P0000000020      05009          ***************************ECRWSH**C-022BRENDA KETTERING-PECKOR CURRENT RESIDENT42 BLACKHAWK CLUB CTDANVILLE CA 94506-4513KEYCODE:442B124NOXKK0112009MEE1FZAQHUMAN READABLE DO NOT PRINT - 1027110921050735696494506451342DDAADAADADFFATFDAATDTFFFTAFDADAAFDFAATFTDFTTAAAFFAAADFADFTFADAATD98062P0000000030      05013          ***********************ECRWSH**C-022BRENDA K PECKOR CURRENT RESIDENT42 BLACKHAWK CLUB CTDANVILLE CA 94506-4513OXKK0113013POL1FZAQHUMAN READABLE DO NOT PRINT - 1027110025014174103694506451342DTFDDDTFFDTAFFFAFFFDDTFFDFFDAFDTDDFTTDDDTFTATAFFFDFTADTAATDFFDTAF9808"
--]]

function read_file(f)
  --local size = 2^8 --512b buffer
  
  local my_f = assert(io.open(f, "rb"))

  -- read the whole file
  local t = my_f:read("*all")
  
  --my_f:seek('set') --reset seek to file start
  my_f:close()
  return t
end

function parse_data(d, rec_sep, fld_sep)
  local records = {}
  local rec_count = 0
  
  -- check to make sure the separator is in the data at least once
  if string.find(d, rec_sep) == nil then
    print("Error: " .. rec_sep .. " not in data!")
    return -1
  else
    -- separator exists, insert the record into the table
    for record in data:gmatch( "[^" ..rec_sep.. "]+" ) do
      rec_count = rec_count + 1
      --table.insert(records, count, rec_sep .. record)
      records[rec_count] = {}
      records[rec_count].data = rec_sep .. record
      records[rec_count].fields = {}
      
      -- split the fields off into the fields table
      local fld_count = 0
      for field in records[rec_count].data:gmatch( "[^" .. fld_sep .. "]+" ) do
        fld_count = fld_count + 1
        records[rec_count].fields[fld_count] = field
        
      end    
    end      
  end
  
  print('\n' .. rec_count .. " Records Inserted")
  --print('\n' .. fld_count .. " Fields")
  return records
end

--require("nonsense")
file="./lua_tests/test_data/fasttech/VIPNT_TEST.txt"

--data = read_file(file)
rsep='\x1e' -- RS
fsep='\x1d' -- GS
records = parse_data(data, rsep, fsep)

if #records >= 1 then
  for i=1,#records,1 do
    print("\nRecord " .. i .. ":\n" .. records[i].data)
    for j=1,#records[i].fields,1 do
      print("Field " .. j .. ": " .. records[i].fields[j])
    end
  end
end

--print("Last Record:\n" .. records[table.maxn(records)])
--[[
for k,v in ipairs(records) do 
  print("Index: " .. k .. "\tRecord Data:\n" .. v)
end
]]