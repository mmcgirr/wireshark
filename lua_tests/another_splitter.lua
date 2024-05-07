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

data = "2P0000000010      05002          ***************************ECRWSH**C-022OXKK01 1 1 002 GOR2 FZAQB KETTERING-PECKOR CURRENT RESIDENT42 BLACKHAWK CLUB CTDANVILLE CA 94506-451304714800HUMAN READABLE DO NOT PRINT - 1027110025006979455994506451342FFDFFTFTATADDTTDTAAAAFDAFTADDTDAAATAFFAAFTAFDFDTTADFFATTDAADAADFT94622P0000000020      05009          ***************************ECRWSH**C-022BRENDA KETTERING-PECKOR CURRENT RESIDENT42 BLACKHAWK CLUB CTDANVILLE CA 94506-4513KEYCODE:442B124NOXKK0112009MEE1FZAQHUMAN READABLE DO NOT PRINT - 1027110921050735696494506451342DDAADAADADFFATFDAATDTFFFTAFDADAAFDFAATFTDFTTAAAFFAAADFADFTFADAATD98062P0000000030      05013          ***********************ECRWSH**C-022BRENDA K PECKOR CURRENT RESIDENT42 BLACKHAWK CLUB CTDANVILLE CA 94506-4513OXKK0113013POL1FZAQHUMAN READABLE DO NOT PRINT - 1027110025014174103694506451342DTFDDDTFFDTAFFFAFFFDDTFFDFFDAFDTDDFTTDDDTFTATAFFFDFTADTAATDFFDTAF9808"

--for k,v in next, string.split(data, '\x1e') do print(k,v) end
the_tbl = string.split(data, '\x1e')

print(#the_tbl)
for k,v in pairs(the_tbl) do 
  print("\nRecord Data: \n" .. v)
    
    fields = string.split(v, '\x1d')
    print("Field data:")
    for k,v in ipairs(fields) do
      print("Field Len: ".. #v .. "\tF# " .. k .. " : " .. v)
    end
end
