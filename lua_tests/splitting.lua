--ff = '\f'
ff = '\x0c'
etx = '\x03'

-- test string - no "separator" at end of string
local s = "JobName=对人类家庭 生命.qjx"..ff.."a123 nonsense=string"..ff.."of=shit"..etx
print(s)

-- insert "separator" before etx, and remove etx
local new_s = s:sub(1,#s-1) .. ff

function mysplit (inputstr, sep)
  if sep == nil then
    sep = ff
  end
  
  local t={}
  -- ([^\f]+)
  -- ([^=]+)
  for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
    table.insert(t, str)
  end
  return t
end



lines = mysplit(new_s, ff)
for _,line in pairs(lines) do
  
  print('Line: ' .. line)
  k_v = mysplit(line, '=')
  for k,v in pairs(k_v)do
    print(k,v)
  end
  print('-----')
end

