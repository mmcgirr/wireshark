--[[ config.lua - some settings
]]

local config = {}

config.r_sep = {
  ['cr'] = '\x0d',
  ['lf'] = '\x0a',
  ['ff'] = '\x0c',
  ['rs'] = '\x1e',
}

config.f_sep = {
  ['us']  = '\x1f',
  ['gs']  = '\x1d',
  ['fs']  = '\x1c',
  ['tab'] = '\x09',
  ['vt']  = '\x0b',
}

config.packet = {
  ['nul'] = '\x00',
  ['soh'] = '\x01',
  ['stx'] = '\x02',
  ['etx'] = '\x03',
  ['eot'] = '\x04',
  ['enq'] = '\x05',
  ['ack'] = '\x06',
  ['nak'] = '\x15',
  ['syn'] = '\x16',
}

config.misc = {
  ['bel'] = '\x07',
  ['bs']  = '\x08',
  ['so']  = '\x0e',
  ['si']  = '\x0f',
  ['dle'] = '\x10',
  ['dc1'] = '\x11',
  ['dc2'] = '\x12',
  ['dc3'] = '\x13',
  ['dc4'] = '\x14',
  ['etb'] = '\x17',
  ['can'] = '\x18',
  ['em']  = '\x19',
  ['sub'] = '\x1a',
  ['esc'] = '\x1b',
}
return config